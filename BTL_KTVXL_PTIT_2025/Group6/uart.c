#include "uart.h"

#define UART1_RX_BUFFER_SIZE 100

static char UART1_RxBuffer[UART1_RX_BUFFER_SIZE];
static uint8_t UART1_RxIndex = 0;
static bool UART1_MessageReady = 0;

FILE __stdout;
int fputc(int chr, FILE *f){
	UART_SendChar((char)chr);
	return chr;
}

void UART_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

	// GPIO
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin = GPIO_Pin_9;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_10;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gpio);

	// Uart
	USART_InitTypeDef uart;

	uart.USART_BaudRate = 115200;
	uart.USART_WordLength = USART_WordLength_8b;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &uart);

	// B?t ng?t
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_InitTypeDef nv;

   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	// C?u hình NVIC b? qu?n lí ng?t
	nv.NVIC_IRQChannel = USART1_IRQn;
	nv.NVIC_IRQChannelPreemptionPriority = 2;
	nv.NVIC_IRQChannelSubPriority = 0;
	nv.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nv);

	USART_Cmd(USART1, ENABLE);
}

void USART1_IRQHandler(void);
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		char c = USART_ReceiveData(USART1);

		if (UART1_MessageReady == false)
		{
			if (c == '.')
			{
				UART1_RxBuffer[UART1_RxIndex] = '\0';
				UART1_MessageReady = true;
			}
			else
			{
				UART1_RxBuffer[UART1_RxIndex++] = c;
			}
		}
	}
}

void UART_SendChar(char c)
{
	USART_SendData(USART1, c);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		;
}

void UART_SendString(char *str)
{
	while (*str)
	{
		UART_SendChar(*str++);
	}
}

uint8_t UART_IsMessageReady(void)
{
	return UART1_MessageReady;
}
 // ??
char *UART_GetMessage(void)
{
	if (UART1_MessageReady)
	{
		UART1_MessageReady = false;
		UART1_RxIndex = 0; // reset index
		return UART1_RxBuffer;
	}
	return NULL;
}

void UART_Process(Command *cmd)
{
	char *ptr = strchr(UART1_RxBuffer, ':');

	if (ptr == NULL)
	{
		strcpy(cmd->cmd, UART1_RxBuffer);
		cmd->value = 0;
	}
	else
	{
		size_t cmd_len = ptr - UART1_RxBuffer;
		strncpy(cmd->cmd, UART1_RxBuffer, cmd_len);
		cmd->cmd[cmd_len] = '\0';

		cmd->value = atoi(ptr + 1);
	}
	
	UART1_MessageReady = false;
	UART1_RxIndex = 0; // reset index
}

void UART_SendInfor(void)
{
	char buffer[200];
	
	snprintf(buffer, sizeof(buffer),
		"GROUP_NAME: Embedded Team\n"
		"MEMBERS: Van An, Van Hung, Van Long\n"
		"PROJECT_NAME: Smart Light System\n"
		"HARDWARE_USED: STM32, BH1750, LCD16x2\n"
		"SOFTWARE_USED: Keil, Cmsis Library\n"
		"DATE: 2025-03-25\r\n"
	);
	
	UART_SendString(buffer);
}
