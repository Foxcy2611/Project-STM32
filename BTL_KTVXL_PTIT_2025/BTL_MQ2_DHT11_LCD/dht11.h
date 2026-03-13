#ifndef __DHT11__H
#define __DHT11__H


#include "stm32f10x.h"                  // Device header
#include "delay.h"


void DHT11_SetOutput(void);
void DHT11_SetInput(void);
uint8_t DHT11_ReadPin(void);
uint8_t DHT11_ReadData(uint8_t *buf);


#endif
