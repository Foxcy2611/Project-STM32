#include "hc_sr04.h"

static void Systick_Config_Static(void){
  SysTick->LOAD = 0xFFFFFF; 
  SysTick->VAL = 0; 
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

static void delay_us_static(uint32_t us){
  uint32_t start = SysTick->VAL;
  uint32_t ticks = us * 72; 
    
  while((start - SysTick->VAL) < ticks);
}

void HC_SR04_Config(void){
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitTypeDef gp;
    
  gp.GPIO_Mode = GPIO_Mode_Out_PP;
  gp.GPIO_Pin = TRIG_Pin;
  gp.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &gp);
    
  gp.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  gp.GPIO_Pin = ECHO_Pin;
  GPIO_Init(GPIOB, &gp);
    
  Systick_Config_Static();
}

float HC_SR04_Read(void){
  GPIO_SetBits(GPIOB, TRIG_Pin);
  delay_us_static(15);
  GPIO_ResetBits(GPIOB, TRIG_Pin);
   
  uint32_t timeout = 0;
  while(GPIO_ReadInputDataBit(GPIOB, ECHO_Pin) == 0) {
    timeout++;
    if(timeout > 1000000) return -1.0; 
  }
    
  uint32_t start_tick = SysTick->VAL; 
    
  timeout = 0;
  while(GPIO_ReadInputDataBit(GPIOB, ECHO_Pin) == 1) {
    timeout++;
    if(timeout > 1000000) return -1.0; 
  }
    
  uint32_t end_tick = SysTick->VAL;
    
	uint32_t ticks_elapsed = start_tick - end_tick;
    
  float time_us = (float)ticks_elapsed / 72.0;
    
  return time_us / 58.0;
}
