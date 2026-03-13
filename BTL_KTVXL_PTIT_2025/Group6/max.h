#ifndef __MAX__H
#define __MAX__H

#include "stm32f10x.h"
#include "stm32f10x_i2c.h"     
#include "stdbool.h"

#define MAX_ADDR 0x57

#define INT_Pin GPIO_Pin_0 

#define SCL_Pin GPIO_Pin_6 
#define SDA_Pin GPIO_Pin_7 

/* Define thanh ghi */

#define REG_INTER_ENABL 0x02
#define REG_WR_PTR_CONF 0x04
#define REG_OVF_COUNTER 0x05
#define REG_RD_PTR_CONF 0x06
#define REG_FIFO_CONFIG 0x08
#define REG_MODE_CONFIG 0x09
#define REG_SP02_CONFIG 0x0A
#define REG_PULSE_AMP_1 0x0C
#define REG_PULSE_AMP_2 0x0D


#define A_FULL_EN 0x80 
#define PPG_RDY_EN 0x40


#define ERASER_POINTER 0x00

#define FIFO_SMP_AVE_4 (0b010 << 5) 
#define FIFO_ROLLOVER_ENABLE (1 << 4) 
#define FIFO_A_FULL_15 0x0F 

#define MODE_RESET 0x40 
#define MODE_SP02 0x03 
#define MODE_HR 0x02 

#define SP02_ADC_RGE_4096 (0b01 << 5)
#define SP02_SR_100 (0b001 << 2) 
#define SP02_LED_PW_411 0x03 

#define LED_CURRENT 0x24 

#define REG_FIFO_DATA 0x07


#define BPM_BUFFER_SIZE 4
#define FINGER_THRESHOLD 5000


void I2C1_WriteBit(uint8_t Addr, uint8_t regAddr, uint8_t data);
void I2C1_ReadBit(uint8_t Addr, uint8_t regAddr, uint8_t *buf, uint8_t length);
void MAX30102_Init(void);
uint32_t MAX30102_GetBPM(void);
uint32_t MAX30102_GetRawIR(void);
uint32_t MAX30102_GetRawRed(void);
bool MAX30102_HasFinger(void);

#endif
