#include "buzzer.h"


void Buzzer_Config(void){
	GPIO_InitTypeDef gp;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	gp.GPIO_Mode = GPIO_Mode_Out_PP;
	gp.GPIO_Pin = GPIO_Pin_3;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gp);

	GPIO_ResetBits(GPIOA, GPIO_Pin_3);
}

void Buzzer_On(void){
	GPIO_SetBits(GPIOA, GPIO_Pin_3);
}

void Buzzer_Off(void){
	GPIO_ResetBits(GPIOA, GPIO_Pin_3);
}
