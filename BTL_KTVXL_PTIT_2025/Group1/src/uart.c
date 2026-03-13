#include "uart.h"

#define UART_BUFFER_SIZE 64

static char RxBff[UART_BUFFER_SIZE];
static volatile uint8_t Idx = 0;
static volatile uint8_t Flag_Done = 0;

static bool Decode_Internal(char *str, Command_t *cmd);

void Command_Init(void){	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);
	GPIO_InitTypeDef gp;
	NVIC_InitTypeDef NV;
	
	gp.GPIO_Mode = GPIO_Mode_AF_PP;
	gp.GPIO_Pin = GPIO_Pin_9;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gp);
	
	gp.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gp.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &gp);

	USART_InitTypeDef uart;
	uart.USART_BaudRate = 115200;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &uart);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

	NV.NVIC_IRQChannel = USART1_IRQn;
	NV.NVIC_IRQChannelPreemptionPriority = 0;
	NV.NVIC_IRQChannelSubPriority = 0;
	NV.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NV);
	
	USART_Cmd(USART1, ENABLE);
}


void USART_SendChar(char c){
	USART_SendData(USART1, c);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void USART_SendString(char *str){
	while(*str){
		USART_SendData(USART1, *str++);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
}


void USART1_IRQHandler(void){
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
    char c = (char)USART_ReceiveData(USART1); 
        
    if(Idx < UART_BUFFER_SIZE - 1){
      RxBff[Idx++] = c;
    }
  }

  if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET){
    volatile uint16_t temp;
    temp = USART_GetFlagStatus(USART1, USART_FLAG_IDLE);
    temp = USART_ReceiveData(USART1); 
        
    (void)temp;
        
    if(Idx > 0){
      RxBff[Idx] = '\0';
      Flag_Done = 1;    
      Idx = 0;         
    }
  }
	
}

static bool Decode_Internal(char *str, Command_t *cmd){
  if(str == NULL || cmd == NULL) return false;

  memset(cmd, 0, sizeof(Command_t));
  char *ptr = strchr(str, ':');
  if(ptr == NULL){
		strcpy(cmd->Name, str);
		cmd->Value = 0;
		return true;
	}

  size_t len = ptr - str;
  if(len >= sizeof(cmd->Name)) len = sizeof(cmd->Name) - 1;

  strncpy(cmd->Name, str, len);
  cmd->Name[len] = '\0';
  cmd->Value = (uint16_t)atoi(ptr + 1);

  return true;
}

bool UART_MessIsReady(void){
	return Flag_Done;
}

bool Command_Decode(Command_t *cmd){
  Flag_Done = 0;
  return Decode_Internal(RxBff, cmd);
}

void Group_FillInfo(Group1 *g){
	strcpy(g->Name_Group, "Group 1");

	strcpy(g->Member[0], "Nguyen Ngoc Chien");
	strcpy(g->Member[1], "Pham Huy Hung");
	strcpy(g->Member[2], "Nguyen Ngoc Huy");
	strcpy(g->Member[3], "Pham Hoang Long");

	strcpy(g->Name_Project, "Light Intensity Measuring System");

	strcpy(g->Hardware_Used[0], "STM32");
	strcpy(g->Hardware_Used[1], "BH1750");
	strcpy(g->Hardware_Used[2], "OLED 128x64");
	strcpy(g->Hardware_Used[3], "5461-AS");
	strcpy(g->Hardware_Used[4], "LED");

	strcpy(g->Software_Used[0], "Keil C");
	strcpy(g->Software_Used[1], "Hercules");
	strcpy(g->Software_Used[2], "Proteus");

	strcpy(g->Date, "24/10/2025");
}


void Group_Print(const Group1 *g){
	char buffer[200];

	USART_SendString("\r\n===== GROUP INFORMATION =====\r\n");

	sprintf(buffer, "- Group Name     : %s\r\n", g->Name_Group);
	USART_SendString(buffer);

	USART_SendString("- Members        :\r\n");
	for (int i = 0; i < 4; i++) {
		if (g->Member[i][0] != '\0') {
			sprintf(buffer, "  %d. %s\r\n", i + 1, g->Member[i]);
			USART_SendString(buffer);
		}
	}

	sprintf(buffer, "- Project Name   : %s\r\n", g->Name_Project);
	USART_SendString(buffer);

	USART_SendString("- Hardware Used  :\r\n");
	for (int i = 0; i < 5; i++) {
		if (g->Hardware_Used[i][0] != '\0') {
			sprintf(buffer, "  %d. %s\r\n", i + 1, g->Hardware_Used[i]);
			USART_SendString(buffer);
		}
	}

	USART_SendString("- Software Used  :\r\n");
	for (int i = 0; i < 3; i++) {
		if (g->Software_Used[i][0] != '\0') {
			sprintf(buffer, "  %d. %s\r\n", i + 1, g->Software_Used[i]);
			USART_SendString(buffer);
		}
	}

	sprintf(buffer, "- Date           : %s\r\n", g->Date);
	USART_SendString(buffer);

	USART_SendString("==============================\r\n");
}

void USART_SendCheck(uint16_t as, uint16_t value){
	char a[40];
	sprintf(a, "LIGHT_INTENSITY: %d\r\n", as);
	USART_SendString(a);
	if(as > value){
		sprintf(a, "WARNING LIGHT !!!");
		OLED_GotoXY(0, 22);
		OLED_Puts(a);
		
		USART_SendString(a);
		USART_SendString("\r\n");
	}
}

void USART_SendDone(uint8_t Mode){
	char m[20];

	if(Mode == 1){
		sprintf(m, "\r\nOK SET THERESHOLD\r\n");
		USART_SendString(m);
	} else if(Mode == 2){
		sprintf(m, "\r\nOK SET TIME CHECK\r\n");
		USART_SendString(m);
	} else if(Mode == 3){
		USART_SendString("\r\nERROR SET\r\n");
	}
}

void USART_SendParameter(void){
	uint16_t threshold, check;
	Flash_Read(&threshold, &check);
	
	char m[100];
	sprintf(m, "\r\nThresold: %d - Check: %d\r\n", threshold, check);
	USART_SendString(m);
}
