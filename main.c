/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   dht11温湿度传感器测试实验
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "zhongduan.h"
#include "bsp_SysTick.h"
#include "dht11.h"
#include "bsp_usart.h"
#include <string.h>
#include <stdio.h>
#include "led.h"
#include "fan.h"
#include "beep.h"
#include "light.h"
#include "i2c.h"
#include "adc.h"
#include "pwm.h"
#include "sgp30.h"
#include "lightlux.h"
#include "exti.h"
unsigned int q;//转换的光照
char a[2];//光照
char buf[200];
char x[5];//温湿度
float Adc_V[5];//Adc_V[0]为PH电压
extern  uint16_t Adc_Value[5];
float PH;//p2
u32 CO2Data,TVOCData;
u32 sgp30_dat;
extern char mode;
extern unsigned char TV[7];
extern char status[200];
void fill(char* buf)
{
	LIGHTLux_Read(a);
	q=((a[0]*256)+a[1])/1.2;
	Adc_V[0]=(float)Adc_Value[0]/4096*3.3;
	PH = -5.7541*Adc_V[0]+16.654;
	SGP30_Write(0x20,0x08);
	sgp30_dat = SGP30_Read();//读取SGP30的值
	CO2Data = (sgp30_dat & 0xffff0000) >> 16;//取出CO2浓度值
	TVOCData = sgp30_dat & 0x0000ffff;       //取出TVOC值
	//sprintf(buf,"guang:%d\r\n",q);
	if( DHT11_Read_TempAndHumidity ( x ) == SUCCESS){
		sprintf(buf,"AT+MQTTPUB=0,\"soilless\",\"{\\\"Humidity\\\":%d.%d\\\,\\\"Temperature\\\":%d.%d\\\,\\\"LightLux\\\":%d\\\,\\\"PH\\\":%.2f\\\,\\\"CO2\\\":%d}\",0,0\r\n",x[0],x[1],x[2],x[3],q,PH,CO2Data);			
	}
}


void Send()
{
	memset(buf,0,sizeof(buf));
	fill(buf);
	Usart_SendString( DEBUG_USARTx , buf);
	//Usart_SendString( USART1 , buf);
}

void autoSend()
{
	memset(buf,0,sizeof(buf));
	fill(buf);
	if((x[2]+x[3]/10)>=TV[0])
	{
		FanOn();
	}
	else{FanOff();}
	if(q<(TV[1]*256+TV[6]))
	{
		LightOn();
	}else{LightOff();}
	if(PH>=TV[3]&&PH<=TV[2])
	{
		BeepOff();
	}else{BeepOn();}
	if(x[0]>=TV[5]&&x[0]<=TV[4])
	{
		RedOff();
	}else{RedOn();}
	Usart_SendString( DEBUG_USARTx , buf);
	//Usart_SendString( USART1 , buf);
}



/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
void Server_360_Forward(void)
{
	TIM_SetCompare4(TIM3,5);//0.5ms --> 正向最大转速
	Delay_ms(500);
	
	//TIM_SetCompare4(TIM3,15);//1.5ms --> 速度为0
	//Delay_ms(3000);
	
	//TIM_SetCompare4(TIM3,25);//2.5ms --> 反向最大转速
	//Delay_ms(1000);
	
	//TIM_SetCompare4(TIM3,15);//1.5ms --> 速度为0
	//Delay_ms(3000);
}

void DevicesStatus()
{//发状态
	sprintf(status,"AT+MQTTPUB=0,\"status\",\"{\\\"light\\\":\\\"%s\\\"\\\,\\\"beep\\\":\\\"%s\\\"\\\,\\\"led\\\":\\\"%s\\\"\\\,\\\"fan\\\":\\\"%s\\\"\\\,\\\"Control\\\":\\\"%s\\\"}\",0,1\r\n"
	,Lightstatus(),Beepstatus(),Ledtstatus(),Fanstatus(),str());
	Usart_SendString(DEBUG_USARTx,status);
	memset(status,0,sizeof(status));
	Delay_ms(500);//发阈值
	sprintf(status,"AT+MQTTPUB=0,\"Tvalue\",\"{\\\"TempTV\\\":%d\\\,\\\"LightTV\\\":%d\\\,\\\"HumidityMaxTV\\\":%d\\\,\\\"HumidityMinTV\\\":%d\\\,\\\"PhMaxTV\\\":%d\\\,\\\"PhMinTV\\\":%d}\",0,1\r\n"
	,TV[0],TV[1]*256+TV[6],TV[4],TV[5],TV[2],TV[3]);
	Usart_SendString(DEBUG_USARTx,status);
	memset(status,0,sizeof(status));
}

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	SysTick_Init();
	Ledconfig();
	ExtiConfig();
	BeepInit();
	//Pwm_Init();
	Fengconfig();
	Lightconfig();
	I2c_Config();
	DHT11_Init ();
	LIGHTLUX_Init();
	Delay_ms(3000);
	Adcc_Init();
	/* 初始化系统定时器 */
	USART1_Config();//初始化串口1
	USART_Config();
	//printf("\r\n***野火STM32 dht11 温湿度传感器实验***\r\n");
	/*初始化DTT11的引脚*/
	Aliyun_Connect();
	ReadStatus(TV);
	DevicesStatus();
	Delay_ms(150);
	printf("LIGHT=%d",TV[1]*256+TV[6]);
	
	
	SGP30_Init();   //初始化SGP30
	Delay_ms(100);
	SGP30_Write(0x20,0x08);
	sgp30_dat = SGP30_Read();//读取SGP30的值
	CO2Data = (sgp30_dat & 0xffff0000) >> 16;
	TVOCData = sgp30_dat & 0x0000ffff;	
	//SGP30模块开机需要一定时间初始化，在初始化阶段读取的CO2浓度为400ppm，TVOC为0ppd且恒定不变，因此上电后每隔一段时间读取一次
	//SGP30模块的值，如果CO2浓度为400ppm，TVOC为0ppd，发送“正在检测中...”，直到SGP30模块初始化完成。
	//初始化完成后刚开始读出数据会波动比较大，属于正常现象，一段时间后会逐渐趋于稳定。
	//气体类传感器比较容易受环境影响，测量数据出现波动是正常的，可自行添加滤波函数。
	while(CO2Data == 400 && TVOCData == 0)
	{
		SGP30_Write(0x20,0x08);
		sgp30_dat = SGP30_Read();//读取SGP30的值
		CO2Data = (sgp30_dat & 0xffff0000) >> 16;//取出CO2浓度值
		TVOCData = sgp30_dat & 0x0000ffff;			 //取出TVOC值
		printf("正在检测中...\r\n");
		Delay_ms(500);
	}
	while(1)
	{
		if(mode==1)
		{
			Send();
			Delay_ms(5000);
		}
		else
		{	
			autoSend();
			Delay_ms(5000);
		}
	}
	
}	

