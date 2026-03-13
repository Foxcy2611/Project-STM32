#include "max7219.h"

uint8_t max7219_buffer[MAX7219_NUM_DEVICES * 8];


void SPI_Config(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
	GPIO_InitTypeDef gp;
	SPI_InitTypeDef spi;
	
	// SCK: AF_PP. MOSI: AF_PP
	gp.GPIO_Mode = GPIO_Mode_AF_PP;
	gp.GPIO_Pin = SCK_Pin | MOSI_Pin;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gp);
	
	// MISO: IN_FLOATING
	gp.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gp.GPIO_Pin = MISO_Pin;
	GPIO_Init(GPIOA, &gp);
	
	// CS: OUT_PP
	gp.GPIO_Mode = GPIO_Mode_Out_PP;
	gp.GPIO_Pin = CS_Pin;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gp);
	
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	spi.SPI_CPHA = SPI_CPHA_1Edge;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CRCPolynomial = 7;
	spi.SPI_DataSize = SPI_DataSize_8b;
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_NSS = SPI_NSS_Soft;
	
	SPI_Init(SPI1, &spi);
	
	SPI_Cmd(SPI1, ENABLE);
}

void SPI_SendData(uint16_t data){
	CS_LOW();

	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

	SPI_I2S_SendData(SPI1, data);
	
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	
	CS_HIGH();
}

void SPI_Send_8bit(uint8_t data){
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

	SPI_I2S_SendData(SPI1, data);
	
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
}


void MAX7219_Send(uint8_t addr, uint8_t reg, uint8_t data) {
  if(addr >= MAX7219_NUM_DEVICES) {
    return;
  }

  CS_LOW();

  for(int i = MAX7219_NUM_DEVICES - 1; i >= 0; i--) {
    if(i == addr) {
      SPI_Send_8bit(reg);
      SPI_Send_8bit(data);
     } else {
       SPI_Send_8bit(MAX7219_REG_NO_OP);
       SPI_Send_8bit(0x00);
     }
  }

  CS_HIGH();
}

void MAX7219_Init(void) {
  SPI_Config(); 

  for(int i = 0; i < MAX7219_NUM_DEVICES; i++) {
    MAX7219_Send(i, MAX7219_REG_DISPLAY_TEST, 0x00);
    MAX7219_Send(i, MAX7219_REG_SCAN_LIMIT, 0x07); 
    MAX7219_Send(i, MAX7219_REG_DECODE_MODE, 0x00); 
		MAX7219_Send(i, MAX7219_REG_INTENSITY, 0x08);
		MAX7219_Send(i, MAX7219_REG_SHUTDOWN, 0x01);
    MAX7219_ClearDisplay(i);                 
   }
}



void MAX7219_ClearDisplay(uint8_t addr) {
  if(addr >= MAX7219_NUM_DEVICES) {
    return;
  }
    
  uint16_t offset = addr * 8;
  for(int i = 0; i < 8; i++) {
     max7219_buffer[offset + i] = 0x00;
     MAX7219_Send(addr, i + 1, 0x00); 
  }
}

void MAX7219_SetLed(uint8_t addr, uint8_t row, uint8_t col, uint8_t state) {
  if(addr >= MAX7219_NUM_DEVICES || row > 7 || col > 7) {
    return;
  }

  uint16_t offset = addr * 8; 
  uint8_t bit_mask = 0x80 >> col; 

  if(state) {
    max7219_buffer[offset + row] |= bit_mask;
  } else {
    max7219_buffer[offset + row] &= ~bit_mask;
  }

  MAX7219_Send(addr, row + 1, max7219_buffer[offset + row]);
}




