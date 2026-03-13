#ifndef __MOTOR__H
#define __MOTOR__H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM


static __inline float constrainf(float value, float min, float max);
void Motor_Config(uint32_t period, uint32_t prescale);
void Motor_Controll(float left, float right);

#endif 
