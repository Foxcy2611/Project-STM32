#include "flash.h"

bool FLASH_Write16(uint32_t addr, uint16_t data)
{
	FLASH_Unlock();
	FLASH_ErasePage(addr);
	
	if (FLASH_ProgramHalfWord(addr, data) != FLASH_COMPLETE)
	{
		FLASH_Lock();
		return false;
	}
	FLASH_Lock();
	return true;
}

uint16_t FLASH_Read(uint32_t addr)
{
	return *(volatile uint16_t*)addr;
}
