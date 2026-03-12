#include "stm32f10x.h"
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "misc.h"                       // Keil::Device:StdPeriph Drivers:Framework

void Delay(uint32_t time);
void LED_Config(void);


int main(void)
{
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000); // --> 0x8000 4000

	SystemInit();

	LED_Config();

	while (1)
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);       
		Delay(500); 
			
		GPIO_SetBits(GPIOC, GPIO_Pin_13); 
		Delay(500);
	}
}

void Delay(uint32_t time)
{
  for(volatile int i = 0 ; i < time ; i++)
	{
		for(volatile int j = 0 ; j < 0x2aff ; j++);
	}
}

void LED_Config(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef gp;
	gp.GPIO_Pin = GPIO_Pin_13;
	gp.GPIO_Mode = GPIO_Mode_Out_PP;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gp);
}
