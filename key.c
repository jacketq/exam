#include "key.h"
void keyconfig()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_KEY_GPIO, ENABLE);
	GPIO_InitStruct.GPIO_Pin=KEY_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_IN_FLOATING;
	GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStruct);
}
unsigned char keysan(GPIO_TypeDef* KEY_GPIO_POR,unsigned short int KEY_GPIO_PI)
{
	if(GPIO_ReadInputDataBit(KEY_GPIO_POR,KEY_GPIO_PI)==key_ON)
	{	
		while(GPIO_ReadInputDataBit(KEY_GPIO_POR,KEY_GPIO_PI)==key_ON);
		return key_ON;
	}
	else
	{
		return key_OFF;
	}	
}

