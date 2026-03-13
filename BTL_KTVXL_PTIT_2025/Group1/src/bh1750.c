#include "bh1750.h"

void BH1750_Config(void){
	I2C1_Start(BH1750_Addr << 1, I2C_Direction_Transmitter);
	I2C1_Write(Power_On);
	I2C1_Stop();
	Delay_ms(10);
	
	I2C1_Start(BH1750_Addr << 1, I2C_Direction_Transmitter);
	I2C1_Write(BH_Reset);
	I2C1_Stop();
	Delay_ms(10);
	
	I2C1_Start(BH1750_Addr << 1, I2C_Direction_Transmitter);
  I2C1_Write(Con_H_Mode);
  I2C1_Stop();
	
	Delay_ms(180);
}

uint16_t BH1750_ReadLight(void){
	uint8_t lsb, msb;
	uint16_t raw;
	
	I2C1_Start(BH1750_Addr << 1, I2C_Direction_Receiver);
	msb = I2C1_Read_Ack();
	lsb = I2C1_Read_Nack();
	I2C1_Stop();
	
	raw = (msb << 8 | lsb);
	return (uint16_t)(raw / 1.2);
}
