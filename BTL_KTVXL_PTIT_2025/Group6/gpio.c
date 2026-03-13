#include "Gpio.h"

void Gpio_Config(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t GPIO_Mode, uint8_t GPIO_Speed)
{
    GPIO_InitTypeDef gpio_init;
    if(GPIOx == GPIOA)
    {
        if( GPIO_Pin == GPIO_Pin_15){
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
            GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
            gpio_init.GPIO_Mode = GPIO_Mode;
            gpio_init.GPIO_Pin = GPIO_Pin;
            gpio_init.GPIO_Speed = GPIO_Speed;
            GPIO_Init(GPIOx, &gpio_init);
        }
        else{
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
            gpio_init.GPIO_Mode = GPIO_Mode;
            gpio_init.GPIO_Pin = GPIO_Pin;
            gpio_init.GPIO_Speed = GPIO_Speed;
            GPIO_Init(GPIOx, &gpio_init);
        }
    }
    else if(GPIOx == GPIOB)
    {
        if( GPIO_Pin == GPIO_Pin_3 || GPIO_Pin == GPIO_Pin_4){
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
            GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
            gpio_init.GPIO_Mode = GPIO_Mode;
            gpio_init.GPIO_Pin = GPIO_Pin;
            gpio_init.GPIO_Speed = GPIO_Speed;
            GPIO_Init(GPIOx, &gpio_init);
        }
        else{
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
            gpio_init.GPIO_Mode = GPIO_Mode;
            gpio_init.GPIO_Pin = GPIO_Pin;
            gpio_init.GPIO_Speed = GPIO_Speed;
            GPIO_Init(GPIOx, &gpio_init);
        }
    }
    else if(GPIOx == GPIOC)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        gpio_init.GPIO_Mode = GPIO_Mode;
        gpio_init.GPIO_Pin = GPIO_Pin;
        gpio_init.GPIO_Speed = GPIO_Speed;
        GPIO_Init(GPIOx, &gpio_init);
    }
}
