#ifndef __UART__H
#define __UART__H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART

void USART_Config(void);
void USART_SendChar(char c);
void USART_SendString(char *str);

#endif
