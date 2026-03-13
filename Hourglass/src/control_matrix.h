#ifndef __CONTROL__MATRIX__H
#define __CONTROL__MATRIX__H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM

#include "mpu6050.h"
#include "max7219.h"
#include "uart.h"
#include "delay.h"

#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"
#include "stdint.h"
#include "string.h"

#define Speed_Sand 100

extern volatile int8_t Gravity;

void LC_ClearDisplay(uint8_t addr);
void LC_SetXY(uint8_t addr, uint8_t x, uint8_t y, bool state);
bool LC_GetXY(uint8_t addr, uint8_t x, uint8_t y);
uint8_t Delay_Check(uint32_t ms, int8_t cur_gravity);

void TIM2_NVIC(void);

void Fill_Sand(uint8_t addr);
void Half_Sand(uint8_t addr);


void Start_Sand_Vertical(void);
void Stun_Fall_Vertical(int x_start);
void Start_Sand_Horizontal(void);
void Stun_Fall_Horizontal(int x_start);

void Update_Sand_Vertical(uint8_t addr);
void Update_Matrix_Horizontal(uint8_t addr);
void Update_Matrix_Left(uint8_t addr);
void Update_Matrix_Right(uint8_t addr);

#endif
