#include "setup.h"

IRSensor_TypedefStruct ir;
PID_TypedefStruct pid;

uint16_t black[8];
uint16_t white[8];
float position;
float OUT_SPEED;

void Setup_All(void){
	delay_Init();
	Motor_Config(1000, 72);
	DMA_Config();
	ADC_Config();
}

void Following_Line(void){
	// doc vi tri
	IRSensor_Read(&ir, ADC_data);
	if(ir.position != -999){ 	
		position = ir.position;
	// tinh toan pid
		OUT_SPEED = PID_Update(&pid, 0.0f, ir.position, 0);
	
		float left_speed = base_speed - OUT_SPEED;
		float right_speed = base_speed + OUT_SPEED;
	
		if(left_speed > max_speed) 
			left_speed = max_speed;
		if(left_speed < -max_speed)
			left_speed = -max_speed;
		if(right_speed > max_speed) 
			right_speed = max_speed;
		if(right_speed < -max_speed)
			right_speed = -max_speed;
	
	
	Motor_Controll(left_speed, right_speed);

	}
}

void Calib_White(void){
	for(int i = 0 ; i < 8 ; i++){
		white[i] = ADC_data[i];
	}
}

void Calib_Black(void){
	for(int i = 0 ; i < 8 ; i++){
		black[i] = ADC_data[i];
	}
}
void IR_PID_Init(void){
	IRSensor_SetCalibration(black, white);
	PID_Init(&pid, 175.0f, 0.0f, 60.0f, 100.f);
}
