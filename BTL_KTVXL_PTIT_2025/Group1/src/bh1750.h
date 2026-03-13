#ifndef __BH1750__H
#define __BH1750__H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C
#include "i2c.h"
#include "Fl_Tim.h"

#define BH1750_Addr 0x5C

#define Power_On 0x01
#define Power_Down 0x00
#define BH_Reset 0x07
#define Con_H_Mode 0x10

void BH1750_Config(void);
uint16_t BH1750_ReadLight(void);

#endif
