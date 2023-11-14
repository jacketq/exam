#include "rc522.h"
#include <string.h>
#define MAXRLEN 18 
unsigned char Spi_Read_RC522_Register(unsigned char Address)
{
	unsigned char data,addr;
	addr=(Address<<1)&0x7E|0x80;
	GPIO_ResetBits(GPIOC, GPIO_Pin_0);
	Spi_SendByte(addr);
	data=Spi_SendByte(0xFF);
	GPIO_SetBits(GPIOC, GPIO_Pin_0);
	return data;
}

void Spi_Write_RC522_Register(unsigned char Address,unsigned char data)
{
	unsigned char addr;
	addr=(Address<<1)&0x7E;
	GPIO_ResetBits(GPIOC, GPIO_Pin_0);
	Spi_SendByte(addr);
	Spi_SendByte(data);
	GPIO_SetBits(GPIOC, GPIO_Pin_0);
}

void SetBit_Register(unsigned char Address,unsigned char mask)
{
	unsigned char temp;
	temp=Spi_Read_RC522_Register(Address);
	Spi_Write_RC522_Register( Address,temp|mask);
}

void Clear_Register(unsigned char Address,unsigned char mask)
{
	unsigned char temp;
	temp=Spi_Read_RC522_Register(Address);
	Spi_Write_RC522_Register( Address,temp&(~mask));
}



                       
/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          	  pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
   char status;  
   unsigned int  unLen;
   unsigned char ucComMF522Buf[MAXRLEN]; 
//  unsigned char xTest ;
   Clear_Register(Status2Reg,0x08);
   Spi_Write_RC522_Register(BitFramingReg,0x07);

//  xTest = Spi_Read_RC522_Register(BitFramingReg);
//  if(xTest == 0x07 )
 //   { LED_GREEN  =0 ;}
 // else {LED_GREEN =1 ;while(1){}}
   SetBit_Register(TxControlReg,0x03);
 
   ucComMF522Buf[0] = req_code;

   status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
//     if(status  == MI_OK )
//   { LED_GREEN  =0 ;}
//   else {LED_GREEN =1 ;}
   if ((status == MI_OK) && (unLen == 0x10))
   {    
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
   }
   else
   {   
			status = MI_ERR; 
	 }
   
   return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////  
char PcdAnticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    

    Clear_Register(Status2Reg,0x08);
    Spi_Write_RC522_Register(BitFramingReg,0x00);
    Clear_Register(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBit_Register(CollReg,0x80);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr)
{
    char status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    Clear_Register(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥 
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////               
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+2] = *(pKey+i);   }
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
 //   memcpy(&ucComMF522Buf[2], pKey, 6); 
 //   memcpy(&ucComMF522Buf[8], pSnr, 4); 
    
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(Spi_Read_RC522_Register(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdRead(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
 //   {   memcpy(pData, ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          pData[IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                  
char PcdWrite(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   
			status = MI_ERR; 
		}
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, pData, 16);

        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pData+i);   }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//               0xC0 = 扣款
//               0xC1 = 充值
//          addr[IN]：钱包地址
//          pValue[IN]：4字节增(减)值，低位在前
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////    
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
       // memcpy(ucComMF522Buf, pValue, 4);
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
   
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    Clear_Register(DivIrqReg,0x04);
    Spi_Write_RC522_Register(CommandReg,PCD_IDLE);
    SetBit_Register(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   Spi_Write_RC522_Register(FIFODataReg, *(pIndata+i));   }
    Spi_Write_RC522_Register(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = Spi_Read_RC522_Register(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = Spi_Read_RC522_Register(CRCResultRegL);
    pOutData[1] = Spi_Read_RC522_Register(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//功    能：复位RC522
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{
		GPIO_SetBits(GPIOB,GPIO_Pin_0);
    delay_10ms(1);
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);
    delay_10ms(1);
    GPIO_SetBits(GPIOB,GPIO_Pin_0);
	  delay_10ms(10);
    Spi_Write_RC522_Register(CommandReg,PCD_RESETPHASE);
    while(Spi_Read_RC522_Register(CommandReg)&0x10);
		delay_10ms(1);
    Spi_Write_RC522_Register(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
    Spi_Write_RC522_Register(TReloadRegL,30);           
    Spi_Write_RC522_Register(TReloadRegH,0);
    Spi_Write_RC522_Register(TModeReg,0x8D);
    Spi_Write_RC522_Register(TPrescalerReg,0x3E);
    Spi_Write_RC522_Register(TxAutoReg,0x40);     
    return MI_OK;
}
//////////////////////////////////////////////////////////////////////
//设置RC632的工作方式 
//////////////////////////////////////////////////////////////////////
char M500PcdConfigISOType(unsigned char type)
{
   if (type == 'A')                     //ISO14443_A
   { 
       Clear_Register(Status2Reg,0x08);

 /*     Spi_Write_RC522_Register(CommandReg,0x20);    //as default   
       Spi_Write_RC522_Register(ComIEnReg,0x80);     //as default
       Spi_Write_RC522_Register(DivlEnReg,0x0);      //as default
	   Spi_Write_RC522_Register(ComIrqReg,0x04);     //as default
	   Spi_Write_RC522_Register(DivIrqReg,0x0);      //as default
	   Spi_Write_RC522_Register(Status2Reg,0x0);//80    //trun off temperature sensor
	   Spi_Write_RC522_Register(WaterLevelReg,0x08); //as default
       Spi_Write_RC522_Register(ControlReg,0x20);    //as default
	   Spi_Write_RC522_Register(CollReg,0x80);    //as default
*/
       Spi_Write_RC522_Register(ModeReg,0x3D);//3F
/*	   Spi_Write_RC522_Register(TxModeReg,0x0);      //as default???
	   Spi_Write_RC522_Register(RxModeReg,0x0);      //as default???
	   Spi_Write_RC522_Register(TxControlReg,0x80);  //as default???

	   Spi_Write_RC522_Register(TxSelReg,0x10);      //as default???
   */
       Spi_Write_RC522_Register(RxSelReg,0x86);//84
 //      Spi_Write_RC522_Register(RxThresholdReg,0x84);//as default
 //      Spi_Write_RC522_Register(DemodReg,0x4D);      //as default

 //      Spi_Write_RC522_Register(ModWidthReg,0x13);//26
       Spi_Write_RC522_Register(RFCfgReg,0x7F);   //4F
	/*   Spi_Write_RC522_Register(GsNReg,0x88);        //as default???
	   Spi_Write_RC522_Register(CWGsCfgReg,0x20);    //as default???
       Spi_Write_RC522_Register(ModGsCfgReg,0x20);   //as default???
*/
   	   Spi_Write_RC522_Register(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
	   Spi_Write_RC522_Register(TReloadRegH,0);
       Spi_Write_RC522_Register(TModeReg,0x8D);
	   Spi_Write_RC522_Register(TPrescalerReg,0x3E);
	   

  //     PcdSetTmo(106);
	   delay_10ms(1);
       PcdAntennaOn();
   }
   else{ return (char)-1; }
   
   return MI_OK;
}


/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
char PcdComMF522(unsigned char Command, 
                 unsigned char *pInData, 
                 unsigned char InLenByte,
                 unsigned char *pOutData, 
                 unsigned int  *pOutLenBit)
{
    char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    switch (Command)
    {
       case PCD_AUTHENT:
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:
          irqEn   = 0x77;
          waitFor = 0x30;
          break;
       default:
         break;
    }
   
    Spi_Write_RC522_Register(ComIEnReg,irqEn|0x80);
    Clear_Register(ComIrqReg,0x80);
    Spi_Write_RC522_Register(CommandReg,PCD_IDLE);
    SetBit_Register(FIFOLevelReg,0x80);
    
    for (i=0; i<InLenByte; i++)
    {   Spi_Write_RC522_Register(FIFODataReg, pInData[i]);    }
    Spi_Write_RC522_Register(CommandReg, Command);
   
    
    if (Command == PCD_TRANSCEIVE)
    {    SetBit_Register(BitFramingReg,0x80);  }
    
//    i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
 i = 2000;
    do 
    {
         n = Spi_Read_RC522_Register(ComIrqReg);
         i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    Clear_Register(BitFramingReg,0x80);
	      
    if (i!=0)
    {    
         if(!(Spi_Read_RC522_Register(ErrorReg)&0x1B))
         {
             status = MI_OK;
             if (n & irqEn & 0x01)
             {   status = MI_NOTAGERR;   }
             if (Command == PCD_TRANSCEIVE)
             {
               	n = Spi_Read_RC522_Register(FIFOLevelReg);
              	lastBits = Spi_Read_RC522_Register(ControlReg) & 0x07;
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOutData[i] = Spi_Read_RC522_Register(FIFODataReg);    }
            }
         }
         else
         {   status = MI_ERR;   }
        
   }
   

   SetBit_Register(ControlReg,0x80);           // stop timer now
   Spi_Write_RC522_Register(CommandReg,PCD_IDLE); 
   return status;
}


/////////////////////////////////////////////////////////////////////
//开启天线  
//每次启动或关闭天险发射之间应至少有1ms的间隔
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn()
{
    unsigned char i;
    i = Spi_Read_RC522_Register(TxControlReg);
    if (!(i & 0x03))
    {
        SetBit_Register(TxControlReg, 0x03);
    }
}


/////////////////////////////////////////////////////////////////////
//关闭天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff()
{
    Clear_Register(TxControlReg, 0x03);
}

//等待卡离开


///////////////////////////////////////////////////////////////////////
// Delay 10ms
///////////////////////////////////////////////////////////////////////
void delay_10ms(unsigned int _10ms)
{
	unsigned int i, j;

	for(i=0; i<_10ms; i++)
	{
		for(j=0; j<60000; j++);
	}
}
