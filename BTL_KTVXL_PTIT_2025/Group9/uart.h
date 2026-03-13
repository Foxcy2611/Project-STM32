#ifndef UART_H
#define UART_H

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void UART1_Init(uint32_t baudrate);
void UART1_SendChar(char c);
void UART1_SendString(char* s);
bool UART1_ReceiveComplete(void);
char* UART1_GetString(void);


#endif