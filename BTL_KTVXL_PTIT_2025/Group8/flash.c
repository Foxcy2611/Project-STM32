#include "flash.h"

void Flash_Write(uint32_t addr, uint16_t data){ // dia chi kem theo data
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

	FLASH_ErasePage(addr); // xoa trang trc khi ghi
	FLASH_ProgramHalfWord(addr, data);

	FLASH_Lock();
}

uint16_t Flash_Read(uint32_t addr){
	// ghi 16byte
	return *(volatile uint16_t*)addr;
}
