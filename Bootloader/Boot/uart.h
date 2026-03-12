#ifndef __UART_H
#define __UART_H

#include "stm32f10x.h"

#define APPLICATION_ADDRESS     (uint32_t)0x08004000 

#define FRAME_HEADER            0xAA
#define FRAME_STOP              0xBB 
#define MAX_DATA_LEN            128    
#define ACK                     0x06
#define NACK                    0x15

#define CMD_ERASE               0x01
#define CMD_WRITE               0x02
#define CMD_READ                0x03
#define CMD_JUMP                0x04


void UART_Init(void);
void UART_DeInit(void);

void USART_SendChar(char c);
void USART_SendString(char* str);
void USART_SendHex(uint8_t data); 

uint8_t USART_GetChar(void);
uint8_t USART_Available(void); 

uint8_t Calculate_Checksum(uint8_t *data, uint16_t len);

#endif
