#ifndef __SD__CARD__H
#define __SD__CARD__H

#include "stm32f10x.h"
#include "stm32f10x_spi.h"              // Keil::Device:StdPeriph Drivers:SPI

#include <string.h>
#include <stdint.h>
#include "uart.h"
#include "ff.h"

#define SD_SPI                   SPI1
#define SD_SPI_CLK               RCC_APB2Periph_SPI1

#define SD_SPI_SCK_PIN           GPIO_Pin_5
#define SD_SPI_MISO_PIN          GPIO_Pin_6
#define SD_SPI_MOSI_PIN          GPIO_Pin_7
#define SD_SPI_CS_PIN            GPIO_Pin_4
#define SD_SPI_GPIO              GPIOA
#define SD_SPI_GPIO_CLK          RCC_APB2Periph_GPIOA

#define SD_CS_LOW()   GPIO_ResetBits(SD_SPI_GPIO, SD_SPI_CS_PIN)
#define SD_CS_HIGH()  GPIO_SetBits(SD_SPI_GPIO, SD_SPI_CS_PIN)

#define CMD0 0
#define CMD1 1
#define CMD8 8
#define CMD9 9
#define CMD16 16
#define CMD17 17
#define CMD24 24
#define CMD55 55
#define CMD58 58

#define ACMD41 41

#define SD_TYPE_UNKNOW 0
#define SD_TYPE_SDSC 1
#define SD_TYPE_SDHC 2


void SD_SPI_Init(void);
uint8_t SD_SPI_ReceiveByte(void);
uint8_t SD_SPI_SendByte(uint8_t data);
void SD_SPI_Set_Speed(void);

extern uint8_t SD_Type;

// ko dc xoa
uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc);
uint8_t SD_Init(void); 
uint8_t SD_WriteBlock(uint8_t *buff, uint32_t addr);
uint8_t SD_ReadBlock(uint8_t *buff, uint32_t addr);
//

void SD_Mount(void); // mount ngay dau main
void SD_Write_Distance(float distance_cm); // ghi

#endif
