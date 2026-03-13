#include "Fl_Tim.h"

volatile uint32_t uwTick = 0; 

void TIM2_Init(void){
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_TimeBaseInitTypeDef tim;
  tim.TIM_ClockDivision = TIM_CKD_DIV1;
  tim.TIM_CounterMode   = TIM_CounterMode_Up;
  
  tim.TIM_Prescaler     = 72 - 1;   
  tim.TIM_Period        = 1000 - 1; // 1 lan tran = 1ms
  tim.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &tim);

  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
  NVIC_InitTypeDef nvic;
  nvic.NVIC_IRQChannel = TIM2_IRQn;
  nvic.NVIC_IRQChannelCmd = ENABLE;
  nvic.NVIC_IRQChannelPreemptionPriority = 1;
  nvic.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&nvic);

  TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void){
  if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    uwTick++;
  }
}

uint32_t millis(void){
  return uwTick;
}

void Delay_ms(uint32_t ms){
	uint32_t start = millis();
	while(millis() - start < ms);
}

void Flash_Save(uint16_t threshold, uint16_t time){

  FLASH_Unlock();

  FLASH_ClearFlag(FLASH_FLAG_EOP | 
									FLASH_FLAG_PGERR | 
									FLASH_FLAG_WRPRTERR);
  FLASH_ErasePage(FLASH_PAGE_ADDR);

  FLASH_ProgramHalfWord(FLASH_PAGE_ADDR, threshold);
  FLASH_ProgramHalfWord(FLASH_PAGE_ADDR + 2, time);

  FLASH_Lock();
}
 // 550 =>0x226
// 1 => 0x01
void Flash_Read(uint16_t *threshold, uint16_t *time){
  *threshold = *(uint16_t *)(FLASH_PAGE_ADDR);
  *time     = *(uint16_t *)(FLASH_PAGE_ADDR + 2);


  if (*threshold == 0xFFFF || *threshold == 0) *threshold = 300;
  if (*time == 0xFFFF || *time == 0) *time = 5;
}

void Flash_Erase(void){
	FLASH_Unlock();
	FLASH_ErasePage(FLASH_PAGE_ADDR);
	FLASH_Lock();
}
