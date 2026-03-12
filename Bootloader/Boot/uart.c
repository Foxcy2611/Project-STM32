#include "uart.h"

void UART_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef gp;
	USART_InitTypeDef uart;

	// TX - PA9
	gp.GPIO_Pin = GPIO_Pin_9;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	gp.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gp);

	// RX - PA10
	gp.GPIO_Pin = GPIO_Pin_10;
	gp.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gp);

	uart.USART_BaudRate = 115200;
	uart.USART_WordLength = USART_WordLength_8b;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &uart);
	USART_Cmd(USART1, ENABLE);
}

void UART_DeInit(void){
	USART_Cmd(USART1, DISABLE);
	USART_DeInit(USART1);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
}

void USART_SendChar(char c){
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, c);
}

void USART_SendString(char* str){
	while(*str){
		USART_SendChar(*str++);
	}
}

uint8_t USART_GetChar(void){
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return (uint8_t)USART_ReceiveData(USART1);
}

uint8_t USART_Available(void){
	return (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET);
}

uint8_t Calculate_Checksum(uint8_t *data, uint16_t len){
	uint8_t sum = 0;
	for(uint16_t i = 0; i < len; i++){
		sum += data[i];
	}
	return sum;
}

void USART_SendHex(uint8_t data){
	char hexTable[] = "0123456789ABCDEF";
	USART_SendChar(hexTable[(data >> 4) & 0x0F]); 
	USART_SendChar(hexTable[data & 0x0F]);    
}
