#ifndef __OLED__H
#define __OLED__H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C

#include "stdbool.h"
#include "string.h"
#include "stdio.h"

#include "i2c.h"

#define OLED_I2C_ADDR 0x3C << 1
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

void OLED_WriteData(uint8_t data);
void OLED_WriteCommand(uint8_t cmd);

void OLED_Config(void);

void OLED_Clear(void);
void OLED_UpdateScreen(void);

void OLED_DrawPixel(uint8_t x, uint8_t y, bool color);

void OLED_GotoXY(uint8_t x, uint8_t y);
void OLED_Puts(char* str);

void OLED_Show(uint16_t as);

#endif
