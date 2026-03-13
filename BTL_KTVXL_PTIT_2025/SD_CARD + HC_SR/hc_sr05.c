#include "hc_sr05.h"

#define HCSR05_PORT   GPIOA
#define HCSR05_TRIG_PIN    GPIO_Pin_1

#define HCSR05_ECHO_PIN    GPIO_Pin_0

// Bien volatile de luu thoi gian echo (don vi: micro giay)
static volatile uint16_t echoTime = 0;
// Flag bao hieu da co ket qua echo san sang doc
static volatile bool echoReady = false;
// Flag bao hieu da gui trigger
static volatile bool triggerSent = false;
// Flag cho biet dang cho falling edge (da bat dau dem)
static volatile bool wattingFalling  =false;

/*
 * Ham: Timer2_Init
 * Muc dich: Cau hinh TIM2 lam bo dem thoi gian cho do ECHO (don vi tick = 1 us)
 * Ghi chu:
 *  - Prescaler = 72-1 voi SystemCoreClock = 72 MHz => timer tick = 1 MHz => 1 tick = 1 us
 *  - Period = 0xFFFF (24 bit/16 bit overflow) de tranh tran som
 */
static void Timer2_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseInitTypeDef tim;
	tim.TIM_ClockDivision =  TIM_CKD_DIV1;
	tim.TIM_CounterMode = TIM_CounterMode_Up;
	tim.TIM_Period = 0xFFFF;
	tim.TIM_Prescaler = 72 - 1;
	
	TIM_TimeBaseInit(TIM2, &tim);
	TIM_Cmd(TIM2, ENABLE);
}

/*
 * Ham: HCSR05_Delay_Us
 * Muc dich: Tao delay chinh xac theo micro giay su dung TIM2
 * Cach hoat dong:
 *  - Dat counter = 0, sau do cho den khi TIM_GetCounter >= 1 (1 tick = 1 us)
 *  - Lap lai us lan
 * Ghi chu:
 *  - Ham block CPU trong qua trinh delay
 */
static void HCSR05_Delay_Us(uint16_t us)
{
	while (us--)
	{
		// Dat counter ve 0 de bat dau dem moi
		TIM_SetCounter(TIM2, 0);
		// Cho den khi counter tang den 1 (tuong ung ~1 us)
		while (TIM_GetCounter(TIM2) < 1);
	}
}

/*
 * Ham: TrigEcho_Init
 * Muc dich: Cau hinh GPIO cho TRIG va ECHO, cau hinh EXTI va NVIC cho chan ECHO
 * Cach hoat dong:
 *  - PA1: TRIG (output push-pull)
 *  - PA0: ECHO (input floating)
 *  - N?i EXTI line 0 v?i PA0, set trigger = rising | falling
 *  - Enable ngat EXTI0 trong NVIC
 */
static void TrigEcho_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef gpio;
	
	// Cau hinh chan Trig là PA1
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = HCSR05_TRIG_PIN;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HCSR05_PORT, &gpio);
	
	// Cau hinh chan Echo là PA0
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = HCSR05_ECHO_PIN;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HCSR05_PORT, &gpio);
	
	// Ket noi EXTI0 voi PA0
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	EXTI_InitTypeDef exti;
	exti.EXTI_Line = EXTI_Line0;
	exti.EXTI_LineCmd = ENABLE;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&exti);
	
	NVIC_InitTypeDef nvic;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	nvic.NVIC_IRQChannel = EXTI0_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 1;
	nvic.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvic);
}

void EXTI0_IRQHandler(void);
/*
 * Ham: TrigEcho_Init
 * Muc dich: Cau hinh GPIO cho TRIG va ECHO, cau hinh EXTI va NVIC cho chan ECHO
 * Cach hoat dong:
 *  - PA1: TRIG (output push-pull)
 *  - PA0: ECHO (input floating)
 *  - N?i EXTI line 0 v?i PA0, set trigger = rising | falling
 *  - Enable ngat EXTI0 trong NVIC
 */
void EXTI0_IRQHandler(void)
{
	if (EXTI_GetFlagStatus(EXTI_Line0) == SET)
	{
		uint8_t state = GPIO_ReadInputDataBit(HCSR05_PORT, HCSR05_ECHO_PIN);
		if (triggerSent)
		{
			if (state)
			{
				TIM_SetCounter(TIM2, 0);
				wattingFalling = true;
			}
			else if (wattingFalling)
			{
				echoTime = TIM_GetCounter(TIM2);
				echoReady  = true;
				triggerSent = false;
				wattingFalling = false;
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

/*
 * Ham: HCSR05_Init
 * Muc dich: Khoi tao toan bo modul HC-SR05: timer + GPIO/EXTI
 * Dat cac flag ban dau ve false
 */
void HCSR05_Init(void)
{
	Timer2_Init();
	TrigEcho_Init();
	
	echoReady = false;
	triggerSent = false;
	wattingFalling = false;
}

/*
 * Ham: HCSR05_Trigger
 * Muc dich: Phat mot xung kich (10 us) o chan TRIG de cam bien phat song
 * Luong thuc:
 *  - Dat echoReady = false va thong bao triggerSent = true
 *  - Tao xung TRIG: 0 -> 1 trong 10 us -> 0
 *  - Sau khi TRIG duoc gui, EXTI se bat dau bat rising/falling tren ECHO
 */
void HCSR05_Trigger(void)
{
	echoReady = false;
	triggerSent = true;
	wattingFalling = false;
	
	GPIO_ResetBits(HCSR05_PORT, HCSR05_TRIG_PIN);
	HCSR05_Delay_Us(2);
	GPIO_SetBits(HCSR05_PORT, HCSR05_TRIG_PIN);
	HCSR05_Delay_Us(10);
	GPIO_ResetBits(HCSR05_PORT, HCSR05_TRIG_PIN);
}

/*
 * Ham: HCSR05_IsReady
 * Muc dich: Tra ve trang thai neu da co ket qua do
 * Tra ve true neu echoReady = true
 */
bool HCSR05_IsReady(void)
{
	return echoReady;
}

/*
 * Ham: HCSR05_GetDistance
 * Muc dich: Neu da co ket qua (echoReady), tinh va tra ve khoang cach (don vi: cm).
 * Luong thuc:
 *  - Neu echoReady false: tra ve -1.0f (khong hop le)
 *  - Tinh distance = echoTime / 58.0 (cong thuc thong dung)
 *  - Neu distance < 2 cm hoac > 400 cm => tra ve -1.0f (ngoai pham vi)
 *  - Dat echoReady = false de cho lan do tiep theo
 */
float HCSR05_GetDistance(void)
{
	if (!echoReady) return -1.0f;
	
	echoReady  = false;
	float distance  = (float)echoTime / (58.0f);
	
	if (distance < 2.0f || distance > 400.0f) return -1.0f;
	
	return distance;
}
