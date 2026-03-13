#ifndef UART_H
#define UART_H

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

typedef struct 
{
    char cmd[64];
    uint32_t value;
} Command;

void UART_Init(void);
void UART_SendChar(char c);
void UART_SendString(char *str);
void UART_SendInfor(void);

uint8_t UART_IsMessageReady(void);
char* UART_GetMessage(void);
void UART_Process(Command *cmd);


#endif

