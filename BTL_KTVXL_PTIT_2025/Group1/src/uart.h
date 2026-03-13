#ifndef __UART__H
#define __UART__H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART

#include "Fl_Tim.h"
#include "oled.h"

#include "stdio.h"
#include "string.h"
#include <stdlib.h>
#include "stdbool.h"
#include <stdint.h>

typedef struct {
    char Name[64];
    uint16_t Value;
} Command_t;

typedef struct {
	char Name_Group[20];
	char Member[5][30];
	char Name_Project[40];
	char Hardware_Used[5][20];
	char Software_Used[3][20];
	char Date[10];
} Group1;


void Command_Init(void);
void USART_SendChar(char c);
void USART_SendString(char *str);
bool UART_MessIsReady(void);
bool Command_Decode(Command_t *cmd);
void Group_Print(const Group1 *g);
void Group_FillInfo(Group1 *g);
void USART_SendCheck(uint16_t as, uint16_t value);
void USART_SendDone(uint8_t Mode);
void USART_SendParameter(void);

#endif
