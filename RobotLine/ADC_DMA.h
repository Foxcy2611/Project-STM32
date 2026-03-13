#ifndef ADC__DMA__H
#define ADC__DMA__H

#include "stm32f10x.h"

extern uint16_t ADC_data[8];

void ADC_Config(void);
void DMA_Config(void);
void DMA_ADC_Init(void);

#endif
