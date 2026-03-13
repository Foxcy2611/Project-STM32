#ifndef __SETUP__H
#define __SETUP__H

#include "stm32f10x.h"
#include "motor.h"
#include "adc_dma.h"
#include "pid.h"
#include "ir_data.h"
#include "delay.h"

#define base_speed 84.0f
#define max_speed 100.0f


extern uint16_t ADC_data[8];
extern uint16_t white[8];
extern uint16_t black[8];


void Setup_All(void);
void Following_Line(void);
void Calib_White(void);
void Calib_Black(void);
void IR_PID_Init(void);

#endif
