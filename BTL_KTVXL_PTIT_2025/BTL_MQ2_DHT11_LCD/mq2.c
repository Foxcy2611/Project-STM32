#include "mq2.h"


void ADC_Config(void){
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    
  GPIO_InitTypeDef gp;
  gp.GPIO_Pin = GPIO_Pin_0;
  gp.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &gp);
    
  ADC_InitTypeDef adc;
  adc.ADC_Mode = ADC_Mode_Independent;
  adc.ADC_ScanConvMode = DISABLE;     
  adc.ADC_ContinuousConvMode = ENABLE; 
  adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  adc.ADC_DataAlign = ADC_DataAlign_Right;
  adc.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &adc);

  ADC_Cmd(ADC1, ENABLE);

  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));
}

uint16_t ADC_Read(void){
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);

  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

  return ADC_GetConversionValue(ADC1);
}
