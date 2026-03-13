#include "hc_sr05_interval.h"

static bool flagCheck = false; 

void HCSR05_Interval_Init(uint16_t interval_ms)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_BaseInitStruct;
	TIM_BaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_BaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseInitStruct.TIM_Prescaler = 7200 - 1;
	TIM_BaseInitStruct.TIM_Period = (interval_ms * 10) - 1;
	TIM_BaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_BaseInitStruct);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM3, ENABLE);
}

void TIM3_IRQHandler(void);
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		if (flagCheck == false)
		{
			flagCheck = true;
		}
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

bool HCSR05_Interval_GetUpdateFlag(void)
{
	return flagCheck;
}

void HCSR05_Interval_SetFlag(bool stt)
{
	flagCheck = stt;
}
