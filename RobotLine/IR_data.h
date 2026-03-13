#ifndef __IR__DATA__H
#define __IR__DATA__H

#include "stm32f10x.h"

#define IR_COUNT 8

typedef struct {
	uint16_t raw[IR_COUNT]; // gtri ADC
	uint16_t norm[IR_COUNT];// ADC = 0 - 1
	float last_position;
	float position;
} IRSensor_TypedefStruct;

void IRSensor_SetCalibration(uint16_t black[], uint16_t white[]);
void IRSensor_Read(IRSensor_TypedefStruct *ir, uint16_t *ADC_data);
float IRSensor_GetPosition(IRSensor_TypedefStruct *ir);

#endif
