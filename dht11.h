#ifndef __DHT11_H
#define	__DHT11_H


#include "bsp_usart.h"
#include "stm32f10x.h"
#include "bsp_SysTick.h"

/************************** DHT11 �������Ͷ���********************************/
typedef struct
{
	char  humi_int;		//ʪ�ȵ���������
	char  humi_deci;	 	//ʪ�ȵ�С������
	char  temp_int;	 	//�¶ȵ���������
	char  temp_deci;	 	//�¶ȵ�С������
	char  check_sum;	 	//У���
		                 
} DHT11_Data_TypeDef;



/************************** DHT11 �������Ŷ���********************************/
#define      DHT11_Dout_SCK_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define      DHT11_Dout_GPIO_CLK                       RCC_APB2Periph_GPIOE

#define      DHT11_Dout_GPIO_PORT                      GPIOE
#define      DHT11_Dout_GPIO_PIN                       GPIO_Pin_6



/************************** DHT11 �����궨��********************************/
#define      DHT11_Dout_0	                            GPIO_ResetBits ( DHT11_Dout_GPIO_PORT, DHT11_Dout_GPIO_PIN ) 
#define      DHT11_Dout_1	                            GPIO_SetBits ( DHT11_Dout_GPIO_PORT, DHT11_Dout_GPIO_PIN ) 

#define      DHT11_Dout_IN()	                          GPIO_ReadInputDataBit ( DHT11_Dout_GPIO_PORT, DHT11_Dout_GPIO_PIN ) 



/************************** DHT11 �������� ********************************/
void                     DHT11_Init                      ( void );
uint8_t DHT11_Read_TempAndHumidity(char *DHT11_Data)

;

#endif /* __DHT11_H */







