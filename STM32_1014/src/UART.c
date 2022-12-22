#include "UART.h"
#include "main.h"
#include "LED.h"
#include "flash.h"
#include "Structure_Data.h"
#include "string.h"

extern SystemVariables 				xSystem;
extern MC60Structure				  GSM;
extern GPS_Structure					GPS;
extern ConfStructure        	Config;  
extern CamStructure							Camera;
extern CamDataStruct						Cam_Data;

//volatile static uint8_t xAddress=0x00000012;
void UART1_Init(uint32_t Baud)
{
	USART_InitTypeDef 				UartInitStructure;
	GPIO_InitTypeDef 					GPIOInitStructure;
	NVIC_InitTypeDef           NVIC_InitStructure;
	
	USART_DeInit(USART1);
	USART_Cmd(USART1, DISABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA ,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	/*Pin9_TX*/
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_AF ;
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIOInitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIOInitStructure);
	/*Pin10_RX*/
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_AF ;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIOInitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIOInitStructure);
	//Configure Interrupt
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/**/
		UartInitStructure.USART_BaudRate=Baud;
	UartInitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	UartInitStructure.USART_Mode= USART_Mode_Rx | USART_Mode_Tx ;
	UartInitStructure.USART_Parity=USART_Parity_No  ;
	UartInitStructure.USART_StopBits=USART_StopBits_1  ;
	UartInitStructure.USART_WordLength=USART_WordLength_8b  ;
	USART_Init (USART1,&UartInitStructure);
	//
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_EnableIRQ(USART1_IRQn);
	USART_Cmd(USART1,ENABLE);
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
}
void USART_ReceiData1(uint16_t Data)
{
	Config.RxBuffer[Config.RxCounter]=Data;
	
		if(Config.RxBuffer[Config.RxCounter]=='\r')
		{
			while(Config.RxCounter<=USART1_RX_DATA)
		{
			Config.RxBuffer[++Config.RxCounter]=NULL;
		}
		Config.RxCounter=0;
		return;
	}
	Config.RxCounter=(Config.RxCounter+1)%USART1_RX_DATA;
	//xSystem.Rxbuffer[xSystem.RxCounter]=0;
}

void UART4_ReceiData(char Data)
{
	/* Dua du lieu vao buffer */
	Camera.RxBuffer[Camera.RxCounter]=Data;
//	if(Camera.RxBuffer[Camera.RxCounter]=='\r')
//	{
////		while(Config.RxCounter<=CONFIG_BUFFER_CAMERA_LEN)
////		{
////			Config.RxBuffer[++Config.RxCounter]=NULL;
////		}
//		Camera.RxCounter=0;
//		return;
//	}
	Camera.RxCounter=(Camera.RxCounter+1)%CAM_RX_DATA;
}
void MC60_GSM_ReceiData(char Data)   /// Nhan nhieu cac ban tin tu GSM
{
	/* Dua du lieu vao buffer */
	GSM.RxBuffer[GSM.RxCounter]=Data;
	GSM.RxCounter=(GSM.RxCounter+1)%MC60_RX_BUFFER;
	GSM.RxBuffer[GSM.RxCounter] = 0;
	GSM.TimeOutReceive = 30;
}
void MC60_GSM_ReceiData1(char Data) /// Nhan cac ban tin tu GSM
{
	/* Dua du lieu vao buffer */
	GSM.RxBuffer[GSM.RxCounter]=Data;
	if(GSM.RxBuffer[GSM.RxCounter]=='\r')
	{
		while(GSM.RxCounter<=GSM_RXBUFFER)
		{
			GSM.RxBuffer[++GSM.RxCounter]=NULL;
		}
		GSM.RxCounter=0;
		return;
	}
	GSM.RxCounter=(GSM.RxCounter+1)%MC60_RX_BUFFER;
	GSM.TimeOutReceive = 30;
}

void MC60_GPS_ReceiData(char Data)
{
	/* Dua du lieu vao buffer */
	GPS.RxBuffer[GPS.RxCounter]=Data;
	if(GPS.RxBuffer[GPS.RxCounter]=='\r')
	{
		while(GPS.RxCounter<=GPS_RXBUFFER)
		{
			GPS.RxBuffer[++GPS.RxCounter]=NULL;
		}
		GPS.RxCounter=0;
		return;
	}
	GPS.RxCounter=(GPS.RxCounter+1)%GPS_RXBUFFER;

}
/*******************************************************************************
 * Function Name  	: Ring_Buffer_Write
 * Return         	: None
 * Parameters 		: None
 * Description		: Khoi tao buffer nhan du lieu
*******************************************************************************/
void Ring_Buffer_Write(Buffer_TypeDef *Buffer,uint8_t Data)
{
	Buffer->Memory[Buffer->Write++]=Data;
	if(Buffer->Write>=USART1_RX_DATA)
		Buffer->Write=0;
}
/*******************************************************************************
 * Function Name  	:Ring_Buffer_Read
 * Return         	: None
 * Parameters 		: None
 * Description		: Khoi tao buffer nhan du lieu
*******************************************************************************/
uint8_t Ring_Buffer_Read(Buffer_TypeDef *Buffer)
{
	uint8_t Data;
	if(Buffer->Read==Buffer->Write) 
		return 0;
	Data=Buffer->Memory[Buffer->Read++];
	if(Buffer->Read>=USART1_RX_DATA)
		Buffer->Read=0;
	return Data;
}

/*******************************************************************************
 * Function Name  	:CAM_SendByte(char Byte)
 * Return         	: None
 * Parameters 		: None
 * Description		: Gui 1 ki tu sang Cam qua UART4
*******************************************************************************/
void CAM_SendByte(char Byte)
{
	while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == 0)
	{}
	USART_SendData(UART4, (uint16_t)Byte);
}


void CAM_SendByte1(uint16_t Byte)
{
	while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == 0)
	{}
	USART_SendData(UART4,Byte);
}
/*******************************************************************************
 * Function Name  	:CAM_SendBytes
 * Return         	: None
 * Parameters 		: None
 * Description		: Gui 1 chuoi ki tu sang UART4
*******************************************************************************/
void Sendstring4(const char *s, uint8_t Len)
{
	//char c;
	while(Len--)
	{	//c=*s++;
		CAM_SendByte(*s++);	
	}
}
void Sendstring4s(uint8_t *s, uint8_t Len)
{
	//char c;
	while(Len--)
	{	//c=*s++;
		CAM_SendByte(*s++);	
	}
}

/*******************************************************************************
 * Function Name  	: Sendbyte
 * Return         	: None
 * Parameters 		: None
 * Description		: Ham gui du lieu 1 byte
*******************************************************************************/
void Sendbyte(char byte)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==0){}
	USART_SendData(USART1,(uint16_t)byte);		
}
/*******************************************************************************
 * Function Name  	: Sendbyte
 * Return         	: None
 * Parameters 		: None
 * Description		: Ham gui du lieu 1 byte
*******************************************************************************/
void Sendbyte1(uint16_t ch)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==0){}
	USART_SendData(USART1,ch);
}
/*******************************************************************************
 * Function Name  	:  Sendstring
 * Return         	: None
 * Parameters 		: None
 * Description		: Ham gui chuoi du lieu
*******************************************************************************/

void Sendstring(const char *s)
{
	char c;
	while ((c=*s++) != '\0' )// \0 la ki tu null (rong)
		Sendbyte(c);	
}
void Send(char * Data)
{
	uint16_t c;
	int i=0;
	while(((c=Data[i])!='\0')||i==20)
	{
		Sendbyte1(c);
		i++;
	}
}
void Send1(uint8_t Data[255],uint16_t Len)
{
	int i=0;
	for(i=0;i<Len;i++)
	{
		char Buf[20];
		sprintf(Buf,"%d\t",Data[i]);
		Sendstring(Buf);	
	}
	Sendstring("\n\r");
}
void Send2(uint16_t Data[255],uint16_t Len)
{
	int i=0;
	for(i=0;i<Len;i++)
	{
		char Buf[20];
		sprintf(Buf,"%d\t",Data[i]);
		Sendstring(Buf);	
	}
	Sendstring("\n\r");
}
void Sendstring1(char *s,uint16_t Len)
{	
	char c;
	while (Len--)
	{
		c=*s++;
		if(c==(char )0xFF) return;
		Sendbyte(c);
	}
}
