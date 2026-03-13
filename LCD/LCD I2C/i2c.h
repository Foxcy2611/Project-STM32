#ifndef __I2C_H
#define __I2C_H
#include "stm32f10x.h"

#define I2C_Port I2C1
#define I2C_GPIO GPIOB
#define I2C_SCL_Pin GPIO_Pin_6
#define I2C_SDA_Pin GPIO_Pin_7

void I2C_Config(void);
void I2C_Start(uint8_t address, uint8_t direction);
void I2C_Stop(void);
void I2C_Write(uint8_t data);
uint8_t I2C_ReadAck(void);
uint8_t I2C_ReadNack(void);

#endif
