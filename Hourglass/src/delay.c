#include "delay.h"

static volatile uint32_t sysTick_ms = 0;

void Delay_Init(void) {
  SysTick_Config(SystemCoreClock / 1000);
}

void SysTick_Handler(void) {
  sysTick_ms++;
}

uint32_t millis(void) {
  return sysTick_ms;
}

void Delay_ms(uint32_t ms) {
  uint32_t start = millis();
  while ((millis() - start) < ms);
}


