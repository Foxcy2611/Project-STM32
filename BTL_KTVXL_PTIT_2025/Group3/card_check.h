#ifndef __CARD__CHECK__H
#define __CARD__CHECK__H

#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include <stdbool.h>
#include <stdint.h>


typedef enum
{
	STATE_IDLE = 0,
	STATE_REQUEST_CARD,
	STATE_CARD_CHECK,
} SystemState;


typedef struct {
	char Name_Group[20];
	char Member[5][30];
	char Name_Project[40];
	char Hardware_Used[5][20];
	char Software_Used[3][20];
	char Date[15];
} Group3;

void USART_Config(void);
void USART_SendString(char *str);
uint32_t CARD_GetPendingUsername(void);

SystemState CARD_Process(void);

void CARD_SetState(SystemState new_state);

void Group_Print(const Group3 *g);
void Group_FillInfo(Group3 *g);

#endif
