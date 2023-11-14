#include "i2c.h"
void I2c_Config(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	// 打开串口GPIO的时钟
	DEBUG_I2Cx_GPIO_APBxClkCmd(DEBUG_I2Cx_GPIO_CLK, ENABLE);
	
	// 打开串口外设的时钟
	DEBUG_I2Cx_APBxClkCmd(DEBUG_I2Cx_CLK, ENABLE);

	// 将I2C SCL的GPIO配置为开漏复用模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_I2Cx_SCL_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_I2Cx_SCL_GPIO_PORT , &GPIO_InitStructure);

  // 将I2C SDA的GPIO配置为开漏复用模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_I2Cx_SDA_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_I2Cx_SDA_GPIO_PORT, &GPIO_InitStructure);
	
	//I2C初始化
	I2C_InitStructure.I2C_ClockSpeed=DEBUG_I2Cx_BAUDRATE;
	I2C_InitStructure.I2C_Mode=I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle=I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1=I2C_ADRESS;
	I2C_InitStructure.I2C_Ack=I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;
	I2C_Init(DEBUG_I2Cx, &I2C_InitStructure);
	I2C_Cmd(DEBUG_I2Cx, ENABLE);
}

void WriteStatus(unsigned char *x)
{
	
	I2C_GenerateSTART(DEBUG_I2Cx, ENABLE);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)==ERROR);
	I2C_Send7bitAddress(DEBUG_I2Cx, EEPROM, I2C_Direction_Transmitter);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)==ERROR);
	I2C_SendData(DEBUG_I2Cx, 0x08);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING)==ERROR);
	I2C_SendData(DEBUG_I2Cx, x[0]);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)==ERROR);
	I2C_SendData(DEBUG_I2Cx, x[1]);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)==ERROR);
	I2C_SendData(DEBUG_I2Cx, x[2]);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)==ERROR);
	I2C_SendData(DEBUG_I2Cx, x[3]);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)==ERROR);
	I2C_SendData(DEBUG_I2Cx, x[4]);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)==ERROR);
	I2C_SendData(DEBUG_I2Cx, x[5]);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)==ERROR);
	I2C_SendData(DEBUG_I2Cx, x[6]);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)==ERROR);
	I2C_GenerateSTOP(DEBUG_I2Cx, ENABLE);
}

void ReadStatus(unsigned char *x)
{
	
	I2C_GenerateSTART(DEBUG_I2Cx, ENABLE);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)==ERROR);
	I2C_Send7bitAddress(DEBUG_I2Cx, EEPROM, I2C_Direction_Transmitter);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)==ERROR);
	I2C_SendData(DEBUG_I2Cx, 0x08);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING)==ERROR);
	
	I2C_GenerateSTART(DEBUG_I2Cx, ENABLE);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)==ERROR);
	I2C_Send7bitAddress(DEBUG_I2Cx, EEPROM, I2C_Direction_Receiver);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)==ERROR);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==ERROR);
	x[0]=I2C_ReceiveData(DEBUG_I2Cx);
	I2C_AcknowledgeConfig(DEBUG_I2Cx, ENABLE);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==ERROR);
	x[1]=I2C_ReceiveData(DEBUG_I2Cx);
	I2C_AcknowledgeConfig(DEBUG_I2Cx, ENABLE);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==ERROR);
	x[2]=I2C_ReceiveData(DEBUG_I2Cx);
	I2C_AcknowledgeConfig(DEBUG_I2Cx, ENABLE);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==ERROR);
	x[3]=I2C_ReceiveData(DEBUG_I2Cx);
	I2C_AcknowledgeConfig(DEBUG_I2Cx, ENABLE);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==ERROR);
	x[4]=I2C_ReceiveData(DEBUG_I2Cx);
	I2C_AcknowledgeConfig(DEBUG_I2Cx, ENABLE);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==ERROR);
	x[5]=I2C_ReceiveData(DEBUG_I2Cx);
	I2C_AcknowledgeConfig(DEBUG_I2Cx, ENABLE);
	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==ERROR);
	x[6]=I2C_ReceiveData(DEBUG_I2Cx);
	I2C_AcknowledgeConfig(DEBUG_I2Cx, DISABLE);
	I2C_GenerateSTOP(DEBUG_I2Cx, ENABLE);
}
//void Start_Mesu(void)
//{
//	I2C_GenerateSTART(DEBUG_I2Cx, ENABLE);
//	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)==ERROR);
//	I2C_Send7bitAddress(DEBUG_I2Cx, GUANG, I2C_Direction_Transmitter);
//	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)==ERROR);
//	I2C_SendData(DEBUG_I2Cx, 0x10);
//	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING)==ERROR)
//	I2C_GenerateSTOP(DEBUG_I2Cx, ENABLE);
//}

//void ReceiveData(char *a)
//{
//	I2C_GenerateSTART(DEBUG_I2Cx, ENABLE);
//	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)==ERROR);
//	I2C_Send7bitAddress(DEBUG_I2Cx, GUANG, I2C_Direction_Receiver);
//	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)==ERROR);
//	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==ERROR);
//	a[0]=I2C_ReceiveData(DEBUG_I2Cx);
//	I2C_AcknowledgeConfig(DEBUG_I2Cx, ENABLE);
//	while(I2C_CheckEvent(DEBUG_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==ERROR);
//	a[1]=I2C_ReceiveData(DEBUG_I2Cx);
//	I2C_AcknowledgeConfig(DEBUG_I2Cx, DISABLE);
//	I2C_GenerateSTOP(DEBUG_I2Cx, ENABLE);
//	I2C_AcknowledgeConfig(DEBUG_I2Cx, ENABLE);
//}

