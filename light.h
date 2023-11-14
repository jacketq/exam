#ifndef _LIGHT_H
#define _LIGHT_H

#include "stm32f10x.h"
#define LIGHT_GPIO_PORT  GPIOA
#define LIGHT_GPIO_PIN   GPIO_Pin_4
#define LIGHT_RCC_GPIO   RCC_APB2Periph_GPIOA
void Lightconfig(void);
void LightOn(void);
void LightOff(void);
char *Lightstatus(void);
#endif


