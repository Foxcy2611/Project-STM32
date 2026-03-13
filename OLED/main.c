#include "stm32f10x.h"                  // Device header
#include "oled.h"
#include "i2c.h"


int main(){
	OLED_Init();
	OLED_Clear();
	OLED_GotoXY(1, 0);
	OLED_Puts("Hello anyone !!");
	OLED_GotoXY(1, 8);
	OLED_Puts("Nguyen Ngoc Chien");
	OLED_UpdateScreen();
	while(1);
}
