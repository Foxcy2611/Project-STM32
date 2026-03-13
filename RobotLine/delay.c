
#include "delay.h"

static volatile uint32_t ms;

void delay_Init(void)
{
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);
}

void SysTick_Handler(void)
{
	ms++;
}

__inline uint32_t millis(void)
{
	return ms;
}

void delay_ms(uint32_t time_delay)
{
	uint32_t cur_time = millis();
	while(millis() - cur_time < time_delay);
}
