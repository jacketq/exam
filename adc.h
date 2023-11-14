#ifndef _ADC_H
#define _ADC_H
#include "stm32f10x.h"
#define GPIO_ADC_PORT  GPIOC
#define GPIO_ADC_PIN   GPIO_Pin_1
#define RCC_ADC_GPIO   RCC_APB2Periph_GPIOC
void Adc_Dma_Config(void);
void Adc_Config(void);
void Adc_GPIO_Config(void);
void Adcc_Init(void);
#endif

