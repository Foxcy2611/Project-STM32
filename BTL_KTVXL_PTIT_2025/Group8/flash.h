#ifndef __FLASH__H
#define __FLASH__H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_flash.h"            // Keil::Device:StdPeriph Drivers:Flash

#define FLASH_ADDR 0x0800FC00

void Flash_Write(uint32_t addr, uint16_t data);
uint16_t Flash_Read(uint32_t addr);


#endif
