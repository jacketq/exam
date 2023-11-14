#include "led.h"
void Ledconfig()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_GPIO, ENABLE);
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin=GPIO_PIN|GPIO_Pin_5|GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_PORT, &GPIO_InitStruct);
	GPIO_SetBits(GPIO_PORT, GPIO_PIN);
	GPIO_SetBits(GPIO_PORT, GPIO_Pin_5);
	GPIO_SetBits(GPIO_PORT, GPIO_Pin_0);
}

void BlueOn(void)
{
	GPIO_ResetBits(GPIO_PORT, GPIO_PIN);
}

void BlueOff(void)
{
	GPIO_SetBits(GPIO_PORT, GPIO_PIN);
}

void RedOn(void)
{
	GPIO_ResetBits(GPIO_PORT, GPIO_Pin_5);
}

void RedOff(void)
{
	GPIO_SetBits(GPIO_PORT, GPIO_Pin_5);
}

void GreenOn(void)
{
	GPIO_ResetBits(GPIO_PORT, GPIO_Pin_0);
}

void GreenOff(void)
{
	GPIO_SetBits(GPIO_PORT, GPIO_Pin_0);
}


char *Ledtstatus(void)
{
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==0)
		return "on";
	else 
		return "off";
}
