#ifndef __LCD__I2C__H
#define __LCD__I2C__H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C
#include "stm32f10x_tim.h"


#define I2C_SCL_Pin GPIO_Pin_6
#define I2C_SDA_Pin GPIO_Pin_7

#define LCD_ADDR 0x27
#define LCD_BACKLIGHT 0x08
#define ENABLE 0x04
#define RS_CMD 0x00
#define RS_DATA 0x01

#define LCD_CLEARDISPLAY 0x01 
#define LCD_DISPLAYCONTROL 0x08
#define LCD_FUNCTIONSET 0x20
#define LCD_SETDDRAMADDR 0x80


#define LCD_DISPLAYON 0x04
#define LCD_CURSOROFF 0x00
#define LCD_BLINKOFF 0x00

#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08

void LCD_SendData(uint8_t data);
void LCD_SendCmd(uint8_t cmd);
void LCD_Clear(void);
void LCD_Display(void);
void LCD_Init(void);
void LCD_GotoXY(uint8_t x, uint8_t y);
void LCD_Puts(char *str);

#endif
