#ifndef _SPI_H
#define _SPI_H
#include "stm32f10x.h"
void Spi_Gpio_Config(void);
void Spi_Config(void);
void Spi_Init(void);
char Spi_SendByte(char data);
char Spi_ReceiveByte(void);
unsigned int SPI_FLASH_ReadID(void);
#endif

