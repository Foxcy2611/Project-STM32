#ifndef MY_HCSR05_H
#define MY_HCSR05_H

#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

// Khoi tao HCSR05
void HCSR05_Init(void);
// Tao xung o chan Trig
void HCSR05_Trigger(void);
// Lay khoang cach
float HCSR05_GetDistance(void);
// Bao da thuc hien do khang cach xong
bool HCSR05_IsReady(void);

#endif
