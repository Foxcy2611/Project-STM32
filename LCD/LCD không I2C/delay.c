#include "stm32f10x.h"
#include "delay.h"

void delay_init(void) {
    SysTick_Config(SystemCoreClock / 1000); // 1ms interrupt
}

volatile uint32_t msTicks = 0;

void SysTick_Handler(void) {
    msTicks++;
}

void delay_ms(uint32_t ms) {
    uint32_t now = msTicks;
    while ((msTicks - now) < ms);
}
