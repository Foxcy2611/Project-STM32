#include "delay.h"

static volatile uint32_t g_millis_ticks = 0;

void Delay_Init(void) {
  TIM_TimeBaseInitTypeDef TM;
  NVIC_InitTypeDef NV;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TM.TIM_Prescaler = 71; 
  TM.TIM_Period = 999;   
  TM.TIM_ClockDivision = TIM_CKD_DIV1;
  TM.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TM);

  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NV.NVIC_IRQChannel = TIM2_IRQn;
  NV.NVIC_IRQChannelPreemptionPriority = 1;
  NV.NVIC_IRQChannelSubPriority = 0;
  NV.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NV);

  TIM_Cmd(TIM2, ENABLE);
}


void TIM2_IRQHandler(void) {
  if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    g_millis_ticks++; 
  }
}

uint32_t millis(void){
  return g_millis_ticks;
}


void delay_ms(uint32_t ms) {
  uint32_t start = millis();
  while ((millis() - start) < ms) {
  }
}
