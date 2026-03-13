#ifndef __UART__H
#define __UART__H

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
typedef struct {
	char cmd[60];
	uint32_t value;
} Command;

void USART_Config(void);
void USART_SendChar(char c);
void USART_SendString(char *str);

bool USART_MessReady(void);
void USART_SendInfor(void);
void USART_Process(Command *cmd);

#endif
