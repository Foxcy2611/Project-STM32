#ifndef __UART__H
#define __UART__H

#include "stm32f10x.h"
#include "ds1307.h"
#include <stdio.h>

extern DS1307_TIME_t tempTime;

void UART1_Config(void);
void UART_SendString(char *str);
uint8_t UART_Check(void);

char* UART_Check_And_Parse(void);

#endif
