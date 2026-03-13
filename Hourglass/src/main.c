#include "stm32f10x.h"

#include "max7219.h"
#include "mpu6050.h"

#include "delay.h"
#include "uart.h"
#include "control_matrix.h"

int main(){
	SystemInit();
	Delay_Init();
	USART_Config();
	
	MPU6050_Init();
	MAX7219_Init();
	TIM2_NVIC();

	Fill_Sand(0);
	LC_ClearDisplay(1);
	
/*
	LC_SetXY(0, 7, 7, true);
	LC_SetXY(0, 7, 0, true);
		
	LC_SetXY(1, 7, 7, true);
	LC_SetXY(1, 7, 0, true);
*/
	
	while(1){
		if(Gravity == 1){ // xuoi doc
			Update_Sand_Vertical(0);
			Update_Sand_Vertical(1);
			Delay_ms(100);
			Start_Sand_Vertical();
			
		} else if(Gravity == -1){ // xuoi nguoc
			Update_Matrix_Horizontal(0);
			Update_Matrix_Horizontal(1);
			Delay_ms(100);
			Start_Sand_Horizontal();
	
		} else if(Gravity == 2){ // trai
			Update_Matrix_Left(0);
			Update_Matrix_Left(1);
		
		} else if(Gravity == -2){ // phai
			Update_Matrix_Right(0);
			Update_Matrix_Right(1);
		}
	}
	
}
