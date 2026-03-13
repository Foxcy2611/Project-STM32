#ifndef __DELAY__
#define __DELAY__
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"                  // Device header

void delay_Init(void);
extern __inline uint32_t millis(void);
void delay_ms(uint32_t time_delay);

#ifdef __cplusplus
}
#endif
#endif
