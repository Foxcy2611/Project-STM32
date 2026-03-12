#include "sd_card.h"

uint8_t SD_Type = SD_TYPE_UNKNOW;

FATFS g_myFATFS;

static char g_sdPath[4] = "0:/";
static char g_logFile[] = "yenn.txt";

void SD_SPI_Init(void){
  GPIO_InitTypeDef gpio;
  SPI_InitTypeDef  spi;

  RCC_APB2PeriphClockCmd(SD_SPI_GPIO_CLK | SD_SPI_CLK, ENABLE);

  gpio.GPIO_Pin = SD_SPI_SCK_PIN | SD_SPI_MOSI_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SD_SPI_GPIO, &gpio);

  gpio.GPIO_Pin = SD_SPI_MISO_PIN;
  gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(SD_SPI_GPIO, &gpio);

  gpio.GPIO_Pin = SD_SPI_CS_PIN;
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SD_SPI_GPIO, &gpio);
  SD_CS_HIGH();

  spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	spi.SPI_CPHA = SPI_CPHA_1Edge;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CRCPolynomial = 7;
	spi.SPI_DataSize = SPI_DataSize_8b;
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_NSS = SPI_NSS_Soft;
  SPI_Init(SD_SPI, &spi);

  SPI_Cmd(SD_SPI, ENABLE);
}

uint8_t SD_SPI_SendByte(uint8_t data){
  while(SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(SD_SPI, data);
  while(SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_RXNE) == RESET);
  return SPI_I2S_ReceiveData(SD_SPI);
}

uint8_t SD_SPI_ReceiveByte(void){
  return SD_SPI_SendByte(0xFF); 
}

void SD_SPI_Set_Speed(void){
	SPI_Cmd(SPI1, DISABLE);
	SPI_InitTypeDef spi;
	
	SPI_I2S_DeInit(SPI1);
	
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	spi.SPI_CPHA = SPI_CPHA_1Edge;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CRCPolynomial = 7;
	spi.SPI_DataSize = SPI_DataSize_8b;
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_NSS = SPI_NSS_Soft;
  SPI_Init(SD_SPI, &spi);
	
	SPI_Cmd(SPI1, ENABLE);
}

uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc) {
  uint8_t response, i;

  SD_CS_LOW();
  SD_SPI_SendByte(0xFF);

  SD_SPI_SendByte(0x40 | cmd);
  SD_SPI_SendByte(arg >> 24);
  SD_SPI_SendByte(arg >> 16);
  SD_SPI_SendByte(arg >> 8);
  SD_SPI_SendByte(arg);
  SD_SPI_SendByte(crc);

  i = 0;
  do {
    response = SD_SPI_ReceiveByte();
    i++;
  } while ((response & 0x80) && i < 0xFF);

	return response;
}

uint8_t SD_Init(void) {
  uint16_t i;
	uint16_t response_debug;
	
  SD_CS_HIGH();
  for (i = 0; i < 10; i++) SD_SPI_SendByte(0xFF);

  i = 0;
  do {
    response_debug = SD_SendCmd(CMD0, 0, 0x95);
    SD_CS_HIGH(); 
		SD_SPI_SendByte(0xFF);
    i++;
		
  } while ((response_debug != 0x01) && (i < 200));

	if(i >= 200) return 1;
	
  response_debug = SD_SendCmd(CMD8, 0x000001AA, 0x87);
  if (response_debug == 0x01) { 
    uint8_t r7[4];
    for (i = 0; i < 4; i++) r7[i] = SD_SPI_ReceiveByte();
    SD_CS_HIGH(); SD_SPI_SendByte(0xFF);
		
		if(r7[2] != 0x01 || r7[3] != 0xAA){
			return 4;
		}
		
    i = 0;
    do {
      SD_SendCmd(CMD55, 0, 0x65);
			response_debug = SD_SendCmd(ACMD41, 0x40000000, 0x77);
			SD_CS_HIGH();
			SD_SPI_SendByte(0xFF);
			i++;
    } while ((response_debug != 0x00) && (i < 0xFFFF));
		
		if(i >= 0xFFFF) return 2;
		
     response_debug = SD_SendCmd(CMD58, 0, 0xFF);
     uint8_t ocr[4];
     for (i = 0; i < 4; i++) ocr[i] = SD_SPI_ReceiveByte();
     SD_CS_HIGH(); SD_SPI_SendByte(0xFF);

		if(response_debug == 0x00){
			if(ocr[0] & 0x40){
				SD_Type = SD_TYPE_SDHC;
			} else {
				SD_Type = SD_TYPE_SDSC;
			}
		}

  } else {
    i = 0;
    do {
      SD_SendCmd(CMD55, 0, 0x65); 
			response_debug = SD_SendCmd(ACMD41, 0x00000000, 0xFF);
      SD_CS_HIGH(); SD_SPI_SendByte(0xFF);
      i++;
    } while ((response_debug != 0x00) && (i < 0xFFFF));
		
		if (i >= 0xFFFF) return 3;
		
		SD_Type = SD_TYPE_SDSC;
	}

	if(SD_Type == SD_TYPE_SDSC){
		SD_SendCmd(CMD16, 512, 0xFF);
		SD_CS_HIGH();
		SD_SPI_ReceiveByte();
	}
	
	return 0; 
}

uint8_t SD_WriteBlock(uint8_t *buff, uint32_t addr){
	uint8_t response;
	uint16_t i;
	
	if(SD_Type == SD_TYPE_SDSC){
		addr = addr << 9;
	}
	
	response = SD_SendCmd(CMD24, addr, 0xFF);
	if(response != 0x00){
		SD_CS_HIGH();
		SD_SPI_ReceiveByte();
		return 1;
	}
	
	SD_SPI_SendByte(0xFE);
	
	for(i = 0 ; i < 512 ; i++){
		SD_SPI_SendByte(buff[i]);
	}
	SD_SPI_SendByte(0xFF);
	SD_SPI_SendByte(0xFF);
	
	response = SD_SPI_ReceiveByte();
	
	if((response & 0x1F) != 0x05){
		SD_CS_HIGH();
		SD_SPI_ReceiveByte();
		return 2;
	}
	
	while(SD_SPI_ReceiveByte() == 0x00);
	
	SD_CS_HIGH();
	SD_SPI_ReceiveByte();
	
	return 0;
}

uint8_t SD_ReadBlock(uint8_t *buff, uint32_t addr){
	uint8_t token;
	uint16_t i;
	
	if(SD_Type == SD_TYPE_SDSC){
		addr = addr << 9;
	}
	
	uint8_t response = SD_SendCmd(CMD17, addr, 0xFF);
	if(response != 0x00){
		USART_SendString("LOI: CMD17 response != 0x00\r\n");
		SD_CS_HIGH();
		SD_SPI_ReceiveByte();
		return 1;
	}
	
	i = 0xFFFF;
	do {
		token = SD_SPI_ReceiveByte();
	} while((token != 0xFE) && (--i));
	
	if(i == 0){
		SD_CS_HIGH();
		SD_SPI_ReceiveByte();
		return 2;
	}
	
	for(i = 0 ; i < 512 ; i++){
		buff[i] = SD_SPI_ReceiveByte();
	}
	SD_SPI_ReceiveByte();
	SD_SPI_ReceiveByte();
	
	return 0;
}

void SD_Mount(void){
    FRESULT res;
    char uart_buff[64];
    FIL tempFile;

    USART_SendString("Dang goi f_mount...\r\n");
    
    res = f_mount(&g_myFATFS, g_sdPath, 1);

    if (res == FR_OK) {
        USART_SendString("Mount thanh cong!\r\n");
    } else {
        sprintf(uart_buff, "f_mount loi (%d). Dung chuong trinh.\r\n", (int)res);
        USART_SendString(uart_buff);
        while(1);
    }

    USART_SendString("Dang tao/xoa file log cu...\r\n");
    res = f_open(&tempFile, g_logFile, FA_CREATE_ALWAYS | FA_WRITE);
    
    if (res != FR_OK) {
        sprintf(uart_buff, "Loi tao file (%d). Dung chuong trinh.\r\n", (int)res);
        USART_SendString(uart_buff);
        while(1);
    }
    
    f_close(&tempFile);
    USART_SendString("File log da san sang.\r\n");
}

void SD_Write_Distance(float distance_cm) {
    FIL fil;
    UINT bw;
    char line[64];
    char uart_buff[64];
    FRESULT res;

    res = f_open(&fil, g_logFile, FA_OPEN_ALWAYS | FA_WRITE | FA_OPEN_APPEND);
    
    if (res == FR_OK) {
        sprintf(line, "Khoang cach hien tai la %.2f cm\r\n", distance_cm);
        
        res = f_write(&fil, line, strlen(line), &bw);
        if (res != FR_OK) {
            sprintf(uart_buff, "Loi f_write: %d\r\n", (int)res);
            USART_SendString(uart_buff);
        }

        res = f_close(&fil);
        if (res != FR_OK) {
            sprintf(uart_buff, "Loi f_close: %d\r\n", (int)res);
            USART_SendString(uart_buff);
        }
        
        if (res == FR_OK) {
            sprintf(uart_buff, "Da ghi %.2f vao %s\r\n", distance_cm, g_logFile);
            USART_SendString(uart_buff);
        }

    } else {
        sprintf(uart_buff, "Loi f_open (khi ghi): %d\r\n", (int)res);
        USART_SendString(uart_buff);
    }
}