#ifndef _LED_H
#define _LED_H
#include "stm32f10x.h"
#define GPIO_PORT  GPIOB
#define GPIO_PIN   GPIO_Pin_1
#define RCC_GPIO   RCC_APB2Periph_GPIOB
#define lerterggit   {GPIO_PORT->ODR ^=GPIO_PIN;}
void Ledconfig(void);
void BlueOn(void);
void BlueOff(void);
void RedOn(void);
void RedOff(void);
void GreenOn(void);
void GreenOff(void);
char *Ledtstatus(void);
#endif

