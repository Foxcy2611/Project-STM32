#include "stm32f10x.h"
#include "stm32f10x_flash.h"

#include "gp2y.h"
#include "uart.h"
#include "flash.h"
#include "lcd.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

// cau truc lenh nhan qua uart
Command U_cmd = {" ", 0};

// nguong canh bao pm2.5 luu trong flash
static uint16_t pm_Val = 50;

// gia tri bui mg/m3
static float dust;

int main(){
	TIM2_Delay();      // khoi tao delay bang timer2
	USART_Config();    // khoi tao uart
	GP2Y_Init();       // khoi tao cam bien bui
	LCD_Init();        // khoi tao lcd 16x2
	
	LCD_Clear();
	USART_SendString("SYSTEM READY...\r\n");

	// doc nguong pm tu flash
	pm_Val = Flash_Read(FLASH_ADDR);

	// neu flash chua ghi bao gio thi tra ve 0xFFFF ? lay gia tri mac dinh 50
	if(pm_Val == 0xFFFF) 
		pm_Val = 50;
	
  while(1){

		// kiem tra xem da nhan du tin nhan uart chua
		if(USART_MessReady()){
			USART_Process(&U_cmd);   // tach lenh tu chuoi
				
				// lenh dat nguong pm25 moi
			if(strcmp(U_cmd.cmd, "SET_PM25_THRESHOLD") == 0){
				pm_Val = (uint16_t)U_cmd.value;  // cap nhat gia tri moi
				Flash_Write(FLASH_ADDR, pm_Val);  // ghi vao flash
				USART_SendString("THRESHOLD UPDATED.\r\n");
			} 
				
			// lenh yeu cau gui thong tin
			else if(strcmp(U_cmd.cmd, "INFO_REQUEST") == 0){
				USART_SendInfor();
			}
		}

        // doc cam bien, tra ve don vi ug/m3
    dust = GP2Y_GetMgm3();

			// hien thi len dong 1 lcd
		char bff[30];
		sprintf(bff, "PM2.5: %.1f ug  ", dust); 
		
		LCD_SetCursor(0, 0); 
		LCD_PrintString(bff);
		
		// neu bui vuot nguong thi canh bao
		if(dust > pm_Val){
			LCD_SetCursor(1, 0); 
			LCD_PrintString("WARNING !!      ");

			// gui canh bao qua uart
			USART_SendString("WARNING: PM2.5 Exceeded Limit!\r\n");
		} 
		
		// neu khong vuot nguong thi hien thi nguong pm
		else {
			LCD_SetCursor(1, 0); 
			sprintf(bff, "Limit: %u ug    ", pm_Val);
			LCD_PrintString(bff);
		}
		
		Delay_ms(1000);  // cap nhat moi 1 giay
	}
}
