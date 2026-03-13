#include "led_7.h"
#include "stm32f10x.h"

#define SEG_PORT         GPIOA
#define RELAY_PORT       GPIOC
#define DIGIT_PORT       GPIOB

#define RELAY_PIN       GPIO_Pin_13

#define SEG_A_PIN       GPIO_Pin_0
#define SEG_B_PIN       GPIO_Pin_1
#define SEG_C_PIN       GPIO_Pin_2
#define SEG_D_PIN       GPIO_Pin_3
#define SEG_E_PIN       GPIO_Pin_4
#define SEG_F_PIN       GPIO_Pin_5
#define SEG_G_PIN       GPIO_Pin_6
#define SEG_DP_PIN      GPIO_Pin_7

#define DIGIT1_PIN      GPIO_Pin_12
#define DIGIT2_PIN      GPIO_Pin_13
#define DIGIT3_PIN      GPIO_Pin_14
#define DIGIT4_PIN      GPIO_Pin_15



static const uint8_t SEG_CODE[10] = {
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

static uint8_t digits[4] = {0,0,0,0};
static uint8_t current_digit = 0;
static uint8_t dot_digit = 0;

static void SevenSeg_Output(uint8_t code, uint8_t dot) {
  GPIO_WriteBit(SEG_PORT, SEG_A_PIN, (code & 0x01) ? Bit_SET : Bit_RESET);
  GPIO_WriteBit(SEG_PORT, SEG_B_PIN, (code & 0x02) ? Bit_SET : Bit_RESET);
  GPIO_WriteBit(SEG_PORT, SEG_C_PIN, (code & 0x04) ? Bit_SET : Bit_RESET);
  GPIO_WriteBit(SEG_PORT, SEG_D_PIN, (code & 0x08) ? Bit_SET : Bit_RESET);
  GPIO_WriteBit(SEG_PORT, SEG_E_PIN, (code & 0x10) ? Bit_SET : Bit_RESET);
  GPIO_WriteBit(SEG_PORT, SEG_F_PIN, (code & 0x20) ? Bit_SET : Bit_RESET);
  GPIO_WriteBit(SEG_PORT, SEG_G_PIN, (code & 0x40) ? Bit_SET : Bit_RESET);
	GPIO_WriteBit(SEG_PORT, SEG_DP_PIN, dot ? Bit_SET : Bit_RESET);
}

void SevenSeg_Init(void) {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  GPIO_InitTypeDef gp;
  gp.GPIO_Mode = GPIO_Mode_Out_PP;
  gp.GPIO_Speed = GPIO_Speed_50MHz;

  gp.GPIO_Pin = SEG_A_PIN | SEG_B_PIN | SEG_C_PIN | SEG_D_PIN |
                SEG_E_PIN | SEG_F_PIN | SEG_G_PIN | SEG_DP_PIN;
  GPIO_Init(SEG_PORT, &gp);

	gp.GPIO_Pin = RELAY_PIN; // 0 on, 1 off
	GPIO_Init(RELAY_PORT, &gp);
	
  gp.GPIO_Pin = DIGIT1_PIN | DIGIT2_PIN | DIGIT3_PIN | DIGIT4_PIN;
  GPIO_Init(DIGIT_PORT, &gp);

  GPIO_SetBits(DIGIT_PORT, DIGIT1_PIN | DIGIT2_PIN | DIGIT3_PIN | DIGIT4_PIN);
  
	GPIO_SetBits(RELAY_PORT, RELAY_PIN);
	
  GPIO_ResetBits(SEG_PORT, SEG_A_PIN | SEG_B_PIN | SEG_C_PIN |
													 SEG_D_PIN | SEG_E_PIN | SEG_F_PIN | SEG_G_PIN | SEG_DP_PIN);

  TIM_TimeBaseInitTypeDef TM;
  TM.TIM_ClockDivision = TIM_CKD_DIV1;
  TM.TIM_CounterMode = TIM_CounterMode_Up;
  TM.TIM_Period = 2000 - 1; // 2ms
  TM.TIM_Prescaler = 72 - 1;
  TIM_TimeBaseInit(TIM3, &TM);
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

  ////////////////////////////////////
  NVIC_InitTypeDef NV;
  NV.NVIC_IRQChannel = TIM3_IRQn;
  NV.NVIC_IRQChannelPreemptionPriority = 1;
  NV.NVIC_IRQChannelSubPriority = 0;
  NV.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NV);

  TIM_Cmd(TIM3, ENABLE);
}

void SevenSeg_SetNumber(float num) {
	if(num > 9999.9f) num = 9999.9f;
	
	uint16_t integer = (uint16_t)num;
	uint16_t fraction = (uint16_t)((num - integer) * 10);
	
	digits[0] = (integer / 100) % 10;
	digits[1] = (integer / 10) % 10;
	digits[2] = (integer / 1) % 10;
	digits[3] = fraction;
}

void SevenSeg_Update(void) {
  GPIO_SetBits(DIGIT_PORT, DIGIT1_PIN | DIGIT2_PIN | DIGIT3_PIN | DIGIT4_PIN);

  uint8_t dot = 0;
	if(current_digit == 2) dot = 1;
	SevenSeg_Output(SEG_CODE[digits[current_digit]], dot);
	
	switch(current_digit)
	{
		case 0: GPIO_ResetBits(DIGIT_PORT, DIGIT1_PIN); break;
		case 1: GPIO_ResetBits(DIGIT_PORT, DIGIT2_PIN); break;
		case 2: GPIO_ResetBits(DIGIT_PORT, DIGIT3_PIN); break;
		case 3: GPIO_ResetBits(DIGIT_PORT, DIGIT4_PIN); break;
	}
	
	current_digit++;
	if(current_digit >= 4) current_digit = 0;
}

void TIM3_IRQHandler(void) {
  if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    SevenSeg_Update();
  }
}

void ON_OFF_Relay(float water_level, float water_level_max){
	if(water_level < water_level_max){
		GPIO_ResetBits(RELAY_PORT, RELAY_PIN);
	}
	else
	{
		GPIO_SetBits(RELAY_PORT, RELAY_PIN);
	}
}
