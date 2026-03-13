#include "stm32f10x.h"                  // Device header
#include "lcd_i2c.h"
#include "delay.h"
#include "card_check.h"
#include "my_rc522.h"
#include "stdio.h"
#include "flash.h"

SystemState state;

int main(void) {
  SystemInit();
	
  Delay_Init();  
	
  RC522_Init();
  Flash_Erase_LastPage();
	
  USART_Config();
  LCD_Init();

	
  static uint32_t last_card_scan_time = 0;
  const uint32_t CARD_SCAN_INTERVAL = 600; 

  SystemState prev_state = (SystemState)-1;

  while(1) {
    state = CARD_Process();
    uint32_t current_time = millis();

    if(state != prev_state) {
      switch(state){
        case STATE_IDLE:
          
					LCD_Clear();
          LCD_GotoXY(0, 0);
          LCD_Puts("Quet the...");
          break;
        case STATE_REQUEST_CARD:
          
				  LCD_Clear();
          LCD_GotoXY(0, 0);
          LCD_Puts("Dki the...");
          break;
        case STATE_CARD_CHECK:
          
				  break;
      }
      
      prev_state = state; 
		}

    switch(state) {
      case STATE_REQUEST_CARD: {
        if(current_time - last_card_scan_time >= CARD_SCAN_INTERVAL) {
          last_card_scan_time = current_time;

          if(RC522_IsCardPresent()) {
            uint8_t* UID = RC522_GetUID();
            char buf[100];
            sprintf(buf, "UID: %02X %02X %02X %02X\r\n", UID[0], UID[1], UID[2], UID[3]);
            USART_SendString(buf);

            uint32_t UID_uint = UID[0] | (UID[1] << 8) | (UID[2] << 16) | (UID[3] << 24);
            uint32_t username = CARD_GetPendingUsername();
            Flash_Save_Card(UID_uint, username);

            LCD_Clear(); 
            LCD_Puts("Dki the OK !!!");
            delay_ms(1000);                         
                 
						CARD_SetState(STATE_IDLE);
          }     
				}      
			} 
			break;

      case STATE_CARD_CHECK: {        
				CARD_SetState(STATE_IDLE);     
			}
      break;
           
			case STATE_IDLE: {              
				if(current_time - last_card_scan_time >= CARD_SCAN_INTERVAL) {                    
					last_card_scan_time = current_time;
                   
					if(RC522_IsCardPresent()) {                        
						uint8_t* UID = RC522_GetUID();                        
						char buf[100];                      
						sprintf(buf, "UID: %02X %02X %02X %02X\r\n", UID[0], UID[1], UID[2], UID[3]);                    
						USART_SendString(buf);

            uint32_t UID_uint = UID[0] | (UID[1] << 8) | (UID[2] << 16) | (UID[3] << 24);
            if(Flash_Check_UID(UID_uint) >= 0) {
                
							LCD_Clear();                    
							LCD_GotoXY(0, 0);                 
							LCD_Puts("Access Granted");              
							delay_ms(1000); 
                       
						} else {                  
							LCD_Clear();                       
							LCD_GotoXY(0, 0);                   
							LCD_Puts("Access Denied");                          
							delay_ms(1000);                   
						}
                                                
						prev_state = (SystemState)-1;                
					}             
				}          
			} 
			break;

			default:               
				break;      
		}
  }
}
