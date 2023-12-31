#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"
void SysTick_Init(void);
void Delay_ms(__IO u32 ms);
void TimingDelay_Decrement(void);
void Delay_us(__IO u32 us);
void SysTick_Delay_Us( __IO uint32_t us);
void SysTick_Delay_Ms( __IO uint32_t ms);


#endif /* __SYSTICK_H */
