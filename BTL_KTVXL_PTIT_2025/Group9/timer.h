#ifndef TIMER_H
#define TIMER_H

#include "stm32f10x.h"
#include <stdbool.h>

void TIM2_Init(uint16_t period_ms);
bool TIM2_GetFlag(void);
void TIM2_SetFlag(bool flag_state);

#endif