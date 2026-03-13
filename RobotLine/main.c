#include "stm32f10x.h"                  // Device header
#include "stm32f10x_adc.h"              // Keil::Device:StdPeriph Drivers:ADC
#include "stm32f10x_dma.h"              // Keil::Device:StdPeriph Drivers:DMA
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART

#include "stdio.h"
#include "adc_dma.h"
#include "pid.h"
#include "delay.h"
#include "motor.h"
#include "setup.h"
#include "oled.h"

#define BTN_CAL GPIO_Pin_4
#define START GPIO_Pin_5


int mode = 0; // 1 calib white, 2 calib black, 3 start 
int cal_step = 0;
float pos;
float correction_;

void BUTTON_Config(void){
	GPIO_InitTypeDef gp;
	// 4 5 b
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	gp.GPIO_Mode = GPIO_Mode_IPU;
	gp.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB, &gp);
}

uint8_t Read_Button(uint16_t PIN){
	return (GPIO_ReadInputDataBit(GPIOB, PIN) == RESET);
}
//////////////////// MAIN //////////////////

int check(void){
	int cnt = 0;
	for(int i = 0 ; i < 8 ; i++){
		if(ADC_data[i] < 1000){
			cnt++;
		}
	}
	if(cnt == 8) return 1;
	else return 0;
}

int main(){
	Setup_All();
	BUTTON_Config();
	OLED_Init();
	OLED_START();
	
	while(1){
		if(Read_Button(BTN_CAL)){
			if(cal_step == 0){
				Calib_White();
				DisplayLoading_White();
				cal_step = 1;
				mode = 1;
				delay_ms(300);
			} else if (cal_step == 1){
				Calib_Black();
				DisplayLoading_Black();
				cal_step = 2;
				mode = 2;
				delay_ms(300);
			}
		}
		if(Read_Button(START) && cal_step == 2){
			IR_PID_Init();
			OLED_B_FINAL();
			mode = 3;
			delay_ms(300);
		}
		if(mode == 3){
			Following_Line();
		}

	
	}
	
}
