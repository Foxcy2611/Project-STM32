#ifndef __LCD16X2_H
#define __LCD16X2_H


#include "stm32f10x.h"


void LCD_Init(void);
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_Clear(void);
void LCD_GotoXY(uint8_t x, uint8_t y);
void LCD_Puts(char *str);


#endif
