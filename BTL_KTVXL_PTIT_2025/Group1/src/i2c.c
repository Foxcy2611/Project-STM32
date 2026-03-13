#include "i2c.h"

void I2C_Config(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	I2C_InitTypeDef i2c;
	GPIO_InitTypeDef gp;
	
	gp.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  gp.GPIO_Mode = GPIO_Mode_AF_OD; // danh rieng cho I2C
  gp.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &gp);

  I2C_DeInit(I2C1);
  i2c.I2C_ClockSpeed = 100000; // 100 kHz
  i2c.I2C_Mode = I2C_Mode_I2C;
  i2c.I2C_DutyCycle = I2C_DutyCycle_2;
  i2c.I2C_OwnAddress1 = 0x00;
  i2c.I2C_Ack = I2C_Ack_Enable;
  i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2C1 , &i2c);

  I2C_Cmd(I2C1, ENABLE);
}

void I2C1_Start(uint8_t addr, uint8_t dir){
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)){ // busy = ban
	}		
	
  I2C_GenerateSTART(I2C1, ENABLE);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  
	I2C_Send7bitAddress(I2C1, addr, dir);
  
	if(dir == I2C_Direction_Transmitter)
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  else
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
}

void I2C1_Write(uint8_t data){
	I2C_SendData(I2C1, data);
  while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}
	
void I2C1_Stop(void){
	I2C_GenerateSTOP(I2C1, ENABLE);
}

uint8_t I2C1_Read_Ack(void){
	I2C_AcknowledgeConfig(I2C1, ENABLE);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
  return I2C_ReceiveData(I2C1);
}

uint8_t I2C1_Read_Nack(void){
	I2C_AcknowledgeConfig(I2C1, DISABLE);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
  uint8_t data = I2C_ReceiveData(I2C1);
  I2C_AcknowledgeConfig(I2C1, ENABLE);
  return data;
}
