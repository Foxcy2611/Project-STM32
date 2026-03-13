#ifndef LCD_H
#define LCD_H

#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_PrintString(char *str);
void LCD_Printf(uint32_t value);


#endif
