#include "uart.h"

void USART_Config(void){
  GPIO_InitTypeDef gpio;
  USART_InitTypeDef uart;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    // TX (PA9)
  gpio.GPIO_Pin = GPIO_Pin_9;
  gpio.GPIO_Mode = GPIO_Mode_AF_PP;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &gpio);

  // RX (PA10)
  gpio.GPIO_Pin = GPIO_Pin_10;
  gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &gpio);

  uart.USART_BaudRate = 115200;
  uart.USART_WordLength = USART_WordLength_8b;
  uart.USART_StopBits = USART_StopBits_1;
  uart.USART_Parity = USART_Parity_No;
  uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  uart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(USART1, &uart);

  USART_Cmd(USART1, ENABLE);
}

void USART_SendChar(char c){
  USART_SendData(USART1, c);
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void USART_SendString(char *str){
  while (*str){
		USART_SendChar(*str++);
	}
}

int fputc(int ch, FILE *f){
	
  USART_SendChar(ch);
  return ch;
}

void USART_SendHex(uint8_t byte) {
  char hexChars[] = "0123456789ABCDEF";
  
	USART_SendChar('0');
  USART_SendChar('x');

	USART_SendChar(hexChars[(byte >> 4) & 0x0F]); // nibble cao
  USART_SendChar(hexChars[byte & 0x0F]);        // nibble th?p
}
