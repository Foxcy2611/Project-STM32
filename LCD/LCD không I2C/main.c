#include "stm32f10x.h"
#include "delay.h"
#include "lcd16x2.h"

int main(void) {
    delay_init();   
    LCD_Init();     

    LCD_GotoXY(0, 0);
    LCD_Puts("Hello STM32");
    LCD_GotoXY(0, 1);
    LCD_Puts("NGOC CHIEN !!");

    while (1);
}
