#ifndef __MAX7219__H
#define __MAX7219__H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C

#define CS_Pin     GPIO_Pin_4
#define SCK_Pin    GPIO_Pin_5
#define MISO_Pin   GPIO_Pin_6
#define MOSI_Pin   GPIO_Pin_7

#define CS_LOW()   GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define CS_HIGH()  GPIO_SetBits(GPIOA, GPIO_Pin_4)

#define MAX7219_REG_NO_OP         0x00
#define MAX7219_REG_DECODE_MODE   0x09
#define MAX7219_REG_INTENSITY     0x0A
#define MAX7219_REG_SCAN_LIMIT    0x0B
#define MAX7219_REG_SHUTDOWN      0x0C
#define MAX7219_REG_DISPLAY_TEST  0x0F

#define MAX7219_NUM_DEVICES 2

extern uint8_t max7219_buffer[MAX7219_NUM_DEVICES * 8];

void SPI_Config(void);
void SPI_SendData(uint16_t data);
void SPI_Send_8bit(uint8_t data);


void MAX7219_Init(void);
void MAX7219_Send(uint8_t addr, uint8_t reg, uint8_t data);
void MAX7219_ClearDisplay(uint8_t addr);
void MAX7219_SetLed(uint8_t addr, uint8_t row, uint8_t col, uint8_t state);


#endif
