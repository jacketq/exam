#include "light.h"
void Lightconfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(LIGHT_RCC_GPIO, ENABLE);
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin=LIGHT_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_Out_PP;
	GPIO_Init(LIGHT_GPIO_PORT, &GPIO_InitStruct);
	GPIO_SetBits(LIGHT_GPIO_PORT, LIGHT_GPIO_PIN);
}

void LightOn(void)
{
	GPIO_ResetBits(LIGHT_GPIO_PORT, LIGHT_GPIO_PIN);
}

void LightOff(void)
{
	GPIO_SetBits(LIGHT_GPIO_PORT, LIGHT_GPIO_PIN);
}

char *Lightstatus(void)
{
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)==0)
		return "on";
	else 
		return "off";
}
