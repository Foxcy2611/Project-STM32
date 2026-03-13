#ifndef __OLED__H
#define __OLED__H

#include "stm32f10x.h"
#include "i2c.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"

#define OLED_I2C_ADDR 0x78
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

void I2C1_Start(uint8_t address, uint8_t direction);
void I2C1_Write(uint8_t data);
void I2C1_Stop(void);
void OLED_WriteCommand(uint8_t cmd);
void OLED_WriteData(uint8_t data);
void OLED_Clear(void);
void OLED_UpdateScreen(void);
void OLED_DrawPixel(uint8_t x, uint8_t y, bool color);
void OLED_GotoXY(uint8_t x, uint8_t y);
void OLED_Puts(char* str);
void OLED_DrawBitmap(uint8_t x,uint8_t y,const uint8_t* bitmap,uint8_t w,uint8_t h);
void OLED_Init(void);
void OLED_B_FINAL(void);

#endif
