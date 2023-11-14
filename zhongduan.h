#ifndef _ZHONGDUAN_H
#define _ZHONGDUAN_H
#include "stm32f10x.h"
void SysTick_Handler(void);
void EXTI0_IRQHandler(void);
void DEBUG_USART_IRQHandler(void);
char *str(void);
#endif

