#include "exti.h"
void NvicConfig()
{
	NVIC_InitTypeDef NVIC_InitStructe;
	NVIC_InitStructe.NVIC_IRQChannel=EXTI0_IRQn;
	NVIC_InitStructe.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructe.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructe.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructe);
}

void ExtiConfig()
{	
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStructe;
	NvicConfig();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Pin=EXTI_KEY_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_IN_FLOATING;
	GPIO_Init(EXTI_KEY_GPIO_PORT, &GPIO_InitStruct);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	EXTI_InitStructe.EXTI_Line=EXTI_Line0;
	EXTI_InitStructe.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructe.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_InitStructe.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStructe);
}
