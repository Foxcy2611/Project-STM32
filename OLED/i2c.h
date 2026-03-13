#ifndef __I2C__H
#define __I2C__H

#include "stm32f10x.h"
#include "stm32f10x_i2c.h"

void I2C_Config(void);
void I2C1_Start(uint8_t address, uint8_t direction);
void I2C1_Write(uint8_t data);
uint8_t I2C1_Read_Ack(void);
uint8_t I2C1_Read_Nack(void);
void I2C1_Stop(void);

#endif
