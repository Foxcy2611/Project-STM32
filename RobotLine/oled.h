#ifndef OLED_H
#define OLED_H

#include "stm32f10x.h"
#include <stdbool.h>
#include <string.h>
#include "setup.h"
#include "stdio.h"

#define OLED_I2C_ADDR    0x78  
#define OLED_WIDTH       128
#define OLED_HEIGHT      64
 
static void I2C1_Config(void);
static void I2C_Write(uint8_t address, uint8_t control, uint8_t data);
static void OLED_SendCommand(uint8_t cmd);
static void OLED_SendData(uint8_t data);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_UpdateScreen(void);
void OLED_DrawPixel(uint8_t x, uint8_t y, bool color);
void OLED_GotoXY(uint8_t x, uint8_t y);
void OLED_Puts(char* str);
void OLED_Fill(uint8_t color);
void DisplayLoading_White(void);
void DisplayLoading_Black(void);
void OLED_DrawBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t w, uint8_t h);
void OLED_B_FINAL(void);
void OLED_START(void);

#endif
