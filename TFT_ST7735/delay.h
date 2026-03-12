#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

void TIM2_Delay(void);
void Delay_ms(uint16_t ms);
void Delay_us(uint16_t us);

#endif
