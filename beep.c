#include "beep.h"
void BeepInit()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin=BEEP_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_Out_PP;
	GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStruct);
	GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
}
void BeepOn()
{
	GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
}
void BeepOff()
{
	GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
}


char *Beepstatus(void)
{
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)==0)
		return "off";
	else 
		return "on";
}

