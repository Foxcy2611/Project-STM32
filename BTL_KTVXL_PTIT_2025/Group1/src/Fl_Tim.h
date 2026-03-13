#ifndef __Fl__Tim__H
#define __Fl__Tim__H

#include "stm32f10x.h"                  // Device header

#include "stdbool.h"

#define FLASH_PAGE_ADDR 0x0800FC00

void TIM2_Init(void);
uint32_t millis(void);
void Delay_ms(uint32_t ms);

void Flash_Save(uint16_t threshold, uint16_t time);
void Flash_Read(uint16_t *threshold, uint16_t *time);
void Flash_Erase(void);

#endif
