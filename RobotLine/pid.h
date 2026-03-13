/*
 * @file       pid library
 * @board      STM32F10x
 * @author     Tong Sy Tan
 * @date       Sun, 09/02/2025
*/


#ifndef __PID__
#define __PID__
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"                  // Device header

typedef struct
{
	float kp;
	float ki;
	float kd;
	float error;
	float pre_pv;
	float P;
	float I;
	float D;
	float output;
	float max_output;
	
	uint32_t cur_time;
	uint32_t pre_time;
	uint32_t dt;
	
}PID_TypedefStruct;

void PID_Init(PID_TypedefStruct * PID, float kp, float ki, float kd, float max_output);
float PID_Update(PID_TypedefStruct *PID, float sp, float pv, float ubias);
void PID_Reset(PID_TypedefStruct *PID);
	
#ifdef __cplusplus
}
#endif
#endif
