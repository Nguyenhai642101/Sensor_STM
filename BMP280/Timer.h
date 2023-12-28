#ifndef __TIMER_H__
#define __TIMER_H__
#include "stm32f10x.h"
void Timer_Init(void);
void delay_ms(uint32_t time);
void delay_us(uint32_t time);

#endif
