#ifndef __UART_H
#define __UART_H

#include "stm32f10x.h"
#include <stdio.h>

void USART_Config(void);
void USART_SendChar(char c);
void USART_SendString(char *str);
void USART_SendHex(uint8_t byte);

#endif
