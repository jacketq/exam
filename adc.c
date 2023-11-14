#include "adc.h"

uint16_t Adc_Value[5];

/**
  * @brief  ADC GPIO 初始化
  * @param  无
  * @retval 无
  */
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 打开 ADC IO端口时钟
	RCC_APB2PeriphClockCmd ( RCC_ADC_GPIO, ENABLE );
	
	// 配置 ADC IO 引脚模式
	// 必须为模拟输入
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
	
	// 初始化 ADC IO
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
  * @brief  配置ADC工作模式
  * @param  无
  * @retval 无
  */
static void ADCx_Mode_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;

	// 打开ADC时钟
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_ADC1, ENABLE );
	
	// ADC 模式配置
	// 只使用一个ADC，属于独立模式
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	
	// 禁止扫描模式，多通道才要，单通道不需要
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 

	// 连续转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;

	// 不用外部触发转换，软件开启即可
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;

	// 转换结果右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	// 转换通道1个
	ADC_InitStructure.ADC_NbrOfChannel = 5;	
		
	// 初始化ADC
	ADC_Init(ADC1, &ADC_InitStructure);
	
	// 配置ADC时钟为PCLK2的8分频，即9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	
	// 配置 ADC 通道转换顺序和采样时间
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
	// ADC 转换结束产生中断，在中断服务程序中读取转换值
	ADC_DMACmd(ADC1, ENABLE);
	// 开启ADC ，并开始转换
	ADC_Cmd(ADC1, ENABLE);
	
	// 初始化ADC 校准寄存器  
	ADC_ResetCalibration(ADC1);
	// 等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	// ADC开始校准
	ADC_StartCalibration(ADC1);
	// 等待校准完成
	while(ADC_GetCalibrationStatus(ADC1));
	
	// 由于没有采用外部触发，所以使用软件触发ADC转换 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}




/**
  * @brief  ADC初始化
  * @param  无
  * @retval 无
  */
void Adcc_Init(void)
{
	ADCx_GPIO_Config();
	Adc_Dma_Config();
	ADCx_Mode_Config();
}
/*********************************************END OF FILE**********************/
