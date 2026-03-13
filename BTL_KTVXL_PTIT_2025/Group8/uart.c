#include "uart.h"

// bo dem nhan du lieu uart
static char RxBff[100];
// chi so ghi vao bo dem
static volatile uint8_t Idx = 0;
// co bao da nhan du mot tin nhan
static bool Flag_Mess = false;

// ham khoi tao uart1
void USART_Config(void){
	// bat clock gpioa + afio + uart1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	GPIO_InitTypeDef gp;
	NVIC_InitTypeDef nv;
	USART_InitTypeDef uart;
	
	// PA9 la chan tx (push pull)
	gp.GPIO_Mode = GPIO_Mode_AF_PP;
	gp.GPIO_Pin = GPIO_Pin_9;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gp);
	
	// PA10 la chan rx (floating input)
	gp.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gp.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &gp);
	
	// cau hinh uart
	uart.USART_BaudRate = 115200;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &uart);
	
	// cho phep ngat khi nhan du lieu
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	// cau hinh nvic ngat uart1
	nv.NVIC_IRQChannel = USART1_IRQn;
	nv.NVIC_IRQChannelPreemptionPriority = 0;
	nv.NVIC_IRQChannelSubPriority = 0;
	nv.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nv);
	
	USART_Cmd(USART1, ENABLE);   // bat uart
}

// gui 1 ky tu
void USART_SendChar(char c){
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, c);
}

// gui chuoi
void USART_SendString(char *str){
	while(*str){
		USART_SendChar(*str++);
	}
}

// trinh ngat uart1
void USART1_IRQHandler(void){
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
		char c = USART_ReceiveData(USART1);

			// ky tu ket thuc la dau cham '.' ? ket thuc chuoi
		if(c == '.'){
			RxBff[Idx] = '\0';  // ket thuc chuoi
			Idx = 0;            // reset chi so
			Flag_Mess = true;   // bao da nhan xong tin
		} 
		else {
			RxBff[Idx++] = c;   // luu ky tu vao bo dem
		}
	}
}

// tach lenh dang "CMD:VALUE"
void USART_Process(Command *cmd){
	char *ptr = strchr(RxBff, ':');  // tim dau ':'

	if(ptr == NULL){
		// neu khong co ':' ? chi co lenh
		strcpy(cmd->cmd, RxBff);
		cmd->value = 0;
	}
	else {
			// tach phan lenh truoc dau ':'
		size_t cmd_len = ptr - RxBff;
		strncpy(cmd->cmd, RxBff, cmd_len);
		cmd->cmd[cmd_len] = '\0';

		// phan sau ':' la gia tri
		cmd->value = atoi(ptr + 1);
	}
	
	Flag_Mess = false; // reset co
	Idx = 0;           // reset buffer index
}

// ham gui thong tin nhom qua uart
void USART_SendInfor(void){
  char buf[300];
  snprintf(buf, sizeof(buf), 
    "GROUP_NAME: Group 8\n"
    "MEMBERS:\n1. Nguyen Duc Trung\n2.Pham Khac Hieu\n3.Pham Quy Hung\n4.Le Sy Hung\n"
    "PROJECT_NAME: Fine dust testing system\n"
    "HARDWARE_USED: STM32F103C8, GP2Y1010AU0F, LCD16x2 I2C\n"
    "SOFTWARE_USED: Keil C, Hercules\n"
    "DATE: 2025 - 11 - 17\n"
  );
  USART_SendString(buf);
}

// kiem tra xem da nhan xong tin nhan chua
bool USART_MessReady(void){
  return Flag_Mess;
}
