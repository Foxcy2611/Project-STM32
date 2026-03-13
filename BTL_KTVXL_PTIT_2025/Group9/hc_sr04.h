#ifndef __HC__SR04
#define __HC__SR04

#include "stm32f10x.h"                  // Device header

#define TRIG_Pin GPIO_Pin_0
#define ECHO_Pin GPIO_Pin_1

void HC_SR04_Config(void);
float HC_SR04_Read(void);


#endif
