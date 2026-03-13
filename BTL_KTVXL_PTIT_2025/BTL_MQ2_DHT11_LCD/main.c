#include "stm32f10x.h"

#include "stdio.h"

#include "delay.h"
#include "mq2.h"
#include "lcd_i2c.h"
#include "i2c.h"
#include "dht11.h"
#include "buzzer.h"


/*
COI BUZZER PA3
LCD SCL PB6
		SDA PB7
MQ2 PA0
DHT11 PA1

*/


uint8_t DHT11_Data[5];

int main(){
	TIM2_Init();
	ADC_Config();
	
	LCD_Begin(20, 4);
	
	Buzzer_Config();
	char bf[100];
	char dh[100];

	while(1){
		uint8_t err = DHT11_ReadData(DHT11_Data);
		if(err == 0){
			sprintf(dh, "Temp = %d", DHT11_Data[2]);
			LCD_SetCursor(0, 1);
			LCD_Print(dh);
		} else {
			LCD_SetCursor(0, 4);
			LCD_Printf("ERROR DHT11");
		}

		uint16_t adc = ADC_Read();
		sprintf(bf, "GAS  = %d", adc);
		
		LCD_SetCursor(0, 0);
		LCD_Print(bf);
		
		if(adc > 2000){
			Buzzer_On();
			LCD_SetCursor(0, 3);
			LCD_Print("WARNING");
		} else {
			Buzzer_Off();
			LCD_SetCursor(0, 3);
			LCD_Print("NOT_GAS");
		}
		
		delay_ms(1000);
	}
}
