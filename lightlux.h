#ifndef __LIGHTLUX_H
#define __LIGHTLUX_H
#include "stm32f10x.h"
#include "bsp_SysTick.h"
#define  LIGHTLUX_SCL   PBout(0)
#define  LIGHTLUX_SDA   PBout(1)


// USART GPIO ���ź궨��
#define  LIGHTLUX_SCL_GPIO_CLK        RCC_APB2Periph_GPIOA
#define  LIGHTLUX_SCL_GPIO_PORT       GPIOA
#define  LIGHTLUX_SCL_GPIO_PIN        GPIO_Pin_11

#define  LIGHTLUX_SDA_GPIO_SDA        RCC_APB2Periph_GPIOA
#define  LIGHTLUX_SDA_GPIO_PORT       GPIOA
#define  LIGHTLUX_SDA_GPIO_PIN        GPIO_Pin_12

#define  LIGHTLUX_SDA_READ()           GPIO_ReadInputDataBit(LIGHTLUX_SDA_GPIO_PORT, LIGHTLUX_SDA_GPIO_PIN)

#define LIGHTLUX_read  0x47  //LIGHTLUX�Ķ���ַ
#define LIGHTLUX_write 0x46  //LIGHTLUX��д��ַ

void LIGHTLUX_GPIO_Init(void);
void LIGHTLUX_IIC_Start(void);				//����IIC��ʼ�ź�
void LIGHTLUX_IIC_Stop(void);	  			//����IICֹͣ�ź�
void LIGHTLUX_IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u16 LIGHTLUX_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 LIGHTLUX_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void LIGHTLUX_IIC_Ack(void);					//IIC����ACK�ź�
void LIGHTLUX_IIC_NAck(void);				//IIC������ACK�ź�
void LIGHTLUX_IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 LIGHTLUX_IIC_Read_One_Byte(u8 daddr,u8 addr);	
void LIGHTLUX_Init(void);				  
void LIGHTLUX_Write(u8 a, u8 b);
void LIGHTLux_Mesu(void);
void LIGHTLux_Read(char *x);
#endif

