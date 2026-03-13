#include "stm32f10x.h"                  // Device header
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C

#include "delay.h"
#include "ds1307.h"
#include "uart.h"
#include "stdio.h"

int main(){
	TIM2_Delay();
	USART_Config();
	DS1307_Config();
	
	DS1307_TIME_t setT;
  setT.hour = 14; 
  setT.min  = 16;
  setT.sec  = 00;
  setT.date = 21;
  setT.month = 11;
  setT.year = 25;
  setT.day  = 6; 
  DS1307_SetTime(&setT);
	
	DS1307_TIME_t now;
	
	char b[100];
	while(1){
		DS1307_GetTime(&now);
		sprintf(b, "Time: %02d:%02d:%02d  -  Date: %02d/%02d/20%02d\n", 
                now.hour, now.min, now.sec, 
                now.date, now.month, now.year
		);
		USART_SendString(b);
		
		delay_ms(1000);
	}
	
}
