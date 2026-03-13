#include "hc_sr04.h"
#include "led_7.h"
#include "uart.h"
#include "timer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stm32f10x_flash.h>

/*
	Giả sử bể nước cao 2m, cảm biến siêu âm đặt sát nắp bể.
	Khi cảm biến đo được khoảng cách D, thì mức nước = 200 - D (cm).
*/
#define WATER_LEVEL_MAX 200				// Chiều cao tổng của bể tính theo cm

void Flash_Write_Uint16(uint32_t addr, uint16_t data);
uint16_t Flash_Read_Uint16(uint32_t addr);

int main()
{
	SevenSeg_Init();			// Khởi tạo 7 đoạn hiển thị
	HC_SR04_Config();			// Cấu hình cảm biến siêu âm HC-SR04
	UART1_Init(115200);			// Khởi tạo UART1 tốc độ 115200 bps
	TIM2_Init(500);				// Khởi tạo Timer2 với chu kỳ 500ms

	while (1)
	{
		// Kiểm tra cờ Timer
		if (TIM2_GetFlag())
		{
			float distance = HC_SR04_Read();								// Đọc khoảng cách từ cảm biến
			 // Nếu giá trị đo hợp lệ và <= 200 cm
			if (distance != -1 && distance <= 200.0f)
			{
				float water_level = WATER_LEVEL_MAX - distance;				// Tính mức nước còn lại
				ON_OFF_Relay(water_level, Flash_Read_Uint16(0x0801FC00));	// Điều khiển relay bật/tắt theo mức nước
				SevenSeg_SetNumber(water_level);							// Hiển thị mức nước lên LED 7 đoạn
				printf("Distance: %.2f\n", (double)water_level);			// Hiển thị mức nước lên LED 7 đoạn
				// Cảnh báo mức nước thấp
				if (water_level < Flash_Read_Uint16(0x0801FC00))
				{
					UART1_SendString("WARNING: Water Level Low! Pump Activated\n");
				}
			}
			TIM2_SetFlag(false);			// Reset cờ timer
		}
		
		// Xử lý dữ liệu khi đã nhận đủ
		if (UART1_ReceiveComplete())
		{
			char cmd[64];
			unsigned int value = 0;
			char *str = UART1_GetString();
			
			printf("Received: %s\n", str);
			
			// Tách chuỗi dạng "COMMAND: value"
			if (sscanf(str, " %[^:]: %u", cmd, &value) == 2)
			{

			}
			else
			{
				// Nếu không có giá trị đi kèm, chỉ lấy lệnh
				strcpy(cmd, str);
				value = 0.0f;
			}
			printf("CMD received: %s\n", cmd);
			printf("Value received: %u\n", value);
			
			// Set mực nước tối thiểu
			if (strcmp(cmd, "SET_WATER_LEVEL_MIN") == 0)
			{
				Flash_Write_Uint16(0x0801FC00, (uint16_t)value);
				printf("Da ghi va flash: %u\n", Flash_Read_Uint16(0x0801FC00));
			}
			// Trả về thông tin dự án
			else if (strcmp(cmd, "INFO_REQUEST") == 0)
			{
				char buffer[200];
	
				snprintf(buffer, sizeof(buffer),
					"GROUP_NAME: Embedded Team\n"
					"MEMBERS: Van An, Van Hung, Van Long\n"
					"PROJECT_NAME: Smart Light System\n"
					"HARDWARE_USED: STM32, BH1750, LCD16x2\n"
					"SOFTWARE_USED: Keil, Cmsis Library\n"
					"DATE: 2025-03-25\r\n"
				);
				
				UART1_SendString(buffer);
			}
		}
	}
}

void Flash_Write_Uint16(uint32_t addr, uint16_t data)
{
	FLASH_Unlock();						// Mở khóa flash
	FLASH_ErasePage(addr);				// Xóa page chứa địa chỉ cần ghi
	FLASH_ProgramHalfWord(addr, data);	// Ghi 16-bit vào flash
	FLASH_Lock();						// Khóa flash lại
}

// Đọc giá trị ở địa chỉ flash
uint16_t Flash_Read_Uint16(uint32_t addr)
{
	uint16_t raw = *(volatile uint16_t*)addr;
	
	
	if (raw == 0xFFFF) return 0;
	
	return raw;
}
