#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"                  // Device header


void delay_init(void);
void SysTick_Handler(void);
void delay_ms(uint32_t ms);

#endif
