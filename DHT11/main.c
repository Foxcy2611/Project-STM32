#include "stm32f10x.h"
#include "stdio.h"
#include "stm32f10x_gpio.h"             // Thu vien GPIO
#include "stm32f10x_rcc.h"              // Thu vien RCC
#include "stm32f10x_tim.h"              // Thu vien Timer
#include "stm32f10x_usart.h"            // Thu vien USART

/*
1. MCU gui Start Signal:
Keo chan DATA xuong LOW trong hon 18ms
Sau do keo len HIGH trong 20us - 40us

2. DHT11 phan hoi:
Keo DATA LOW 80us
Sau do HIGH 80us

3. DHT11 gui 40 bit data: 40 bit = 5 byte
16 bit do am (phan nguyen + phan thap phan)
16 bit nhiet do (phan nguyen + phan thap phan)
8 bit checksum
*/

// Bien toan cuc
uint8_t DHT11_Data[5];

// === Tao bo Timer2 de lam delay_us va delay_ms ===
void TIM2_Init(void) {
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseInitTypeDef tim;
  tim.TIM_ClockDivision = TIM_CKD_DIV1;
  tim.TIM_CounterMode = TIM_CounterMode_Up;
  tim.TIM_Period = 0xFFFF;
  tim.TIM_Prescaler = 72 - 1; // prescaler de timer dem moi 1us
  TIM_TimeBaseInit(TIM2, &tim);

  TIM_Cmd(TIM2, ENABLE);
}

void delay_us(uint16_t us) {
  TIM_SetCounter(TIM2, 0);
  while (TIM_GetCounter(TIM2) < us);
}

void delay_ms(uint16_t ms) {
  while (ms--) delay_us(1000);
}

// === Cau hinh UART1 (PA9 la chan TX) ===
void UART1_Init(void) {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

  GPIO_InitTypeDef gpio;
  gpio.GPIO_Pin = GPIO_Pin_9;
  gpio.GPIO_Mode = GPIO_Mode_AF_PP;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &gpio);

  USART_InitTypeDef usart;
  usart.USART_BaudRate = 9600;
  usart.USART_WordLength = USART_WordLength_8b;
  usart.USART_StopBits = USART_StopBits_1;
  usart.USART_Parity = USART_Parity_No;
  usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  usart.USART_Mode = USART_Mode_Tx;
  USART_Init(USART1, &usart);

  USART_Cmd(USART1, ENABLE);
}

// Gui 1 ky tu qua UART
void UART1_SendChar(char c) {
  USART_SendData(USART1, c);
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

// Gui chuoi ky tu qua UART
void UART1_SendString(char *str) {
  while (*str) UART1_SendChar(*str++);
}

// === Cau hinh chan PA1 giao tiep DHT11 ===
void DHT11_SetOutput(void) {
  GPIO_InitTypeDef gpio; 
  gpio.GPIO_Pin = GPIO_Pin_1;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_Out_OD; // output open-drain de tranh xung dot muc dien ap
  GPIO_Init(GPIOA, &gpio);
}

void DHT11_SetInput(void) {
  GPIO_InitTypeDef gpio;
  gpio.GPIO_Pin = GPIO_Pin_1;
  gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING; // che do nhan du lieu
  GPIO_Init(GPIOA, &gpio);
}

// Doc trang thai chan DATA
uint8_t DHT11_ReadPin(void) {
  return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
}

// === Ham giao tiep va doc du lieu tu DHT11 ===
uint8_t DHT11_ReadData(uint8_t *buf) {
  // Su dung con tro buf de luu dia chi mang du lieu
	uint8_t i, j;

	// Buoc 1: MCU gui tin hieu Start
  DHT11_SetOutput();  
  GPIO_ResetBits(GPIOA, GPIO_Pin_1); // keo chan xuong LOW
  delay_ms(20); // toi thieu 18ms
  GPIO_SetBits(GPIOA, GPIO_Pin_1);   // keo len HIGH
  delay_us(30); // thoi gian 20us - 40us
  DHT11_SetInput(); // chuyen sang che do nhan

  // Cho DHT11 phan hoi
  delay_us(40);

  // Buoc 2: DHT11 se keo LOW 80us roi HIGH 80us
	if (DHT11_ReadPin() == 1) return 1; // khong co phan hoi

	delay_us(80); 
  if (DHT11_ReadPin() == 0) return 2; // sai dinh dang phan hoi

  delay_us(80); // cho truoc khi gui du lieu

  // Doc 5 byte du lieu (40 bit)
  for (j = 0; j < 5; j++) {
    uint8_t byte = 0;

    for (i = 0; i < 8; i++) {

        while (DHT11_ReadPin() == 0); // doi chan len HIGH

        delay_us(30);

        if (DHT11_ReadPin() == 1){
					byte |= (1 << (7 - i)); // ghi bit 1 vao byte
				}

				while (DHT11_ReadPin()); // doi chan xuong LOW
      }

      buf[j] = byte;
   }

/*
 |= la phep OR de gan bit
 1 << n la dich bit sang trai n lan

 Vi du:
 1 << 3 = 0b00001000

 bit dau tien: 1 << 7 = 10000000
 bit cuoi:     1 << 0 = 00000001
*/

    // Kiem tra checksum
   uint8_t sum = buf[0] + buf[1] + buf[2] + buf[3];

   if (sum != buf[4]) return 3; // loi checksum

   return 0; // doc thanh cong
}

// === Chuong trinh chinh ===
int main(void) {

    SystemInit();
    TIM2_Init();
    UART1_Init();

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 

    char msg[64];

    while (1) {

        uint8_t err = DHT11_ReadData(DHT11_Data);

        if (err == 0) {

            sprintf(msg, "Nhiet do: %d*C - Do am: %d%%\r\n", DHT11_Data[2], DHT11_Data[0]);

            UART1_SendString(msg);

        } else {

            UART1_SendString("Loi doc DHT11\r\n");

        }

        delay_ms(2000);
    }
}
