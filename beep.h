#ifndef _BEEP_H
#define _BEEP_H
#include "stm32f10x.h"
#define BEEP_GPIO_PORT  GPIOA
#define BEEP_GPIO_PIN   GPIO_Pin_8
void BeepInit(void);
void BeepOn(void);
void BeepOff(void);
char *Beepstatus(void);
#endif
