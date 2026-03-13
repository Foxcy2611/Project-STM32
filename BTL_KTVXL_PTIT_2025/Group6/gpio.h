#ifndef GPIO_H
#define GPIO_H
#include "stm32f10x.h"


void Gpio_Config(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t GPIO_Mode, uint8_t GPIO_Speed);

#endif
