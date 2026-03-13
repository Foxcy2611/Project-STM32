#include "i2c.h"

/*
quy trinh giao tiep i2c

https://khuenguyencreator.com/giao-thuc-i2c-lap-trinh-stm32-voi-rtc-ds3231/
*/
void I2C_Config(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE); // bat clock I2C1
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // BAT CLOCK GPIOB

  GPIO_InitTypeDef gp;
  I2C_InitTypeDef i2c; // dat ten bth

  gp.GPIO_Pin = I2C_SCL_Pin | I2C_SDA_Pin; // cac nay thi toi define o file i2c.h 
  gp.GPIO_Mode = GPIO_Mode_AF_OD; // mode af_od
  gp.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(I2C_GPIO, &gp);

  I2C_DeInit(I2C_Port); // reset i2c1 ve mac dinh
  i2c.I2C_ClockSpeed = 100000; // tan so clock la 100khz
  i2c.I2C_Mode = I2C_Mode_I2C; // mode i2c chuan
  i2c.I2C_DutyCycle = I2C_DutyCycle_2; // ti le xung la 50%
  i2c.I2C_OwnAddress1 = 0x00; // nay ko qtr, them cho du (0x00 = 0)
  i2c.I2C_Ack = I2C_Ack_Enable; // bat tính nang ack
	// stm32 dóng vai tro la master, lcd la slave
	// master ra lenh cho slave
	// master se gui lenh cho slave, neu slave nhan dc lenh thi slave gui nguoc lai cho master 1 bit ten la ACK
  i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2C_Port, &i2c);

  I2C_Cmd(I2C_Port, ENABLE);
}

void I2C_Start(uint8_t address, uint8_t direction){
  I2C_GenerateSTART(I2C_Port, ENABLE);
	// Gui bit start, bat dau giao tiep
  while(!I2C_CheckEvent(I2C_Port, I2C_EVENT_MASTER_MODE_SELECT));
// cho bit sb = 1, hay la cho khi STM32 nhan I2C la che do MASTER
	
  I2C_Send7bitAddress(I2C_Port, address, direction);
// Gui 7 bit dia chi la address, direction la bit doc du lieu
	// I2C_Direction_Transmitter = 0x00: ghi du lieu
	
  if(direction == I2C_Direction_Transmitter) // cho event (su kien), 
    while(!I2C_CheckEvent(I2C_Port, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); // gui du lieu
  else
    while(!I2C_CheckEvent(I2C_Port, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));// nhan du lieu
}

void I2C_Stop(void){
  I2C_GenerateSTOP(I2C_Port, ENABLE);
}

void I2C_Write(uint8_t data){ // ham gui lenh
  I2C_SendData(I2C_Port, data); // gui lenh, gui xong thi phai doi no gui xog
  while(!I2C_CheckEvent(I2C_Port, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); ///
}


	/*
	master se gui dia chi slave + bit R/W
	slave nhan dia chi kia -> gui ack cho master
	slave gui 1 byte du lieu
	master se gui nguoc lai slave ack hoac nack
	+ ack: muon doc tiep
	+ nack: ko muon doc -> het du lieu
	*/

uint8_t I2C_ReadAck(void){
  I2C_AcknowledgeConfig(I2C_Port, ENABLE); // master gui ack
  while(!I2C_CheckEvent(I2C_Port, I2C_EVENT_MASTER_BYTE_RECEIVED)); // cho
  return I2C_ReceiveData(I2C_Port); // lay byte vua nhan 
}

uint8_t I2C_ReadNack(void){
  I2C_AcknowledgeConfig(I2C_Port, DISABLE);
  while(!I2C_CheckEvent(I2C_Port, I2C_EVENT_MASTER_BYTE_RECEIVED));
  uint8_t data = I2C_ReceiveData(I2C_Port);
  I2C_GenerateSTOP(I2C_Port, ENABLE);
  return data;
}
