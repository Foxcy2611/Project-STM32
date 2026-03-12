#include "stm32f10x.h"
#include "uart.h"
#include "sd_card.h"
#include "ff.h"


int main(){
    USART_Config();
    SD_SPI_Init();
    
	SD_Mount();
	USART_SendString("=== MOUNT ===\r\n");
	
	SD_Write_Distance(12.12);
	SD_Write_Distance(26.11);
	SD_Write_Distance(10.02);
    
    while(1);
}
