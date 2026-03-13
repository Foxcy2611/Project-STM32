#include "stm32f10x.h"
#include "delay.h"
#include "ds1307.h"
#include "uart.h"
#include "flash.h"
#include <stdio.h>
#include <string.h>

int main(){
	TIM2_Delay();
	UART1_Config();
	DS1307_Config();

	DS1307_TIME_t current;
	int timer_count = 0;

	UART_SendString("SYSTEM READY.\n");

	while(1){
		if(UART_Check()){

			char* cmd_status = UART_Check_And_Parse();

			if(cmd_status != NULL) {
				if(strcmp(cmd_status, "SET_TIME") == 0) {

					DS1307_SetTime(&tempTime);
					Flash_Write_Time(&tempTime);
					UART_SendString("OK: Updated & Saved.\n");
				} else if (strcmp(cmd_status, "INFO_REQUEST") == 0) {
					UART_SendString("INFO: Group 1 - Embedded STM32\n");
				
				} else {
					UART_SendString("ERR: Unknown Command.\n");
				}
			}
		}
		delay_ms(10);
		timer_count++;
		if(timer_count >= 100) { 
			timer_count = 0;
			DS1307_GetTime(&current);
			char buff[50];
			sprintf(buff, "CURRENT_TIME:%02d:%02d:%02d\r\n", 
							current.hour, current.min, current.sec);
			UART_SendString(buff);
		}
	}	
}
