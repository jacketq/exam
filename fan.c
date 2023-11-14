#include "fan.h"
void Fengconfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(FAN_RCC_GPIO, ENABLE);
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin=FAN_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_Out_PP;
	GPIO_Init(FAN_GPIO_PORT, &GPIO_InitStruct);
	GPIO_SetBits(FAN_GPIO_PORT, FAN_GPIO_PIN);
}

void FanOn(void)
{
	GPIO_ResetBits(FAN_GPIO_PORT, FAN_GPIO_PIN);
}

void FanOff(void)
{
	GPIO_SetBits(FAN_GPIO_PORT, FAN_GPIO_PIN);
}

char *Fanstatus(void)
{
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0)
		return "on";
	else 
		return "off";
}
