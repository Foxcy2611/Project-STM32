#include "Lcd_i2c.h"
#include "hc_sr05.h"
#include "hc_sr05_interval.h"
#include "sd_card.h"
#include "uart.h"
#include "ff.h"
#include "buzzer.h"


/*
 * Ham main: Chuong trinh chinh dieu khien he thong
 * Chuc nang:
 *  - Khoi tao LCD I2C hien thi thong tin
 *  - Khoi tao cam bien do khoang cach HC-SR05
 *  - Su dung timer de do dinh ky moi 500ms
 *  - Ghi du lieu vao the SD
 *  - Gui thong tin qua UART de debug
 *  - Hien thi khoang cach len LCD
 */
int main()
{
	// Khoi tao man hinh LCD 20 cot 4 dong
	LCD_Begin(20, 4);
	// Dat con tro hien thi tai cot 18 dong 0, in don vi "cm"
	LCD_SetCursor(18, 0);
	LCD_Print("cm");
	
	HCSR05_Init();
	
	// Khoi tao timer do khoang cach moi 500ms
	HCSR05_Interval_Init(500);
	
	USART_Config();
	
	Buzzer_Init();
	SD_SPI_Init();
	SD_InitFileSystem("pop.txt");
	
	while (1)
	{
		/*
		 * Kiem tra co du lieu cap nhat tu bo timer dinh ky (500ms)
		 * Neu co thi phat lenh trigger den cam bien HC-SR05 de do khoang cach moi
		 */
		if (HCSR05_Interval_GetUpdateFlag())
		{
			HCSR05_Trigger();									// Gui xung kich den cam bien
			HCSR05_Interval_SetFlag(false);		// Xoa co cap nhat de cho lan sau
		}
		
		/*
		 * Kiem tra neu cam bien da do xong va co ket qua
		 */
		if (HCSR05_IsReady())
		{
			float distance  = HCSR05_GetDistance();
			
			SD_Write_Distance(distance);
			
			LCD_SetCursor(0, 0);
			LCD_Printf("Distance:  %.2lf ", (double)distance);
//			char bff[100];
//			sprintf(bff, "DIS = %.2lf", (double)distance);
//			USART_SendString(bff);
			
			if(distance <= 10){
				Buzzer_On();
			} else {
				Buzzer_Off();
			}
		}
	}
//	USART_Config();
//	USART_SendString("OK\r\n");
}
