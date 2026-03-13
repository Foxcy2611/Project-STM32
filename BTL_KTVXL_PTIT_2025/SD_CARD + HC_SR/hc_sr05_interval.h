#ifndef HC_SR05_INTERVAL_H
#define HC_SR05_INTERVAL_H

#include "stm32f10x_tim.h"
#include <stdint.h>
#include <stdbool.h>

void HCSR05_Interval_Init(uint16_t interval_ms);
bool HCSR05_Interval_GetUpdateFlag(void);
void HCSR05_Interval_SetFlag(bool stt);

#endif
