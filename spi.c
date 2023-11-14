#include "spi.h"

void Spi_Gpio_Config()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	GPIO_SetBits(GPIOC, GPIO_Pin_0);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Spi_Config()
{
	SPI_InitTypeDef SPI_InitStructe;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	SPI_InitStructe.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_256;
	SPI_InitStructe.SPI_CPHA=SPI_CPHA_2Edge ;
	SPI_InitStructe.SPI_CPOL=SPI_CPOL_High  ;
	SPI_InitStructe.SPI_CRCPolynomial=7;
	SPI_InitStructe.SPI_DataSize=SPI_DataSize_8b;
	SPI_InitStructe.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructe.SPI_FirstBit=SPI_FirstBit_MSB;
	SPI_InitStructe.SPI_Mode=SPI_Mode_Master;
	SPI_InitStructe.SPI_NSS=SPI_NSS_Soft;
	SPI_Init(SPI1, &SPI_InitStructe);
	SPI_Cmd(SPI1, ENABLE);
}

void Spi_Init()
{
	 Spi_Gpio_Config();
	 Spi_Config();
}

char Spi_SendByte(char data)
{
	SPI_I2S_SendData(SPI1, data);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET);
	return SPI_I2S_ReceiveData(SPI1);
	
}

char Spi_ReceiveByte()
{	
	return (Spi_SendByte(0x00));
}

unsigned int SPI_FLASH_ReadID(void) 
{
 unsigned int Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

 /* 开始通讯：CS 低电平 */
 GPIO_ResetBits(GPIOC, GPIO_Pin_0);

 /* 发送 JEDEC 指令，读取 ID */
 Spi_SendByte(0x9F);

 /* 读取一个字节数据 */
 Temp0 = Spi_SendByte(0xFF);

 /* 读取一个字节数据 */
 Temp1 = Spi_SendByte(0xFF);

 /* 读取一个字节数据 */
 Temp2 = Spi_SendByte(0xFF);

 /* 停止通讯：CS 高电平 */
 /* 把数据组合起来，作为函数的返回值 */
 Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
	
 GPIO_SetBits(GPIOC, GPIO_Pin_0);
 
 return Temp;
}

