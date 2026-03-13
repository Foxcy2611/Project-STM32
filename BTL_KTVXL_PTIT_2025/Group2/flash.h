#ifndef __FLASH__H
#define __FLASH__H

#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include "ds1307.h" 

#define FLASH_ADDR_START  0x0800FC00 

void Flash_Write_Time(DS1307_TIME_t *time);
void Flash_Read_Time(DS1307_TIME_t *time);

#endif
