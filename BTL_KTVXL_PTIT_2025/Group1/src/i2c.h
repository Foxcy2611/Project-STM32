#ifndef __I2C__H
#define __I2C__H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C

#define SCL_Pin GPIO_Pin_6
#define SDA_Pin GPIO_Pin_7

void I2C_Config(void);
void I2C1_Start(uint8_t addr, uint8_t dir);
void I2C1_Write(uint8_t data);
void I2C1_Stop(void);

uint8_t I2C1_Read_Ack(void);
uint8_t I2C1_Read_Nack(void);

#endif
