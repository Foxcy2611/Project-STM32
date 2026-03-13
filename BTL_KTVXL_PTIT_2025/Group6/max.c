#include "max.h"

static volatile uint8_t Flag_Data_Ready = 0;
static volatile uint32_t max_tick = 0;
static uint32_t beatAvg;
static long delta;
static long lastBeat = 0;
static float dcFilterIR = 0;
static uint32_t ir_sample = 0;
static uint32_t _red_sample = 0;

static void MAX_Time_Init(void) {

    if (SysTick_Config(SystemCoreClock / 1000)) { 
        while (1);
    }

    NVIC_SetPriority(SysTick_IRQn, 0); // xet uu tien muc 0, group 2
}

static uint32_t MAX_Millis(void) {
    return max_tick;
}

static void MAX_Delay(uint32_t ms) {
    uint32_t start = max_tick;
    while ((max_tick - start) < ms);
}

void SysTick_Handler(void) {
    max_tick++;
}

static void I2C_Config(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 

  GPIO_InitTypeDef gp;
  NVIC_InitTypeDef nv;
	EXTI_InitTypeDef ex;
	I2C_InitTypeDef i2c; 
	
  gp.GPIO_Pin = SCL_Pin | SDA_Pin; 
  gp.GPIO_Mode = GPIO_Mode_AF_OD; 
  gp.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &gp);

	gp.GPIO_Pin = INT_Pin;
	gp.GPIO_Mode = GPIO_Mode_IPU;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gp);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	
	ex.EXTI_Line = EXTI_Line0;
	ex.EXTI_Mode = EXTI_Mode_Interrupt;
	ex.EXTI_Trigger = EXTI_Trigger_Falling;
	ex.EXTI_LineCmd = ENABLE;
	EXTI_Init(&ex);
	
	nv.NVIC_IRQChannel = EXTI0_IRQn;
	nv.NVIC_IRQChannelPreemptionPriority = 0;
	nv.NVIC_IRQChannelSubPriority = 1;
	nv.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nv);
	///////////////////
	
	
  I2C_DeInit(I2C1); 
  i2c.I2C_ClockSpeed = 100000;
  i2c.I2C_Mode = I2C_Mode_I2C; 
  i2c.I2C_DutyCycle = I2C_DutyCycle_2; 
  i2c.I2C_OwnAddress1 = 0x00; 
  i2c.I2C_Ack = I2C_Ack_Enable; 
	i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2C1, &i2c);

  I2C_Cmd(I2C1, ENABLE);
}

void I2C1_WriteBit(uint8_t Addr, uint8_t regAddr, uint8_t data){
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	
	I2C_GenerateSTART(I2C1, ENABLE);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1, Addr << 1, I2C_Direction_Transmitter);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	I2C_SendData(I2C1, regAddr);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_SendData(I2C1, data);
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTOP(I2C1, ENABLE);
}

void I2C1_ReadBit(uint8_t Addr, uint8_t regAddr, uint8_t *buf, uint8_t length){
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)){
	}
	
  I2C_GenerateSTART(I2C1, ENABLE);
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
  I2C_Send7bitAddress(I2C1, Addr << 1, I2C_Direction_Transmitter);
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    
  I2C_SendData(I2C1, regAddr);
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
  I2C_GenerateSTART(I2C1, ENABLE);
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
  I2C_Send7bitAddress(I2C1, Addr << 1, I2C_Direction_Receiver);
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    
  while(length) {
    if(length == 1) {
      I2C_AcknowledgeConfig(I2C1, DISABLE);
      I2C_GenerateSTOP(I2C1, ENABLE);
    }
        
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    
		  *buf = I2C_ReceiveData(I2C1);
      buf++;
      length--;
  }
    I2C_AcknowledgeConfig(I2C1, ENABLE);
}


void MAX30102_Init(void){
	MAX_Time_Init();
	I2C_Config();
	uint8_t configValue;
	
	I2C1_WriteBit(MAX_ADDR, REG_MODE_CONFIG, MODE_RESET);
	MAX_Delay(100);
	
	configValue = FIFO_SMP_AVE_4 | FIFO_ROLLOVER_ENABLE | FIFO_A_FULL_15;
	I2C1_WriteBit(MAX_ADDR, REG_FIFO_CONFIG, configValue);
	
	configValue = MODE_SP02; 
	I2C1_WriteBit(MAX_ADDR, REG_MODE_CONFIG, configValue);
	
	configValue = SP02_ADC_RGE_4096 | SP02_SR_100 | SP02_LED_PW_411;
	I2C1_WriteBit(MAX_ADDR, REG_SP02_CONFIG, configValue);
	
	I2C1_WriteBit(MAX_ADDR, REG_PULSE_AMP_1, LED_CURRENT);
	I2C1_WriteBit(MAX_ADDR, REG_PULSE_AMP_2, LED_CURRENT);
	
	I2C1_WriteBit(MAX_ADDR, REG_WR_PTR_CONF, ERASER_POINTER);
	I2C1_WriteBit(MAX_ADDR, REG_OVF_COUNTER, ERASER_POINTER);
	I2C1_WriteBit(MAX_ADDR, REG_RD_PTR_CONF, ERASER_POINTER);
	
	configValue = PPG_RDY_EN;
	I2C1_WriteBit(MAX_ADDR, REG_INTER_ENABL, configValue);
	
	uint8_t temp;
	I2C1_ReadBit(MAX_ADDR, 0x00, &temp, 1);
}

void EXTI0_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line0) != RESET){
		Flag_Data_Ready = 1;
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}


static void MAX30102_ReadFIFO(void){
	uint8_t buffer[6];
	uint8_t temp_status;
	I2C1_ReadBit(MAX_ADDR, REG_FIFO_DATA, buffer, 6);
	
	_red_sample = ((buffer[0] & 0x03) << 16) | (buffer[1] << 8) | (buffer[2]);
	ir_sample = ((buffer[3] & 0x03) << 16) | (buffer[4] << 8) | (buffer[5]);


	I2C1_ReadBit(MAX_ADDR, 0x00, &temp_status, 1);
}

uint32_t MAX30102_GetRawIR(void) { 
	return ir_sample; 
}
uint32_t MAX30102_GetRawRed(void) { 
	return _red_sample; 
}

bool MAX30102_HasFinger(void){
  if(ir_sample > FINGER_THRESHOLD){
    return true;
  }
  return false;
}


static bool checkForBeat(uint32_t sample){
	float sample_f = (float)sample;
	
	dcFilterIR = 0.95 * dcFilterIR + 0.05 * sample_f;
	float acValue = sample_f - dcFilterIR;
	
	if(acValue < -20 && (MAX_Millis() - lastBeat > 250)){
		return true;
	}
	return false;
}




uint32_t MAX30102_GetBPM(void){
  if(Flag_Data_Ready == 1){
    Flag_Data_Ready = 0; 
        
    MAX30102_ReadFIFO();
        
    if(ir_sample < 5000){
      beatAvg = 0;
      dcFilterIR = 0;
    } else {
      if(checkForBeat(ir_sample)){
        delta = MAX_Millis() - lastBeat;
        lastBeat = MAX_Millis();
        uint32_t beatMinute = 60000 / delta;
                       
				if(beatMinute < 255 && beatMinute > 20){                
					if(beatAvg == 0) beatAvg = beatMinute;
					else beatAvg = (beatAvg * 3 + beatMinute) / 4;
				}
			}    
		}
  }
    
  return beatAvg;
}
