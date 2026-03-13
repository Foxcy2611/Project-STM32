#ifndef TIMER2_H
#define TIMER2_H

#include "stm32f10x.h"
#include <stdint.h>

void Timer2_InitForMillis(void);
uint32_t getMilliseconds(void);
//void delayMilliseconds(uint32_t ms);

#endif