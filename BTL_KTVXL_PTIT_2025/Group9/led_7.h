#ifndef __LED_7_H
#define __LED_7_H

#include "stm32f10x.h"

void SevenSeg_Init(void);
void SevenSeg_SetNumber(float num);
void SevenSeg_Update(void);
void ON_OFF_Relay(float water_level, float water_level_max);

#endif
