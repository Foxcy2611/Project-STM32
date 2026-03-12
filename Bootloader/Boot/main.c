#include "stm32f10x.h"
#include "uart.h"

#define FRAME_STOP      0xBB
#define FRAME_HEADER    0xAA
#define Re_Pass 				0x55AA

typedef  void (*pFunction)(void);
pFunction JumpToApplication;
uint32_t JumpAddress;

uint8_t RxBuffer[256]; 
volatile uint32_t timer_blink = 0; 

void GPIO_Config(void);
void DeInit_Peripherals(void);
void Process_Command(uint8_t cmd, uint8_t len, uint8_t* payload);
void Jump_To_App(void);

int main(void){
	SystemInit();
	GPIO_Config(); 
	UART_Init();  
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR, 
												 ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);
	
	if(BKP_ReadBackupRegister(BKP_DR1) == Re_Pass){
		BKP_WriteBackupRegister(BKP_DR1, 0x0000);
		
		// WAIT FOR UPLOAD NEW FIRMWARE
	} else {
		Jump_To_App();
	}

	
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	
	while(1){ 
		
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_RESET) {    
			for(volatile int i=0; i<50000; i++);
			
			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_RESET) {  
				
				Jump_To_App();    
			}
		}

			
		timer_blink++;
		
		if(timer_blink >= 50000){
			GPIOC->ODR ^= GPIO_Pin_13; 
			timer_blink = 0;
		}

			
		if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET){
				// START
			uint8_t byte = USART_ReceiveData(USART1);
			if(byte == FRAME_HEADER){
				// CMD
				uint8_t cmd = USART_GetChar();
				
				// LENGTH
				uint8_t len = USART_GetChar();
				// DATA = ADDRESS + PAYLOAD
				for(int i = 0 ; i < len ; i++){
					RxBuffer[i] = USART_GetChar();
				}
				// CRC
				uint8_t receivedCRC = USART_GetChar();
				// STOP
				uint8_t stopByte = USART_GetChar();
				
				if (stopByte != FRAME_STOP){
					continue;
				}

				uint8_t calCRC = 0;
				calCRC += cmd;
				calCRC += len;
				for(int i=0; i<len; i++) calCRC += RxBuffer[i];
				
				if (calCRC == receivedCRC){
				//	USART_SendChar(ACK);
					Process_Command(cmd, len, RxBuffer);
				} else {
					
					USART_SendChar(NACK);
				} 
			}
		}
	}
}

void Process_Command(uint8_t cmd, uint8_t len, uint8_t* payload)
{
    FLASH_Status status = FLASH_COMPLETE;
    
    uint32_t address = payload[0] | (payload[1] << 8) | (payload[2] << 16) | (payload[3] << 24);
    
    switch(cmd){
        case CMD_ERASE: 
        {
					for(int i = 0 ; i < 3 ; i++){
						FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
						status = FLASH_ErasePage(address); 
					
						if(status == FLASH_COMPLETE) break;
					}
						
					
					if(status == FLASH_COMPLETE){
						USART_SendChar(ACK);
					} else {
						USART_SendChar(NACK);
					}
					
					break;
        } 

        case CMD_WRITE: 
        {    
					FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
					
					uint8_t dataLen = len - 4;
					uint32_t currentAddr = address;

					if (currentAddr % 2 != 0) break;

					for(int i = 0; i < dataLen; i += 2){
					
						uint16_t data16 = payload[4 + i]; 
						if(i + 1 < dataLen) 
							data16 |= (uint16_t)(payload[4 + i + 1] << 8); 
						else 		
							data16 |= 0xFF00;
							
						status = FLASH_ProgramHalfWord(currentAddr, data16);
						
						if (status != FLASH_COMPLETE) break; 
						
						currentAddr += 2; 
					}
					
					if(status == FLASH_COMPLETE){
						USART_SendChar(ACK);
					} else {
						USART_SendChar(NACK);
					}
					
					break;
        }

        case CMD_READ:
        {
					USART_SendChar(ACK);
					
					uint16_t sizeToRead = payload[4] | (payload[5] << 8);
					
					for(int i = 0; i < sizeToRead; i++){
						uint8_t val = *(__IO uint8_t *)(address + i);
						USART_SendHex(val); 
					}
					break;
        }
        
				case CMD_JUMP:    
					USART_SendChar(ACK);
					Jump_To_App();     
					break;
    }
}

void Jump_To_App(void){
	if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000){
		FLASH_Lock();
		
		__disable_irq(); 
		
		DeInit_Peripherals(); 
		
		JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
		JumpToApplication = (pFunction) JumpAddress;
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		
		JumpToApplication();
		
    } else {
			
			UART_Init(); 
    }
}

void DeInit_Peripherals(void)
{
	UART_DeInit();
	
	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOC);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | 
												 RCC_APB2Periph_USART1, DISABLE);
	
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL  = 0;
	RCC_DeInit();
}

void GPIO_Config(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef gp;
	
	gp.GPIO_Pin = GPIO_Pin_0;
	gp.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_Init(GPIOA, &gp);

	gp.GPIO_Pin = GPIO_Pin_13;
	gp.GPIO_Mode = GPIO_Mode_Out_PP;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gp);
}
