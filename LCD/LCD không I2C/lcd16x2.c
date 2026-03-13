#include "lcd16x2.h"
#include "delay.h"

#define RS_PIN     GPIO_Pin_0
#define EN_PIN     GPIO_Pin_1
#define D4_PIN     GPIO_Pin_2
#define D5_PIN     GPIO_Pin_3
#define D6_PIN     GPIO_Pin_4
#define D7_PIN     GPIO_Pin_5
#define LCD_PORT   GPIOA

void LCD_Enable(void) {
    GPIO_SetBits(LCD_PORT, EN_PIN);
    delay_ms(1);
    GPIO_ResetBits(LCD_PORT, EN_PIN);
    delay_ms(1);
}

void LCD_Send4Bits(uint8_t data) {
    GPIO_WriteBit(LCD_PORT, D4_PIN, (data >> 0) & 1);
    GPIO_WriteBit(LCD_PORT, D5_PIN, (data >> 1) & 1);
    GPIO_WriteBit(LCD_PORT, D6_PIN, (data >> 2) & 1);
    GPIO_WriteBit(LCD_PORT, D7_PIN, (data >> 3) & 1);
	// data >> x : dich data n lan sang phai
  // vdu (data >> 2) & 1 va data = 0b10110110, dich 2 lan
	// data = 0b00 101101 & 00000001 = 0b00000001 = 1
	// ==> 1
}

void LCD_SendCommand(uint8_t cmd) {
    GPIO_ResetBits(LCD_PORT, RS_PIN);
    LCD_Send4Bits(cmd >> 4);
    LCD_Enable();
    LCD_Send4Bits(cmd & 0x0F);
    LCD_Enable();
    delay_ms(2);
}

void LCD_SendData(uint8_t data) {
    GPIO_SetBits(LCD_PORT, RS_PIN);
    LCD_Send4Bits(data >> 4);
    LCD_Enable();
    LCD_Send4Bits(data & 0x0F);
    LCD_Enable();
    delay_ms(2);
}

void LCD_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = RS_PIN | EN_PIN | D4_PIN | D5_PIN | D6_PIN | D7_PIN;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LCD_PORT, &gpio);

    delay_ms(20);
    LCD_Send4Bits(0x03); LCD_Enable(); delay_ms(5);
    LCD_Send4Bits(0x03); LCD_Enable(); delay_ms(5);
    LCD_Send4Bits(0x03); LCD_Enable(); delay_ms(5);
    LCD_Send4Bits(0x02); LCD_Enable(); delay_ms(5);

    LCD_SendCommand(0x28);
    LCD_SendCommand(0x0C);
    LCD_SendCommand(0x06);
    LCD_SendCommand(0x01);
    delay_ms(2);
}

void LCD_Clear(void) {
    LCD_SendCommand(0x01);
    delay_ms(2);
}

void LCD_GotoXY(uint8_t x, uint8_t y) {
    uint8_t addr = (y == 0) ? 0x80 + x : 0xC0 + x;
    LCD_SendCommand(addr);
}

void LCD_Puts(char *str) {
    while (*str) {
        LCD_SendData(*str++);
    }
}
