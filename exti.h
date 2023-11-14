#ifndef _EXTI_H
#define _EXTI_H
#include "stm32f10x.h"
#define EXTI_KEY_GPIO_PORT  GPIOA
#define EXTI_KEY_GPIO_PIN   GPIO_Pin_0
void NvicConfig(void);
void ExtiConfig(void);
#endif

