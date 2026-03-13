#include "flash.h"

void Flash_Write_Time(DS1307_TIME_t *time) {
	FLASH_Unlock(); 
	FLASH_ErasePage(FLASH_ADDR_START);
	
	FLASH_ProgramWord(FLASH_ADDR_START,     (uint32_t)time->hour);
	FLASH_ProgramWord(FLASH_ADDR_START + 4, (uint32_t)time->min);
	FLASH_ProgramWord(FLASH_ADDR_START + 8, (uint32_t)time->sec);
	
	FLASH_Lock(); 
}

void Flash_Read_Time(DS1307_TIME_t *time) {
	time->hour = (uint8_t)(*(__IO uint32_t*)(FLASH_ADDR_START));
	time->min  = (uint8_t)(*(__IO uint32_t*)(FLASH_ADDR_START + 4));
	time->sec  = (uint8_t)(*(__IO uint32_t*)(FLASH_ADDR_START + 8));
}
