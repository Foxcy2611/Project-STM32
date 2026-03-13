#ifndef __MPU6050__H
#define __MPU6050__H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C

#include "uart.h"

#define SCL_Pin GPIO_Pin_6
#define SDA_Pin GPIO_Pin_7

#define MPU_ADDR 0x68

#define REG_PWR_MGMT_1 0x6B
#define REG_WHO_AM_I 0x75
#define REG_GYRO_CONFIG 0x1B
#define REG_ACCEL_CONFIG 0x1C
#define REG_SMPLRT_DIV 0x19
#define REG_CONFIG 0x1A

#define REG_ACCEL_XOUT_H 0x3B
#define REG_ACCEL_XOUT_L 0x3C
#define REG_ACCEL_YOUT_H 0x3D
#define REG_ACCEL_YOUT_L 0x3E
#define REG_ACCEL_ZOUT_H 0x3F
#define REG_ACCEL_ZOUT_L 0x40

#define REG_GYRO_XOUT_H 0x43
#define REG_GYRO_XOUT_L 0x44
#define REG_GYRO_YOUT_H 0x45
#define REG_GYRO_YOUT_L 0x46
#define REG_GYRO_ZOUT_H 0x47
#define REG_GYRO_ZOUT_L 0x48

void I2C1_Config(void);
void I2C1_WriteBit(uint8_t addr, uint8_t regAddr, uint8_t data);
void I2C1_ReadBit(uint8_t addr, uint8_t regAddr, uint8_t *buff, uint8_t len);


void MPU6050_Init(void);
void MPU6050_ReadAccel(int16_t *Ax, int16_t *Ay, int16_t *Az);
void MPU6050_ReadGyro(int16_t *Gx, int16_t *Gy, int16_t *Gz);
int16_t MPU6050_ReturnAx(void);
int16_t MPU6050_ReturnAy(void);

#endif
