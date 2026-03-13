#include "stm32f10x.h"
#include "delay.h"

void TIM2_Init(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef TM;
	
	TM.TIM_ClockDivision = TIM_CKD_DIV1;
	TM.TIM_CounterMode = TIM_CounterMode_Up;
	TM.TIM_Period = 0xFFFF;
	TM.TIM_Prescaler = 71;
	TIM_TimeBaseInit(TIM2, &TM);
	
	TIM_Cmd(TIM2, ENABLE);
}

void delay_us(uint16_t us){
	TIM_SetCounter(TIM2, 0);
	while(TIM_GetCounter(TIM2) < us);
}

void delay_ms(uint16_t ms){
	while(ms--){
		delay_us(1000);
	}
}
