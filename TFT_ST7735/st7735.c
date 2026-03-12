#include "st7735.h"

void SPI_Config(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	GPIO_InitTypeDef gpio;
	SPI_InitTypeDef spi;
	
	gpio.GPIO_Pin = TFT_SCK | TFT_SDA;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TFT_PORT, &gpio);
	
	gpio.GPIO_Pin = TFT_A0 | TFT_CS | TFT_RST;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TFT_PORT, &gpio);
	
	CS_HIGH();
	RST_HIGH();
	
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	spi.SPI_CPHA = SPI_CPHA_1Edge;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CRCPolynomial = 7;
	spi.SPI_DataSize = SPI_DataSize_8b;
	spi.SPI_Direction = SPI_Direction_1Line_Tx;
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(TFT_SPI, &spi);
	
	SPI_Cmd(TFT_SPI, ENABLE);
}

void SPI_SendByte(uint8_t byte){
	while(SPI_I2S_GetFlagStatus(TFT_SPI, SPI_I2S_FLAG_TXE) == RESET);
	
	SPI_I2S_SendData(TFT_SPI, byte);
	
	while(SPI_I2S_GetFlagStatus(TFT_SPI, SPI_I2S_FLAG_BSY) == RESET);
}

void SPI_WriteCmd(uint8_t cmd){
	A0_CMD();
	CS_LOW();
	
	SPI_SendByte(cmd);
	
	CS_HIGH();
}

void SPI_WriteData(uint8_t data){
	A0_DATA();
	CS_LOW();
	
	SPI_SendByte(data);
	
	CS_HIGH();
}
void SPI_Reset(void){
	RST_HIGH();
	Delay_ms(5);
	
	RST_LOW();
	Delay_ms(20);
	
	RST_HIGH();
	Delay_ms(150);
}

void ST7735_Init(void){
	SPI_Config();
	
	SPI_Reset();
	
	
	SPI_WriteCmd(REG_SWRESET);
	Delay_ms(150);
	
	SPI_WriteCmd(REG_SLPOUT);
	Delay_ms(200);
	SPI_WriteCmd(REG_COLMOD);
	SPI_WriteData(0x05);
	
	SPI_WriteCmd(REG_MADCTL);
	SPI_WriteData(0xA0);
	
	SPI_WriteCmd(REG_GMCTRN1);
	SPI_WriteData(0x09); // VRF0P
	SPI_WriteData(0x16); // VOS0P
	SPI_WriteData(0x09); SPI_WriteData(0x20); // PK0P - 1P
	SPI_WriteData(0x21); SPI_WriteData(0x1B); // 2 - 3
	SPI_WriteData(0x13); SPI_WriteData(0x19); // 4 - 5
  SPI_WriteData(0x17); SPI_WriteData(0x15); // 6 - 7 
	SPI_WriteData(0x1E); SPI_WriteData(0x2B); // 8 - 9
  SPI_WriteData(0x04); SPI_WriteData(0x05); // SELV0- -1P
	SPI_WriteData(0x02); SPI_WriteData(0x0E); // SELV62P - 63P
	
	SPI_WriteCmd(REG_GMCTRN1);
  SPI_WriteData(0x0B); 
	SPI_WriteData(0x14); 
	SPI_WriteData(0x08); SPI_WriteData(0x1E);
  SPI_WriteData(0x22); SPI_WriteData(0x1D); 
	SPI_WriteData(0x18); SPI_WriteData(0x1E);
  SPI_WriteData(0x1B); SPI_WriteData(0x1A); 
	SPI_WriteData(0x24); SPI_WriteData(0x2B);
  SPI_WriteData(0x06); SPI_WriteData(0x06); 
	SPI_WriteData(0x02); SPI_WriteData(0x0F);
	
	SPI_WriteCmd(REG_DISPON);
	Delay_ms(100);
	
	ST7735_FillScreen(RGB_BLACK);
}

void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1){
	SPI_WriteCmd(REG_CASET);
	SPI_WriteData(0x00); SPI_WriteData(x0);
	SPI_WriteData(0x00); SPI_WriteData(x1);
	
	SPI_WriteCmd(REG_RASET);
	SPI_WriteData(0x00); SPI_WriteData(y0);
	SPI_WriteData(0x00); SPI_WriteData(y1);
	
	SPI_WriteCmd(REG_RAMWR);
}

void ST7735_FillScreen(uint16_t color){
	ST7735_SetAddressWindow(0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1);

	A0_DATA();
	CS_LOW();
	
	for(uint16_t i = 0 ; i < ST7735_WIDTH ; i++){
		for(uint16_t j = 0 ; j < ST7735_HEIGHT ; j++){
			SPI_SendByte(color >> 8);
			SPI_SendByte(color & 0xFF);
		}
	}
	
	CS_HIGH();
}

void ST7735_DrawPixel(uint8_t x, uint8_t y, uint16_t color){
	if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;

  ST7735_SetAddressWindow(x, y, x + 1, y + 1);
    
  A0_DATA();
  CS_LOW();

	SPI_SendByte(color >> 8);
	SPI_SendByte(color & 0xFF);

	CS_HIGH();
}

void ST7735_DrawImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint16_t* data){
	if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;
    
	ST7735_SetAddressWindow(x, y, x + w - 1, y + h - 1);
      
	A0_DATA();    
	CS_LOW();
	
	for(int i = 0; i < w * h; i++) {  
		SPI_SendByte(data[i] >> 8);
		SPI_SendByte(data[i] & 0xFF);
	}
		
	CS_HIGH();
}

void ST7735_DrawImageStruct(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t* data) {   
	if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;   
  
  ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);
     
  A0_DATA(); 
	CS_LOW();
    
	for(uint32_t i = 0; i < w * h * 2; i += 2) {  
		SPI_SendByte(data[i + 1]);
		SPI_SendByte(data[i]);
	}
       
	CS_HIGH();
}
