#ifndef __ST7735__H
#define __ST7735__H

#include "stm32f10x.h"                  // Device header

#include "delay.h"

#define ST7735_WIDTH  160
#define ST7735_HEIGHT 128

#define REG_SWRESET 0x01
#define REG_SLPOUT  0x11
#define REG_DISPON  0x29

#define REG_CASET   0x2A
#define REG_RASET   0x2B
#define REG_RAMWR   0x2C

#define REG_MADCTL  0x36
#define REG_COLMOD  0x3A

#define REG_GMCTRP1 0xE0
#define REG_GMCTRN1 0xE1

#define RGB_BLACK   0x0000
#define RGB_BLUE    0x001F
#define RGB_RED     0xF800
#define RGB_GREEN   0x07E0
#define RGB_CYAN    0x07FF
#define RGB_BROWN   0xA145
#define RGB_MAGENTA 0xF81F // Tim hong
#define RGB_PINK    0xFC90
#define RGB_YELLOW  0xFFE0
#define RGB_WHITE   0xFFFF
#define RGB_ORANGE  0xF940

#define TFT_SPI SPI1
#define TFT_PORT GPIOA

#define TFT_A0  GPIO_Pin_2
#define TFT_RST GPIO_Pin_3
#define TFT_CS  GPIO_Pin_4
#define TFT_SCK GPIO_Pin_5
#define TFT_SDA GPIO_Pin_7

#define CS_LOW()   GPIO_ResetBits(TFT_PORT, TFT_CS)
#define CS_HIGH()  GPIO_SetBits(TFT_PORT, TFT_CS);

#define A0_CMD()   GPIO_ResetBits(TFT_PORT, TFT_A0)
#define A0_DATA()  GPIO_SetBits(TFT_PORT, TFT_A0)

#define RST_LOW()  GPIO_ResetBits(TFT_PORT, TFT_RST)
#define RST_HIGH() GPIO_SetBits(TFT_PORT, TFT_RST)

void SPI_Config(void);
void SPI_SendByte(uint8_t byte);
void SPI_WriteCmd(uint8_t cmd);
void SPI_WriteData(uint8_t data);
void SPI_Reset(void);

void ST7735_Init(void);
void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void ST7735_FillScreen(uint16_t color);
void ST7735_DrawPixel(uint8_t x, uint8_t y, uint16_t color);
void ST7735_DrawImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint16_t* data);
void ST7735_DrawImageStruct(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t* data);

#endif
