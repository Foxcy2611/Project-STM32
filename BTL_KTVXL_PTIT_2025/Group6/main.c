#include "uart.h"
#include "lcd.h"
#include "Gpio.h"
#include "max.h"
#include "timer2.h"
#include "flash.h"
#include "string.h" 
#include "stdio.h" 

uint32_t last_check_sensor = 0;
bool flag_check_req = false;
uint32_t last_req_sensor = 0;
uint16_t time_update_interval = 500;
Command herculesCmd = {" ", 0};

int main()
{
	UART_Init();
	MAX30102_Init();
	LCD_Init();
	Timer2_InitForMillis();

	uint32_t live_bpm = 0;

	while (1)
	{
		live_bpm = MAX30102_GetBPM();

		if (UART_IsMessageReady())
		{
			UART_Process(&herculesCmd);
			if (strcmp(herculesCmd.cmd, "HEART_RATE_REQUES") == 0)
			{
				LCD_Clear();
				flag_check_req = true;
				last_req_sensor = getMilliseconds();
			}
			else if (strcmp(herculesCmd.cmd, "SET_HEART_RATE_INTERVAL") == 0)
			{
				FLASH_Write16(ADD_PAGE_LAST_FLASH, (uint16_t)herculesCmd.value);
			}
			else if (strcmp(herculesCmd.cmd, "INFO_REQUEST") == 0)
			{
				UART_SendInfor();
			}
		}

		if (flag_check_req)
		{
			if (getMilliseconds() - last_req_sensor >= 30000) 
			{
				flag_check_req = false;
				UART_SendString("ERROR: Sensor Timeout or No Finger\n");
				LCD_Clear();
				LCD_PrintString("Timeout!");
			}

			if (MAX30102_HasFinger())
			{
				if (live_bpm > 0) 
				{
					printf("Your HR is: %u BPM\n", live_bpm);

					LCD_Clear();
					LCD_SetCursor(0, 0);
					LCD_PrintString("Sent via UART!");

					flag_check_req = false; 
				}
			}
			else 
			{
				if(getMilliseconds() % 500 < 50) { 
					LCD_SetCursor(0, 0);
					LCD_PrintString("Put hand on....");
				}
			}
		}

		uint16_t interval = FLASH_Read(ADD_PAGE_LAST_FLASH);
		if (interval == 0xFFFF) interval = 500; 

		if (getMilliseconds() - last_check_sensor >= interval && flag_check_req == false)
		{
			last_check_sensor = getMilliseconds();

			LCD_SetCursor(0, 0);
			LCD_PrintString("Monitor Mode    ");
			LCD_SetCursor(0, 1);

			if (MAX30102_HasFinger()) {
				char buff[16];
				if(live_bpm > 0) sprintf(buff, "BPM: %u", live_bpm);
				else sprintf(buff, "Detecting....  ");

				LCD_Clear();
				LCD_PrintString(buff);
				UART_SendString(buff);
				UART_SendString("\r\n");
			
			} else {
				LCD_SetCursor(0, 0);
				LCD_PrintString("No Finger     ");
			}
		}
	}
}
