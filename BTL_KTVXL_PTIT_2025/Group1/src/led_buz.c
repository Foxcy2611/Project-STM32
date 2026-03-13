#include "led_buz.h"

volatile uint16_t LED_Number = 0;

const uint8_t SEG_CODE[10] = {
  0x3F, // 0
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F  // 9
};

void SEG_BUZ_Config(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitTypeDef gpio;
  TIM_TimeBaseInitTypeDef tim;
	NVIC_InitTypeDef nvic;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;

	gpio.GPIO_Pin = SEG_A_PIN | SEG_B_PIN | SEG_C_PIN | 
									SEG_D_PIN | SEG_E_PIN | SEG_F_PIN | SEG_G_PIN;
	GPIO_Init(SEG_PORT, &gpio);
	
	gpio.GPIO_Pin = DIGIT1_PIN | DIGIT2_PIN | 
									DIGIT3_PIN | DIGIT4_PIN ;
	GPIO_Init(DIGIT_PORT, &gpio);
	
	
	GPIO_SetBits(DIGIT_PORT, DIGIT1_PIN | DIGIT2_PIN | 
													 DIGIT3_PIN | DIGIT4_PIN);
	
	GPIO_ResetBits(SEG_PORT, SEG_A_PIN | SEG_B_PIN | SEG_C_PIN |
                           SEG_D_PIN | SEG_E_PIN | SEG_F_PIN | SEG_G_PIN);
	
	
	tim.TIM_ClockDivision = TIM_CKD_DIV1;
	tim.TIM_CounterMode = TIM_CounterMode_Up;
	tim.TIM_Period = 200 - 1;
	tim.TIM_Prescaler = 720 - 1;
	tim.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &tim);
	TIM_Cmd(TIM3, ENABLE);
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); 
	
	nvic.NVIC_IRQChannel = TIM3_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 2;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
}

void TIM3_IRQHandler(void){
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		static uint8_t scan_pos = 0; 
		
		GPIO_SetBits(DIGIT_PORT, DIGIT1_PIN | DIGIT2_PIN | 
													   DIGIT3_PIN | DIGIT4_PIN);
		uint8_t digit_val = 0;
		if		  (scan_pos == 0) digit_val = LED_Number / 1000;
		else if (scan_pos == 1) digit_val = (LED_Number / 100) % 10;
		else if (scan_pos == 2) digit_val = (LED_Number / 10) % 10;
		else 										digit_val = LED_Number % 10; // c
		
		uint16_t curr_port = GPIO_ReadOutputData(GPIOA);
		GPIO_Write(GPIOA, (curr_port & 0xFF80) | SEG_CODE[digit_val]);

		if(scan_pos == 0) 		 GPIO_ResetBits(DIGIT_PORT, DIGIT1_PIN);
		else if(scan_pos == 1) GPIO_ResetBits(DIGIT_PORT, DIGIT2_PIN);
		else if(scan_pos == 2) GPIO_ResetBits(DIGIT_PORT, DIGIT3_PIN);
		else 									 GPIO_ResetBits(DIGIT_PORT, DIGIT4_PIN);
		
		scan_pos++;
		if(scan_pos > 3){
			scan_pos = 0;
		}
	}
}
