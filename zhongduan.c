#include "zhongduan.h"
#include "bsp_usart.h"
#include "bsp_SysTick.h"
#include <string.h>
#include "light.h"
#include "fan.h"
#include "led.h"
#include "beep.h"
#include "cJSON.h"
#include "i2c.h"
#include "FreeRTOS.h"
#include "task.h"
#define pubtopic   "status"
char ucTemp[256];
volatile u8 i;
char json[200];
cJSON *cjson;
char status[200];
char mode=1;
unsigned char TV[7]={20,100,7,5,80,60};
unsigned char Tplace;
unsigned int Lplace;
unsigned char HMaxplace,HMinplace;
unsigned char PMaxplace,PMinplace;
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0)!=RESET)
	{
		mode=~mode;
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}
//extern void xPortSysTickHandler(void);

void SysTick_Handler(void)
{
	TimingDelay_Decrement();
//	#if (INCLUDE_xTaskGetSchedulerState  == 1 )
//      if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
//      {
//    #endif  /* INCLUDE_xTaskGetSchedulerState */  
//        xPortSysTickHandler();
//    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
//      }
//    #endif  /* INCLUDE_xTaskGetSchedulerState */

}


void DEBUG_USART_IRQHandler(void)
{

	char res;
	if(USART_GetITStatus(DEBUG_USARTx,USART_IT_RXNE) != RESET){
		res = USART_ReceiveData(DEBUG_USARTx);		 
		printf("%c",res);

	  ucTemp[i] =res;
		if(ucTemp[0]=='+')i++;
		else i=0;
		if((i)&&(ucTemp[i-1]==0x0A))
		{
			if(strstr(ucTemp,"DeviceStatus")!=NULL)
			{//发状态
				sprintf(status,"AT+MQTTPUB=0,\"status\",\"{\\\"light\\\":\\\"%s\\\"\\\,\\\"beep\\\":\\\"%s\\\"\\\,\\\"led\\\":\\\"%s\\\"\\\,\\\"fan\\\":\\\"%s\\\"\\\,\\\"Control\\\":\\\"%s\\\"}\",0,1\r\n",Lightstatus(),Beepstatus(),Ledtstatus(),Fanstatus(),str());
				Usart_SendString(DEBUG_USARTx,status);
				memset(status,0,sizeof(status));
				printf("%s",Ledtstatus());
			}
			strncpy(json,ucTemp+25,(strlen(ucTemp)-25));
			printf("json=%s",json);
			if(json[0]=='{')
			{
				cjson=cJSON_Parse(json);
				char *led=cJSON_GetObjectItem(cjson,"led")->valuestring;
				char *beep=cJSON_GetObjectItem(cjson,"beep")->valuestring;
				char *fan=cJSON_GetObjectItem(cjson,"fan")->valuestring;
				char *light=cJSON_GetObjectItem(cjson,"light")->valuestring;
				char *control=cJSON_GetObjectItem(cjson,"control")->valuestring;
				Tplace=cJSON_GetObjectItem(cjson,"TempTV")->valueint;
				if(Tplace!=0&&Tplace!=83)
					TV[0]=Tplace;
				Lplace=cJSON_GetObjectItem(cjson,"LightTV")->valueint;
				if(Lplace!=0&&Lplace!=83)
					TV[1]=Lplace/256;
					TV[6]=Lplace%256;
				PMaxplace=cJSON_GetObjectItem(cjson,"PhMaxTV")->valueint;
				if(PMaxplace!=0&&PMaxplace!=83)
					TV[2]=PMaxplace;
				PMinplace=cJSON_GetObjectItem(cjson,"PhMinTV")->valueint;
				if(PMinplace!=0&&PMinplace!=83)
					TV[3]=PMinplace;
				HMaxplace=cJSON_GetObjectItem(cjson,"HumidityMaxTV")->valueint;
				if(HMaxplace!=0&&HMaxplace!=83)
					TV[4]=HMaxplace;
				HMinplace=cJSON_GetObjectItem(cjson,"HumidityMinTV")->valueint;
				if(HMinplace!=0&&HMinplace!=83)
					TV[5]=HMinplace;
				WriteStatus(TV);
				printf("%d,%d,%d,%d,%d,%d\r\n",Tplace,Lplace,HMaxplace,HMinplace,PMaxplace,PMinplace);
				printf("%s,%s,%s,%s,%s,%d,%d,%d,%d,%d,%d\r\n",led,beep,fan,light,control,TV[1]*256+TV[6],TV[0],TV[2],TV[3],TV[4],TV[5]);
				printf("解析成功\r\n");
				if(strcmp(beep,"on")==0)
				{
					BeepOn();
					printf("蜂鸣器打开\r\n");
				}
				if(strcmp(beep,"off")==0)
				{
					BeepOff();
					printf("蜂鸣器关闭\r\n");
				}
				if(strcmp(led,"blue")==0)
				{
					BlueOn();
					RedOff();
					GreenOff();
					printf("蓝灯打开\r\n");
				}
				if(strcmp(led,"red")==0)
				{
					RedOn();
					BlueOff();
					GreenOff();
					printf("红灯打开\r\n");
				}
				if(strcmp(led,"green")==0)
				{
					GreenOn();
					RedOff();
					BlueOff();
					printf("绿灯打开\r\n");
				}
				if(strcmp(led,"off")==0)
				{
					BlueOff();
					GreenOff();
					RedOff();
					printf("LED关闭\r\n");
				}
				if(strcmp(light,"on")==0)
				{
					LightOn();
					printf("生长灯打开\r\n");
				}
				if(strcmp(light,"off")==0)
				{
					LightOff();
					printf("生长灯关闭\r\n");
				}
				if(strcmp(fan,"on")==0)
				{
					FanOn();
					printf("风扇打开\r\n");
				}
				if(strcmp(fan,"off")==0)
				{
					FanOff();
					printf("风扇关闭\r\n");
				}
				if(strcmp(control,"manual")==0)
				{
					mode=1;
					printf("手动模式");
				}
				if(strcmp(control,"automatic")==0)
				{
					mode=0;
					printf("自动模式");
				}
				cJSON_Delete(cjson);
				cJSON_free(cjson);
			}
			else{cJSON_GetErrorPtr();}
			i=0;
			memset(ucTemp,0,sizeof(ucTemp));
			memset(json,0,sizeof(json));
		}
	}
		
//		i++;
//		if(i>256)
//			i=0;
//		}

}

char *str(void)
{
	if(mode==1)
		return "manual";
	else
		return "automatic";
}


