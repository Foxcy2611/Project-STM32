#ifndef FLASH_H
#define FLASH_H

#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>

#define ADD_PAGE_LAST_FLASH     0x0801FC00

bool FLASH_Write16(uint32_t addr, uint16_t data);
uint16_t FLASH_Read(uint32_t addr);


#endif