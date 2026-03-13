#ifndef __LED__BUZ__H
#define __LED__BUZ__H

#include "stm32f10x.h"                  // Device header

#define SEG_PORT        GPIOA
#define SEG_A_PIN       GPIO_Pin_0
#define SEG_B_PIN       GPIO_Pin_1
#define SEG_C_PIN       GPIO_Pin_2
#define SEG_D_PIN       GPIO_Pin_3
#define SEG_E_PIN       GPIO_Pin_4
#define SEG_F_PIN       GPIO_Pin_5
#define SEG_G_PIN       GPIO_Pin_6

#define DIGIT_PORT      GPIOB
#define DIGIT1_PIN      GPIO_Pin_12
#define DIGIT2_PIN      GPIO_Pin_13
#define DIGIT3_PIN      GPIO_Pin_14
#define DIGIT4_PIN      GPIO_Pin_15

extern volatile uint16_t LED_Number;

void SEG_BUZ_Config(void);


#endif
