#include "uart.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#define UART_BUFF_SIZE 64

volatile uint8_t rx_buffer[UART_BUFF_SIZE];
volatile uint8_t rx_index = 0;
volatile uint8_t rx_flag = 0;

DS1307_TIME_t tempTime;

uint8_t UART_Check(void){
	return rx_index;
}

char* UART_Check_And_Parse(void) {
    

    rx_flag = 0;
    char *result = "ERROR";

    
    // 'S' -> SET_TIME
	if (rx_buffer[0] == 'S') {
		int h, m, s;
		if (sscanf((char*)rx_buffer, "SET_TIME:%d:%d:%d", &h, &m, &s) == 3) {
				tempTime.hour = h;
				tempTime.min = m;
				tempTime.sec = s;
				tempTime.day = 7; tempTime.date = 22; tempTime.month = 11; tempTime.year = 25;
				
				result = "SET_TIME"; 
		}
	}
    //  'I' ->  INFO_REQUEST
	else if (rx_buffer[0] == 'I') {
		if (strncmp((char*)rx_buffer, "INFO_REQUEST", 12) == 0) {
				result = "INFO_REQUEST";
		}
	}

	rx_index = 0;
	memset((void*)rx_buffer, 0, UART_BUFF_SIZE);

	return result;
}

void UART1_Config(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef gpio;
	// TX
	gpio.GPIO_Pin = GPIO_Pin_9; gpio.GPIO_Mode = GPIO_Mode_AF_PP; gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
	// RX
	gpio.GPIO_Pin = GPIO_Pin_10; gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gpio);

	USART_InitTypeDef usart;
	usart.USART_BaudRate = 9600;
	usart.USART_WordLength = USART_WordLength_8b;
	usart.USART_StopBits = USART_StopBits_1;
	usart.USART_Parity = USART_Parity_No;
	usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &usart);

	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = USART1_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
}

void UART_SendChar(char c) {
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, c);
}

void UART_SendString(char *str) {
	while (*str) UART_SendChar(*str++);
}

void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_RXNE)) {
		char c = USART_ReceiveData(USART1);
		if (rx_flag == 0) { // Ch? nh?n khi buffer tr?ng
			if (c == '\n' || c == '\r') {
					rx_buffer[rx_index] = 0;
					if (rx_index > 0) rx_flag = 1; // B?t c?
			} else {
					if (rx_index < UART_BUFF_SIZE - 1) rx_buffer[rx_index++] = c;
			}
		}
	}
}
