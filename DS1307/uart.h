#ifndef __UART__H
#define __UART__H

#include "stm32f10x.h"

void USART_Config(void);
void USART_SendString(char *str);
void USART_SendChar(char c);

#endif
