#ifndef __LCD_I2C_H
#define __LCD_I2C_H

#include "stm32f10x.h"
#include "i2c.h"

// I2C address c?a LCD (thu?ng 0x27)
#define LCD_ADDR 0x27

// Backlight và control
#define LCD_BACKLIGHT 0x08
#define ENABLE 0x04
#define RS_CMD 0x00
#define RS_DATA 0x01

// LCD Commands (page 24 HD...)
#define LCD_CLEARDISPLAY 0x01 
#define LCD_RETURNHOME 0x02 
#define LCD_ENTRYMODESET 0x04 
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETDDRAMADDR 0x80

// Flags
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

#define LCD_4BITMODE 0x00
#define LCD_1LINE 0x00
#define LCD_2LINE 0x08

// Hàm LCD
void LCD_Init(uint8_t lines);
void LCD_Clear(void);
void LCD_Home(void);
void LCD_noDisplay(void);
void LCD_display(void);
void LCD_noBlink(void);
void LCD_blink(void);
void LCD_noCursor(void);
void LCD_Cursor(void);
void LCD_scrollDisplayRight(void);
void LCD_scrollDisplayLeft(void);
void LCD_SetEntryMode(uint8_t leftToRight, uint8_t shiftDisplay);
void LCD_SetCursor(uint8_t col, uint8_t row);
void LCD_SendString(const char *str);
void LCD_SendData(uint8_t data);
void LCD_SendCmd(uint8_t cmd);

#endif
