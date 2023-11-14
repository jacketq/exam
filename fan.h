#ifndef _WIFI_H
#define _WIFI_H

#include "stm32f10x.h"
#define FAN_GPIO_PORT  GPIOA
#define FAN_GPIO_PIN   GPIO_Pin_5
#define FAN_RCC_GPIO   RCC_APB2Periph_GPIOA
void Fengconfig(void);
void FanOn(void);
void FanOff(void);
char *Fanstatus(void);
#endif

