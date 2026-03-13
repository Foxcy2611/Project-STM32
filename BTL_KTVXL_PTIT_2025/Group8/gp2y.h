#ifndef __GP2Y__H
#define __GP2Y__H

#include "stm32f10x.h"

void TIM2_Delay(void);
void Delay_us(uint16_t us);
void Delay_ms(uint16_t ms);

void GP2Y_Init(void);
uint16_t GP2Y_Read(void);
float GP2Y_Caculate(float volt);
float GP2Y_GetMgm3(void);

#endif
