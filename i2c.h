#ifndef _I2C_H
#define _I2C_H
#include "stm32f10x.h"
#define  DEBUG_I2Cx                    I2C1
#define  DEBUG_I2Cx_CLK                RCC_APB1Periph_I2C1
#define  DEBUG_I2Cx_APBxClkCmd         RCC_APB1PeriphClockCmd
#define  DEBUG_I2Cx_BAUDRATE           400000

//// USART GPIO Òý½Åºê¶¨Òå
#define  DEBUG_I2Cx_GPIO_CLK           RCC_APB2Periph_GPIOB
#define  DEBUG_I2Cx_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
//    
#define  DEBUG_I2Cx_SCL_GPIO_PORT         GPIOB   
#define  DEBUG_I2Cx_SCL_GPIO_PIN          GPIO_Pin_6
#define  DEBUG_I2Cx_SDA_GPIO_PORT       	GPIOB
#define  DEBUG_I2Cx_SDA_GPIO_PIN        	GPIO_Pin_7

#define  I2C_ADRESS										 0x70
#define  EEPROM												 0xA0

void I2c_Config(void);
//void Start_Mesu(void);
//void ReceiveData(char *a);
void WriteStatus(unsigned char *x);
void ReadStatus(unsigned char *x);
#endif

