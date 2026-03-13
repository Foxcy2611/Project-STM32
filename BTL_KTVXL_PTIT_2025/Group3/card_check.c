#include "card_check.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "flash.h"

#define RX_BUF_SIZE 50

static char RxBff[RX_BUF_SIZE];
static volatile uint8_t Idx = 0;
static volatile bool cmd_ready = false;

static uint32_t pending_username = 0;
static SystemState curr_state = STATE_IDLE;
static char CmdName[20];
static uint32_t UserID = 0;

Group3 g;


void USART_Config(void)
{
	GPIO_InitTypeDef gp;
	USART_InitTypeDef uart;
	NVIC_InitTypeDef nv;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

	// TX - PA9
	gp.GPIO_Pin = GPIO_Pin_9;
	gp.GPIO_Mode = GPIO_Mode_AF_PP;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gp);

	// RX - PA10
	gp.GPIO_Pin = GPIO_Pin_10;
	gp.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gp);

	uart.USART_BaudRate = 9600;
	uart.USART_WordLength = USART_WordLength_8b;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &uart);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	nv.NVIC_IRQChannel = USART1_IRQn;
	nv.NVIC_IRQChannelPreemptionPriority = 0;
	nv.NVIC_IRQChannelSubPriority = 0;
	nv.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nv);

	USART_Cmd(USART1, ENABLE);
}

void USART_SendChar(char c)
{
	USART_SendData(USART1, c);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void USART_SendString(char *str)
{
	while (*str)
		USART_SendChar(*str++);
}

void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		char c = USART_ReceiveData(USART1);
		if (c == '.')
		{
			RxBff[Idx] = '\0';
			Idx = 0;
			cmd_ready = true;
		}
		else if (Idx < RX_BUF_SIZE - 1)
		{
			RxBff[Idx++] = c;
		}
	}
}


uint32_t CARD_GetPendingUsername(void) {
    return pending_username;
}

void CARD_SetState(SystemState new_state)
{
    curr_state = new_state;
}

SystemState CARD_Process(void)
{
    if(!cmd_ready) return curr_state;
    cmd_ready = false;

    for(int i = 0; RxBff[i]; i++)
      if (RxBff[i] == '\r' || RxBff[i] == '\n') RxBff[i] = 0;

    char *ptr = strchr(RxBff, ':');
    if(ptr) {
      *ptr = 0;
      strcpy(CmdName, RxBff);
      UserID = atoi(ptr + 1);
    } else {
      strcpy(CmdName, RxBff);
      UserID = 0;
    }

    char msg[100];
    if(strcmp(CmdName, "REQUEST_CARD") == 0) {
      curr_state = STATE_REQUEST_CARD;
      pending_username = UserID;  
      sprintf(msg, ">> REQUEST_CARD ID=%u.\r\nPlease scan card...\r\n", UserID);
      USART_SendString(msg);
    
		} else if (strcmp(CmdName, "CARD_CHECK") == 0) {
      curr_state = STATE_CARD_CHECK;

      FlashCard_t card;
      bool found = false;

      
      for(int i = 0; i < FLASH_PAGE_SIZE / sizeof(FlashCard_t); i++) {
        Flash_Read_Card(i, &card);
        if(card.UserName == 0xFFFFFFFF) break; // flash tr?ng
            
        if(card.UserName == UserID) { 
          found = true;
          break; 
        }
      }

      if(found) {
        char msg[100];
        sprintf(msg, "==> Access Granted\r\n>> CARD_CHECK UserName=%u. \r\n>> UID bytes=[%02X %02X %02X %02X]\r\n",
                 card.UserName,
                (card.UID >> 0) & 0xFF,
                (card.UID >> 8) & 0xFF,
                (card.UID >> 16) & 0xFF,
                (card.UID >> 24) & 0xFF
				);
        
				USART_SendString(msg);
        
			} else {
        USART_SendString(">> CARD_CHECK NOT FOUND\r\n");
      }
		} else if(strcmp(CmdName, "INFO_REQUEST") == 0){
			Group_FillInfo(&g);
			Group_Print(&g);
		}	else {
			curr_state = STATE_IDLE;
			USART_SendString(">> UNKNOWN CMD\r\n");
		}
		return curr_state;
}

void Group_FillInfo(Group3 *g){
	strcpy(g->Name_Group, "Group 3");

	strcpy(g->Member[0], "Dinh Pham Cong Thanh");
	strcpy(g->Member[1], "Nguyen Xuan Tien");
	strcpy(g->Member[2], "Nguyen Van Doan");
	strcpy(g->Member[3], "Hoang Khanh Duy");

	strcpy(g->Name_Project, "Access control system !!");

	strcpy(g->Hardware_Used[0], "STM32F103C8");
	strcpy(g->Hardware_Used[1], "RFID-RC522");
	strcpy(g->Hardware_Used[2], "LCD 16x2 I2C");
	strcpy(g->Hardware_Used[3], "CP2102");

	strcpy(g->Software_Used[0], "Keil C");
	strcpy(g->Software_Used[1], "Hercules");

	strcpy(g->Date, "11/11/2025");
}


void Group_Print(const Group3 *g){
	char line[128];

	USART_SendString("\r\n==> GROUP INFORMATION :\r\n");

	sprintf(line, "- Group Name     : %s\r\n", g->Name_Group);
	USART_SendString(line);
	
	USART_SendString("- Members        :\r\n");
	for (int i = 0; i < 4; i++) {
		if (g->Member[i][0] != '\0') {
			sprintf(line, "%d. %s\r\n", i + 1, g->Member[i]);
			USART_SendString(line);
		}
	}

	sprintf(line, "- Project Name   : %s\r\n", g->Name_Project);
	USART_SendString(line);

	USART_SendString("- Hardware Used  :\r\n");
	for (int i = 0; i < 5; i++) {
		if (g->Hardware_Used[i][0] != '\0') {
			sprintf(line, "%d. %s\r\n", i + 1, g->Hardware_Used[i]);
			USART_SendString(line);
		}
	}

	USART_SendString("- Software Used  :\r\n");
	for (int i = 0; i < 3; i++) {
		if (g->Software_Used[i][0] != '\0') {
			sprintf(line, "%d. %s\r\n", i + 1, g->Software_Used[i]);
			USART_SendString(line);
		}
	}

	sprintf(line, "- Date           : %s\r\n", g->Date);
	USART_SendString(line);
}
