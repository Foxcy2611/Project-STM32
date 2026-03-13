#include "mpu6050.h"

void I2C1_Config(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	I2C_InitTypeDef i2c;
	GPIO_InitTypeDef gp;
	
	gp.GPIO_Pin = SCL_Pin | SDA_Pin;
  gp.GPIO_Mode = GPIO_Mode_AF_OD;
  gp.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &gp);

  I2C_DeInit(I2C1);
  i2c.I2C_ClockSpeed = 100000; // 100 000
  i2c.I2C_Mode = I2C_Mode_I2C;
  i2c.I2C_DutyCycle = I2C_DutyCycle_2;
  i2c.I2C_OwnAddress1 = 0x00;
  i2c.I2C_Ack = I2C_Ack_Enable;
  i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2C1 , &i2c);

  I2C_Cmd(I2C1, ENABLE);
}

void I2C1_WriteBit(uint8_t addr, uint8_t regAddr, uint8_t data){
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	
	I2C_GenerateSTART(I2C1, ENABLE);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1, addr << 1, I2C_Direction_Transmitter);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1, regAddr);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2C1, data);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTOP(I2C1, ENABLE);
}

void I2C1_ReadBit(uint8_t addr, uint8_t regAddr, uint8_t *buff, uint8_t len){
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	
	I2C_GenerateSTART(I2C1, ENABLE);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1, addr << 1, I2C_Direction_Transmitter);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1, regAddr);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		///
	I2C_GenerateSTART(I2C1, ENABLE);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1, addr << 1, I2C_Direction_Receiver);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	while(len > 0){
		if(len == 1){
			I2C_AcknowledgeConfig(I2C1, DISABLE);
			I2C_GenerateSTOP(I2C1, ENABLE);
		}
		
		while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
		
		*buff++ = I2C_ReceiveData(I2C1);
		len--;
	}
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}


void MPU6050_Init(void){
	uint8_t check = 0;

	I2C1_Config();
		
	I2C1_ReadBit(MPU_ADDR, REG_WHO_AM_I, &check, 1);
	if(check == 0x68){
		I2C1_WriteBit(MPU_ADDR, REG_PWR_MGMT_1, 0x00);
		I2C1_WriteBit(MPU_ADDR, REG_SMPLRT_DIV, 0x07);
		I2C1_WriteBit(MPU_ADDR, REG_CONFIG, 0x00);
		I2C1_WriteBit(MPU_ADDR, REG_GYRO_CONFIG, 0x00);
		I2C1_WriteBit(MPU_ADDR, REG_ACCEL_CONFIG, 0x00);
	} else {
		USART_SendString("INIT ERROR\r\n");
	}
}

void MPU6050_ReadAccel(int16_t *Ax, int16_t *Ay, int16_t *Az){
	uint8_t buf[6];
	I2C1_ReadBit(MPU_ADDR, REG_ACCEL_XOUT_H, buf, 6);
	
	*Ax = (int16_t)(buf[0] << 8 | buf[1]);
  *Ay = (int16_t)(buf[2] << 8 | buf[3]);
  *Az = (int16_t)(buf[4] << 8 | buf[5]);
}

void MPU6050_ReadGyro(int16_t *Gx, int16_t *Gy, int16_t *Gz){
  uint8_t buf[6];
  I2C1_ReadBit(MPU_ADDR, REG_GYRO_XOUT_H, buf, 6);

  *Gx = (int16_t)(buf[0] << 8 | buf[1]);
  *Gy = (int16_t)(buf[2] << 8 | buf[3]);
  *Gz = (int16_t)(buf[4] << 8 | buf[5]);
}

int16_t MPU6050_ReturnAx(void){
	uint8_t buf[6];
	I2C1_ReadBit(MPU_ADDR, REG_ACCEL_XOUT_H, buf, 6);
	return (int16_t)(buf[0] << 8 | buf[1]);
}

int16_t MPU6050_ReturnAy(void){
	uint8_t buf[6];
	I2C1_ReadBit(MPU_ADDR, REG_ACCEL_YOUT_H, buf, 2); //
	return (int16_t)(buf[0] << 8 | buf[1]);
}
