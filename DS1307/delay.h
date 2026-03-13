#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM

void TIM2_Delay(void);
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);

#endif
