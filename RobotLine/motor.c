#include "motor.h"
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "fmath.h"

uint16_t prd;

void Motor_Config(uint32_t period, uint32_t prescale){
	GPIO_InitTypeDef gp;
	TIM_TimeBaseInitTypeDef TM;
	TIM_OCInitTypeDef TO;
	
	prd = period;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA |
												 RCC_APB2Periph_TIM1, ENABLE);
	
	gp.GPIO_Mode = GPIO_Mode_AF_PP;
	gp.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 |
								GPIO_Pin_10 | GPIO_Pin_11;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gp);
	
	TM.TIM_ClockDivision = TIM_CKD_DIV1;
	TM.TIM_CounterMode = TIM_CounterMode_Up;
	TM.TIM_Period = period - 1;
	TM.TIM_Prescaler = prescale - 1;
	TM.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TM);
	
	TIM_OCStructInit(&TO);
	TO.TIM_OCMode = TIM_OCMode_PWM1;
	TO.TIM_OCPolarity = TIM_OCPolarity_High;
	TO.TIM_OutputState = TIM_OutputState_Enable;
	TO.TIM_OutputNState = TIM_OutputNState_Enable;
	TO.TIM_Pulse = 0;
	
	TIM_OC1Init(TIM1, &TO);
	TIM_OC2Init(TIM1, &TO);
	TIM_OC3Init(TIM1, &TO);
	TIM_OC4Init(TIM1, &TO);

	
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void Motor_Controll(float left_level, float right_level)
{
	left_level = constrainf(left_level, -100.0, 100.0);
	right_level = constrainf(right_level, -100.0, 100.0);
	
	if(left_level >= 0)
	{
		TIM1 -> CCR1 = 0;
		TIM1 -> CCR2 = (uint16_t)(prd*left_level/100);
	}
	else
	{
		TIM1 -> CCR1 = (uint16_t)(-prd*left_level/100);
		TIM1 -> CCR2 = 0;
	}
	
	if(right_level >= 0)
	{
		TIM1 -> CCR3 = 0;
		TIM1 -> CCR4 = (uint16_t)(prd*right_level/100);
	}
	else
	{
		TIM1 -> CCR3 = (uint16_t)(-prd*right_level/100);
		TIM1 -> CCR4 = 0;
	}
}
