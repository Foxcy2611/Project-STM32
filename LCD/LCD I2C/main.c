#include "stm32f10x.h"                  // Device header
#include "stm32f10x_adc.h"              // Keil::Device:StdPeriph Drivers:ADC
#include "stm32f10x_dma.h"              // Keil::Device:StdPeriph Drivers:DMA
#include "stm32f10x_exti.h"             // Keil::Device:StdPeriph Drivers:EXTI
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART

#include "delay.h"
#include "i2c.h"
#include "i2c.h"
#include "lcd_i2c.h"

#include "stm32f10x.h"
#include "delay.h"
#include "i2c.h"
#include "lcd_i2c.h"


int main(void){
    // Khoi tao Timer
    TIM2_Init();

    // Khoi tao I2C1 (PB6=SCL, PB7=SDA)
    I2C_Config();

    // Khoi tao LCD 2 dong
    LCD_Init(2);

    // Hien thi noi dung
    LCD_SetCursor(0, 0);
    LCD_SendString("HELLO STM32");

    LCD_SetCursor(0, 1);
	LCD_SendString("PTIT mai dinh");

    while(1){

    }
}
