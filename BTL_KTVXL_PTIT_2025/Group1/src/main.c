/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

Bai tap lon 
Ky thuat Vi Xu Ly Nhom 1

	## De bai:
       Thiet ke he thong do luong anh sang bang STM32F103C8T6 voi BH1750
			 Hien thi qua OLED 128x64 va led 7 doan 4 so
			 Thiet lap nguong anh sang qua UART va hien thi led canh bao muc an toan hoac vuot nguong
			 Gui du lieu nguong anh sang theo chu ky qua UART
			 
			 Linh kien / Chuc nang       |  Ngoai vi giao tiep | Chan su dung
			 
		-	 STM32F103C8
		
		-	 5461-AS                            GPIO 						  a-g: PA0-PA6
																					    						  DIG1-DIG4: PB12-PB15 
																														
		-	 BH1750: 												    I2C						  	SCL: PB6
																					    					  	SDA: PB7
																														
		-	 OLED: 													    I2C 					  	SCL: PB6
																				    							  SDA: PB7
																														
		-	 Bo delay ms noi bo							    TIMER
		
		-	 Thiet lap nguong anh sang:         UART					  	TX: PA9
																												  	RX: PA10
																																																										  	
																														
		-  Cai dat thong so										FLASH

==> OK

*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stm32f10x.h"

#include "bh1750.h"
#include "Fl_Tim.h"
#include "i2c.h"
#include "oled.h"
#include "led_buz.h"
#include "uart.h"

Command_t cmd;
Group1 G_1;

uint16_t Set_Light = 0;
uint16_t Set_Check = 0;

uint32_t time_prv = 0;

int main(){
	SystemInit();   

	TIM2_Init(); 
	Command_Init();    
	I2C_Config();
	BH1750_Config(); 
	OLED_Config();
	SEG_BUZ_Config();
	Group_FillInfo(&G_1);

	Flash_Read(&Set_Light, &Set_Check);

	while(1){
		
		uint32_t now = millis();
		
		if(UART_MessIsReady()){
			if(Command_Decode(&cmd)){
				
				if(strcmp(cmd.Name, "SET_LIGHT_THRESHOLD") == 0){
					Set_Light = cmd.Value;
					Flash_Save(Set_Light, Set_Check);

					USART_SendDone(1);
				} else if(strcmp(cmd.Name, "SET_CHECK_INTERVAL") == 0){
					Set_Check = cmd.Value; 
					Flash_Save(Set_Light, Set_Check);

					USART_SendDone(2);
				} else if(strcmp(cmd.Name, "INFO_REQUEST") == 0){
					Group_Print(&G_1);

				} else if(strcmp(cmd.Name, "INFO_PARAMETER") == 0){
					USART_SendParameter();
				} else {
					USART_SendDone(3);
				}
			}
		
		}
		
		if(now - time_prv >= Set_Check * 1000){
			time_prv = millis(); 

			uint16_t light = BH1750_ReadLight();

			OLED_Clear(); 
			OLED_Show(light);
			LED_Number = light;

			USART_SendCheck(light, Set_Light);
			OLED_UpdateScreen();
		}
		
	}
	
}
