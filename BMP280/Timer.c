#include "Timer.h"


void Timer_Init()
{
	RCC->APB2ENR &= ~(1 << 11);												// clear bit 11 for Timer1
	RCC->APB2ENR |= 1 << 11;													// set bit 11 for Timer 1
	TIM1->PSC |= 72 - 1;															// set prescaler = 71
	TIM1->ARR |= 0xFFFF;
	TIM1->CR1 |= 0x01;																// enable timer
	TIM1->EGR |= 0x01;																// rollover CNT
}

void delay_ms(uint32_t time)
{
	while(time)
	{
		TIM1->CNT = 0U;
		while(TIM1 -> CNT < 1000);
		time--;
	}
}

void delay_us(uint32_t time)
{
		TIM1->CNT = 0U;
		while(TIM1 -> CNT < time);
}
