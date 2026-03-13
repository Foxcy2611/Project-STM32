#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef enum {
	LCD_STATE_NONE = 0,
	LCD_STATE_SAFE,
	LCD_STATE_WARNING
} LCD_State_t;

// ----- Public API -----
void LCD_Begin(uint8_t cols, uint8_t rows);
void LCD_Clear(void);
void LCD_Home(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_CursorOn(void);
void LCD_CursorOff(void);
void LCD_BlinkOn(void);
void LCD_BlinkOff(void);
void LCD_ScrollLeft(void);
void LCD_ScrollRight(void);
void LCD_SetCursor(uint8_t col, uint8_t row);
void LCD_PrintChar(char kitu);
void LCD_Print(const char *str);
void LCD_Printf(const char *format, ...);
void LCD_SetEntryMode(uint8_t leftToRight, uint8_t shiftDisplay);

void LCD_CreateChar(uint8_t index, uint8_t bitmap[8]);
void LCD_WriteChar(uint8_t index);

// ----- API Warning -----
void LCD_CheckGasWarning(uint16_t gas_level, uint16_t threshold);

#endif
