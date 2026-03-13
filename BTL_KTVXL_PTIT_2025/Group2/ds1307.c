#include "ds1307.h"

#define DS_ADDR 0x68

static void I2C_Config(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef gp;
	I2C_InitTypeDef i2c;
	
	gp.GPIO_Mode = GPIO_Mode_AF_OD;
	gp.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gp);
	
	I2C_DeInit(I2C1);
	i2c.I2C_Ack = I2C_Ack_Enable;
	i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	i2c.I2C_ClockSpeed = 100000;
	i2c.I2C_DutyCycle = I2C_DutyCycle_2;
	i2c.I2C_Mode = I2C_Mode_I2C;
	i2c.I2C_OwnAddress1 = 0x00;
	I2C_Init(I2C1, &i2c);
	
	I2C_Cmd(I2C1, ENABLE);
}

static void I2C1_WriteByte(uint16_t addr, uint16_t regAddr, uint8_t data){
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

static void I2C1_ReadByte(uint16_t addr, uint16_t regAddr, uint8_t *buff, uint16_t cnt){
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	
	I2C_GenerateSTART(I2C1, ENABLE);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1, addr << 1, I2C_Direction_Transmitter);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1, regAddr);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	//
	I2C_GenerateSTART(I2C1, ENABLE);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1, addr << 1, I2C_Direction_Receiver);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	for(int i = 0 ; i < cnt ; i++){
		if(i == cnt - 1){
			I2C_AcknowledgeConfig(I2C1, DISABLE);
			I2C_GenerateSTOP(I2C1, ENABLE);
			while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
			buff[i] = I2C_ReceiveData(I2C1);
		} else {
			I2C_AcknowledgeConfig(I2C1, ENABLE);
			while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)){
			}
      buff[i] = I2C_ReceiveData(I2C1);
		}
	}
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}
// 12 -> 0x12
static uint8_t Dec2BCD(uint8_t val) {
  return ((val / 10) << 4) | (val % 10);
}

// 0x58 -> 58
static uint8_t BCD2Dec(uint8_t val) {
  return ((val >> 4) * 10) + (val & 0x0F);
}

void DS1307_Config(void){
	I2C_Config();
	
	uint8_t seg_reg;
	I2C1_ReadByte(DS_ADDR, REG_SEC, &seg_reg, 1);
	
	if(seg_reg & 0x80){
		seg_reg &= 0x7F;
		I2C1_WriteByte(DS_ADDR, REG_SEC, seg_reg);
	}
}

void DS1307_SetTime(DS1307_TIME_t *time){
	// CH = 0 ALWAYS
	I2C1_WriteByte(DS_ADDR, REG_SEC, Dec2BCD(time->sec) & 0x7F);
	// MINUTE
	I2C1_WriteByte(DS_ADDR, REG_MIN, Dec2BCD(time->min));
	// HOUR vs bit-6 = 0 -> Mode 24h
	I2C1_WriteByte(DS_ADDR, REG_HOUR, Dec2BCD(time->hour) & 0x3F); 
    
  I2C1_WriteByte(DS_ADDR, REG_DAY,   Dec2BCD(time->day));
  I2C1_WriteByte(DS_ADDR, REG_DATE,  Dec2BCD(time->date));
  I2C1_WriteByte(DS_ADDR, REG_MONTH, Dec2BCD(time->month));
  I2C1_WriteByte(DS_ADDR, REG_YEAR,  Dec2BCD(time->year));
}

void DS1307_GetTime(DS1307_TIME_t *time) {
  uint8_t buffer[7];
    
	I2C1_ReadByte(DS_ADDR, REG_SEC, buffer, 7);
	
	time->sec   = BCD2Dec(buffer[0] & 0x7F); // 00h: Seconds
	time->min   = BCD2Dec(buffer[1]);        // 01h: Minutes
	time->hour  = BCD2Dec(buffer[2] & 0x3F); // 02h: Hours (Mode 24h)
	time->day   = BCD2Dec(buffer[3]);        // 03h: Day
	time->date  = BCD2Dec(buffer[4]);        // 04h: Date
	time->month = BCD2Dec(buffer[5]);        // 05h: Month
	time->year  = BCD2Dec(buffer[6]);        // 06h: Year
}
