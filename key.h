#ifndef _KEY_H
#define _KEY_H
#include "stm32f10x.h"
#define KEY_GPIO_PORT  GPIOA
#define KEY_GPIO_PIN   GPIO_Pin_0
#define RCC_KEY_GPIO   RCC_APB2Periph_GPIOA
#define key_ON     1
#define key_OFF    0
void keyconfig(void);
unsigned char keysan(GPIO_TypeDef* KEY_GPIO_POR,unsigned short int KEY_GPIO_PI);

#endif

