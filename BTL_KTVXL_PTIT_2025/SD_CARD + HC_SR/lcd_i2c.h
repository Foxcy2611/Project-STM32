#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// ----- Public API -----
// Ham khoi tao LCD I2C voi so cot va so hang duoc chi dinh
void LCD_Begin(uint8_t cols, uint8_t rows);
// Ham xoa toan bo noi dung tren man hinh LCD
void LCD_Clear(void);
// Dua con tro hien thi ve dau dong hien tai (vi tri 0,0)
void LCD_Home(void);
// Bat tat man hinh
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
// Bat tat con tro
void LCD_CursorOn(void);
void LCD_CursorOff(void);
// Bat tat blink con tro
void LCD_BlinkOn(void);
void LCD_BlinkOff(void);
// Dich man hinh
void LCD_ScrollLeft(void);
void LCD_ScrollRight(void);
// Set vi tri hien thi
void LCD_SetCursor(uint8_t col, uint8_t row);
// In du lieu ra ngoai man hinh
void LCD_PrintChar(char kitu);
void LCD_Print(const char *str);
void LCD_Printf(const char *format, ...);
// Thiet lap che do ghi cho LCD
void LCD_SetEntryMode(uint8_t leftToRight, uint8_t shiftDisplay);

#endif
