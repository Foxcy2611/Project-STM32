#include "gp2y.h"

// ham khoi tao timer2 de tao delay us va ms
void TIM2_Delay(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseInitTypeDef TM;
	TM.TIM_ClockDivision = TIM_CKD_DIV1;
	TM.TIM_CounterMode = TIM_CounterMode_Up;
	TM.TIM_Period = 0xFFFF;     // dem toi da
	TM.TIM_RepetitionCounter = 0;
	TM.TIM_Prescaler = 71;      // 72mhz / (71+1) = 1mhz ? 1 tick = 1us
	TIM_TimeBaseInit(TIM2, &TM);
	
	TIM_Cmd(TIM2, ENABLE);      // bat timer2
}

// delay us bang timer counter
void Delay_us(uint16_t us){
	TIM_SetCounter(TIM2, 0);            // reset counter
	while(TIM_GetCounter(TIM2) < us);   // cho den khi dem du
}

// delay ms bang goi Delay_us
void Delay_ms(uint16_t ms){
	while(ms--){
			Delay_us(1000);
	}
}

// khoi tao cam bien GP2Y
void GP2Y_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef gp;
	ADC_InitTypeDef adc;
	
	// PA0 lam chan analog input
	gp.GPIO_Mode = GPIO_Mode_AIN;
	gp.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &gp);
	
	// PA1 dieu khien led hong ngoai cua cam bien (output push pull)
	gp.GPIO_Mode = GPIO_Mode_Out_PP;
	gp.GPIO_Pin = GPIO_Pin_1;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gp);

	// cau hinh adc doc 1 kenh
	adc.ADC_ContinuousConvMode = DISABLE;
	adc.ADC_DataAlign = ADC_DataAlign_Right;
	adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	adc.ADC_Mode = ADC_Mode_Independent;
	adc.ADC_NbrOfChannel = 1;
	adc.ADC_ScanConvMode = DISABLE;
	ADC_Init(ADC1, &adc);
	
	// kenh adc0, thoi gian lay mau 55.5 chu ky
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	
	ADC_Cmd(ADC1, ENABLE);

	// reset + calibrate
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
}

// doc gia tri tu cam bien GP2Y
uint16_t GP2Y_Read(void){
	uint16_t adc_val = 0;

	GPIO_SetBits(GPIOA, GPIO_Pin_1); // bat led hong ngoai
	Delay_us(280);                   // doi dung 280us (theo datasheet)

	// bat dau chuyen doi adc
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	adc_val = ADC_GetConversionValue(ADC1);
	
	Delay_us(40);                    // doi 40us
	GPIO_ResetBits(GPIOA, GPIO_Pin_1); // tat led
	
	return adc_val;
}

// tinh mat do bui du tren dien ap
float GP2Y_Caculate(float volt){
	float K_Stan = 5.0f;  // he so do do theo datasheet
	
	// gia tri voc khi khong co bui, neu muon hieu chinh thi tang/sua tham so nay
	float Voc = 0; 
	
	float den = 0;
	if(volt > Voc){
			den = (volt - Voc) / K_Stan;    // cong thuc tinh mat do bui
	}
	return den;    
}

// tra ve gia tri bui mg/m3
float GP2Y_GetMgm3(void){
	uint16_t raw_val = GP2Y_Read();
	
	// neu co cau chia ap thi nhan 2
	// float volt_val = ((raw_val * 3.3f) / 4095.0f) * 2.0f;
	
	// neu noi truc tiep vao pa0
	float volt_val = (raw_val * 3.3f) / 4095.0f;
	
	// nhan 1000 de ra don vi mg/m3 -> ug/m3
	return GP2Y_Caculate(volt_val) * 1000.0f; 
}
