#ifndef __MQ2__H
#define __MQ2__H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_adc.h"              // Keil::Device:StdPeriph Drivers:ADC

void ADC_Config(void);
uint16_t ADC_Read(void);

#endif
