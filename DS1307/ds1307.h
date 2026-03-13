#ifndef __DS1307_H
#define __DS1307_H

#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C
#include "stm32f10x.h"                  // Device header

typedef struct {
	uint8_t sec; // 0 - 59
	uint8_t min; // 0 - 59
	uint8_t hour; // 0 - 23
	
	uint8_t day; // 1 - 7
	uint8_t date; // 1 - 31
	uint8_t month; // 1 - 12
	uint8_t year; // 2000 - 2099 <-> 0 - 99
	
} DS1307_TIME_t;

#define REG_SEC 0x00
#define REG_MIN 0x01
#define REG_HOUR 0x02

#define REG_DAY 0x03
#define REG_DATE 0x04
#define REG_MONTH 0x05
#define REG_YEAR 0x06
#define REG_CONTROL 0x07


void DS1307_Config(void);
void DS1307_SetTime(DS1307_TIME_t *time);
void DS1307_GetTime(DS1307_TIME_t *time);

#endif
