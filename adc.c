#include "adc.h"

uint16_t Adc_Value[5];

/**
  * @brief  ADC GPIO ��ʼ��
  * @param  ��
  * @retval ��
  */
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// �� ADC IO�˿�ʱ��
	RCC_APB2PeriphClockCmd ( RCC_ADC_GPIO, ENABLE );
	
	// ���� ADC IO ����ģʽ
	// ����Ϊģ������
	GPIO_InitStructure.GPIO_Pin = GPIO_ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	// ��ʼ�� ADC IO
	GPIO_Init(GPIO_ADC_PORT, &GPIO_InitStructure);				
}

void Adc_Dma_Config(void)
{
	DMA_InitTypeDef DMA_Init_Structe;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel1);
	DMA_Init_Structe.DMA_PeripheralBaseAddr=(uint32_t)(&(ADC1->DR));
	DMA_Init_Structe.DMA_MemoryBaseAddr=(uint32_t)(Adc_Value);
	DMA_Init_Structe.DMA_DIR=DMA_DIR_PeripheralSRC;
	DMA_Init_Structe.DMA_BufferSize=5;
	DMA_Init_Structe.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_Init_Structe.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_Init_Structe.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
	DMA_Init_Structe.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
	DMA_Init_Structe.DMA_Mode=DMA_Mode_Circular;
	DMA_Init_Structe.DMA_Priority=DMA_Priority_VeryHigh ;
	DMA_Init_Structe.DMA_M2M=DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_Init_Structe);
	DMA_Cmd(DMA1_Channel1, ENABLE);
}
/**
  * @brief  ����ADC����ģʽ
  * @param  ��
  * @retval ��
  */
static void ADCx_Mode_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;

	// ��ADCʱ��
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_ADC1, ENABLE );
	
	// ADC ģʽ����
	// ֻʹ��һ��ADC�����ڶ���ģʽ
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	
	// ��ֹɨ��ģʽ����ͨ����Ҫ����ͨ������Ҫ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 

	// ����ת��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;

	// �����ⲿ����ת���������������
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;

	// ת������Ҷ���
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	// ת��ͨ��1��
	ADC_InitStructure.ADC_NbrOfChannel = 5;	
		
	// ��ʼ��ADC
	ADC_Init(ADC1, &ADC_InitStructure);
	
	// ����ADCʱ��ΪPCLK2��8��Ƶ����9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	
	// ���� ADC ͨ��ת��˳��Ͳ���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_11 , 1, 
	                         ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_12 , 2, 
	                         ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_13 , 3, 
	                         ADC_SampleTime_55Cycles5);	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_14 , 4, 
	                         ADC_SampleTime_55Cycles5);	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_15 , 5, 
	                         ADC_SampleTime_55Cycles5);
	// ADC ת�����������жϣ����жϷ�������ж�ȡת��ֵ
	ADC_DMACmd(ADC1, ENABLE);
	// ����ADC ������ʼת��
	ADC_Cmd(ADC1, ENABLE);
	
	// ��ʼ��ADC У׼�Ĵ���  
	ADC_ResetCalibration(ADC1);
	// �ȴ�У׼�Ĵ�����ʼ�����
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	// ADC��ʼУ׼
	ADC_StartCalibration(ADC1);
	// �ȴ�У׼���
	while(ADC_GetCalibrationStatus(ADC1));
	
	// ����û�в����ⲿ����������ʹ���������ADCת�� 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}




/**
  * @brief  ADC��ʼ��
  * @param  ��
  * @retval ��
  */
void Adcc_Init(void)
{
	ADCx_GPIO_Config();
	Adc_Dma_Config();
	ADCx_Mode_Config();
}
/*********************************************END OF FILE**********************/
