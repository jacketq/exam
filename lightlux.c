#include "lightlux.h"

void LIGHTLUX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(LIGHTLUX_SCL_GPIO_CLK | LIGHTLUX_SDA_GPIO_SDA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = LIGHTLUX_SCL_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LIGHTLUX_SCL_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = LIGHTLUX_SDA_GPIO_PIN;
  GPIO_Init(LIGHTLUX_SDA_GPIO_PORT, &GPIO_InitStructure);
}


void LSDA_OUT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = LIGHTLUX_SDA_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LIGHTLUX_SDA_GPIO_PORT, &GPIO_InitStructure);
}

void LSDA_IN(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = LIGHTLUX_SDA_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(LIGHTLUX_SDA_GPIO_PORT, &GPIO_InitStructure);
}

//����IIC��ʼ�ź�
void LIGHTLUX_IIC_Start(void)
{
  LSDA_OUT();
  GPIO_SetBits(GPIOA, GPIO_Pin_12);
  GPIO_SetBits(GPIOA, GPIO_Pin_11);
  Delay_us(20);

  GPIO_ResetBits(GPIOA, GPIO_Pin_12);	//START:when CLK is high,DATA change form high to low
  Delay_us(20);
  GPIO_ResetBits(GPIOA, GPIO_Pin_11); 	//ǯסI12C���ߣ�׼�����ͻ��������
}

//����IICֹͣ�ź�
void LIGHTLUX_IIC_Stop(void)
{
  LSDA_OUT();
  GPIO_ResetBits(GPIOA, GPIO_Pin_11);
  GPIO_ResetBits(GPIOA, GPIO_Pin_12);	//STOP:when CLK is high DATA change form low to high
  Delay_us(20);
  GPIO_SetBits(GPIOA, GPIO_Pin_11);
  GPIO_SetBits(GPIOA, GPIO_Pin_12);	//����I12C���߽����ź�
  Delay_us(20);
}

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 LIGHTLUX_IIC_Wait_Ack(void)
{
  u8 ucErrTime = 0;
  LSDA_IN();
  GPIO_SetBits(GPIOA, GPIO_Pin_12);
  Delay_us(10);
  GPIO_SetBits(GPIOA, GPIO_Pin_11);
  Delay_us(10);
  while(LIGHTLUX_SDA_READ())
  {
    ucErrTime++;
    if(ucErrTime > 250)
    {
      LIGHTLUX_IIC_Stop();
      return 1;
    }
  }
  GPIO_ResetBits(GPIOA, GPIO_Pin_11);  		//ʱ�����0
  return 0;
}

//����ACKӦ��
void LIGHTLUX_IIC_Ack(void)
{
  GPIO_ResetBits(GPIOA, GPIO_Pin_11);
  LSDA_OUT();
  GPIO_ResetBits(GPIOA, GPIO_Pin_12);
  Delay_us(20);
  GPIO_SetBits(GPIOA, GPIO_Pin_11);
  Delay_us(20);
  GPIO_ResetBits(GPIOA, GPIO_Pin_11);
}

//������ACKӦ��
void LIGHTLUX_IIC_NAck(void)
{
  GPIO_ResetBits(GPIOA, GPIO_Pin_11);
  LSDA_OUT();
  GPIO_SetBits(GPIOA, GPIO_Pin_12);
  Delay_us(20);
  GPIO_SetBits(GPIOA, GPIO_Pin_11);
  Delay_us(20);
  GPIO_ResetBits(GPIOA, GPIO_Pin_11);
}

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void LIGHTLUX_IIC_Send_Byte(u8 txd)
{
  u8 t;
  LSDA_OUT();
  GPIO_ResetBits(GPIOA, GPIO_Pin_11); 	    	//����ʱ�ӿ�ʼ���ݴ���
  for(t = 0; t < 8; t++)
  {
    if((txd & 0x80) >> 7)
      GPIO_SetBits(GPIOA, GPIO_Pin_12);
    else
      GPIO_ResetBits(GPIOA, GPIO_Pin_12);
    txd <<= 1;
    Delay_us(20);
    GPIO_SetBits(GPIOA, GPIO_Pin_11);
    Delay_us(20);
    GPIO_ResetBits(GPIOA, GPIO_Pin_11);
    Delay_us(20);
  }
  Delay_us(20);

}

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
u16 LIGHTLUX_IIC_Read_Byte(u8 ack)
{
  u8 i;
  u16 receive = 0;
  LSDA_IN();
  for(i = 0; i < 8; i++ )
  {
    GPIO_ResetBits(GPIOA, GPIO_Pin_11);
    Delay_us(20);
    GPIO_SetBits(GPIOA, GPIO_Pin_11);
    receive <<= 1;
    if(LIGHTLUX_SDA_READ())
      receive++;
    Delay_us(20);
  }
  if (!ack)
    LIGHTLUX_IIC_NAck();//����nACK
  else
    LIGHTLUX_IIC_Ack(); //����ACK
  return receive;
}


//��ʼ��IIC�ӿ�
void LIGHTLUX_Init(void)
{
  LIGHTLUX_GPIO_Init();
  LIGHTLux_Mesu();
//	LIGHTLUX_ad_write(0x120,0x61);
//	LIGHTLUX_ad_write(0x01,0x00);
}


void LIGHTLUX_Write(u8 a, u8 b)
{
  LIGHTLUX_IIC_Start();
  LIGHTLUX_IIC_Send_Byte(LIGHTLUX_write); //����������ַ+дָ��
  LIGHTLUX_IIC_Wait_Ack();
  LIGHTLUX_IIC_Send_Byte(a);		//���Ϳ����ֽ�
  LIGHTLUX_IIC_Wait_Ack();
  LIGHTLUX_IIC_Send_Byte(b);
  LIGHTLUX_IIC_Wait_Ack();
  LIGHTLUX_IIC_Stop();
  Delay_ms(100);
}

void LIGHTLux_Mesu(void)
{
	LIGHTLUX_IIC_Start();
	LIGHTLUX_IIC_Send_Byte(LIGHTLUX_write);
	LIGHTLUX_IIC_Wait_Ack();
	LIGHTLUX_IIC_Send_Byte(0x10);
	LIGHTLUX_IIC_Wait_Ack();
	LIGHTLUX_IIC_Stop();
	Delay_ms(100);
}

void LIGHTLux_Read(char *x)
{
  
  LIGHTLUX_IIC_Start();
  LIGHTLUX_IIC_Send_Byte(LIGHTLUX_read); //����������ַ+��ָ��
  LIGHTLUX_IIC_Wait_Ack();
  x[0] = LIGHTLUX_IIC_Read_Byte(1);
  x[1] = LIGHTLUX_IIC_Read_Byte(0);
  LIGHTLUX_IIC_Stop();
}

