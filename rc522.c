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
//��    �ܣ�Ѱ��
//����˵��: req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//          	  pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ�����ײ
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ���֤��Ƭ����
//����˵��: auth_mode[IN]: ������֤ģʽ
//                 0x60 = ��֤A��Կ
//                 0x61 = ��֤B��Կ 
//          addr[IN]�����ַ
//          pKey[IN]������
//          pSnr[IN]����Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ���ȡM1��һ������
//����˵��: addr[IN]�����ַ
//          pData[OUT]�����������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ�д���ݵ�M1��һ��
//����˵��: addr[IN]�����ַ
//          pData[IN]��д������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ��ۿ�ͳ�ֵ
//����˵��: dd_mode[IN]��������
//               0xC0 = �ۿ�
//               0xC1 = ��ֵ
//          addr[IN]��Ǯ����ַ
//          pValue[IN]��4�ֽ���(��)ֵ����λ��ǰ
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ����Ƭ��������״̬
//��    ��: �ɹ�����MI_OK
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
//��MF522����CRC16����
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
//��    �ܣ���λRC522
//��    ��: �ɹ�����MI_OK
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
    Spi_Write_RC522_Register(ModeReg,0x3D);            //��Mifare��ͨѶ��CRC��ʼֵ0x6363
    Spi_Write_RC522_Register(TReloadRegL,30);           
    Spi_Write_RC522_Register(TReloadRegH,0);
    Spi_Write_RC522_Register(TModeReg,0x8D);
    Spi_Write_RC522_Register(TPrescalerReg,0x3E);
    Spi_Write_RC522_Register(TxAutoReg,0x40);     
    return MI_OK;
}
//////////////////////////////////////////////////////////////////////
//����RC632�Ĺ�����ʽ 
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
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pInData[IN]:ͨ��RC522���͵���Ƭ������
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOutData[OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
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
    
//    i = 600;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
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
//��������  
//ÿ��������ر����շ���֮��Ӧ������1ms�ļ��
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
//�ر�����
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff()
{
    Clear_Register(TxControlReg, 0x03);
}

//�ȴ����뿪


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
