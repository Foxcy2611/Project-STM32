#ifndef __DELAY__H
#define __DELAY__H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM

void Delay_Init(void);
uint32_t millis(void);
void delay_ms(uint32_t ms);


#endif
