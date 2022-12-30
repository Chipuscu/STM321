#include "main.h"           
#include "Camera.h"
#include "STM32F2xx.h"                  // Device header
#include "flash.h"
#include "UART.h"
#include "string.h"
#include "debug.h"

extern CamStructure					Camera;
extern CamDataStruct					Cam_Data;
extern SystemVariables        				xSystem;

/*******************************************************************************
 * Function Name  	:UART1_Init
 * Return         	: None
 * Parameters 		: None
 * Description		: Init
*******************************************************************************/
void UART4_Init(uint32_t Baud)
{
	
	USART_InitTypeDef 				UartInitStructure;
	GPIO_InitTypeDef 				GPIOInitStructure;
	NVIC_InitTypeDef          		NVIC_InitStructure;
	
	USART_DeInit(UART4);
	USART_Cmd(UART4, DISABLE);
	USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC ,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);

	/*Pin10_TX*/
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_AF ;
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIOInitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIOInitStructure);
	/*Pin11_RX*/
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_AF ;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIOInitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIOInitStructure);
	//Configure Interrupt
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
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
	USART_Init (UART4,&UartInitStructure);
	//
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	
	NVIC_EnableIRQ(UART4_IRQn);
	USART_Cmd(UART4,ENABLE);
}
/*******************************************************************************
 * Function Name  	:UART1_Init
 * Return         	: None
 * Parameters 		: None
 * Description		: Init
*******************************************************************************/
void CAM_process(void)
{
	//Process data to struct
	Process_data_camera((uint8_t *)Camera.RxBuffer,(CamDataStruct *)&Cam_Data);	

}
/**************************************************************************************
* Function Name  	: 
* Return         	: None
* Parameters 		: None
* Description		: Process data analist
*************************************************************************************/
uint8_t 	Process_data_camera(uint8_t* Buffer,CamDataStruct *Data)
{
	char buf[50],Number[1];
	uint16_t Size_Pakage=CAM_PACKAGE_SIZE_4;
	uint16_t Pos=0,i=0;

	if(Buffer[Pos]!=CAM_HEADER_PACKAGE  && Buffer[Pos+3]!=CAM_FINISH_PACKAGE)
		return FAIL;
	if(Buffer[Pos+1]==CAM_COMMAND_SNAPSHOT&&Buffer[Pos+5]==CAM_COMMAND_BYTE_R)
	{
		Data->ID_Cam=Buffer[Pos+2];
		Data->NumberofPackage=Buffer[Pos+11];
		sprintf(Number,"%c",Data->NumberofPackage);
		FLASH_WriteBuffer((uint8_t *)Number,(300+Cam_Data.Name*10)*4*1024+Cam_Data.Name-1,1);
	}
	if(Buffer[Pos+1]==CAM_COMMAND_GET_DATA&& Buffer[Pos+5]==CAM_COMMAND_BYTE_H)
	{
		Pos+=11;
		while(Size_Pakage--)
			Data->Data[i++]=Buffer[Pos++];
		
		FLASH_WriteBuffer(Data->Data,Camera.AddtoWrite,1024);
		
		delay_ms(1);
		sprintf(buf,"Add= %d \r",Camera.AddtoWrite);
		Sendstring(buf);
		Camera.AddtoWrite+=1024;
		return OK;
	}
	return 1;
}

/**************************************************************************************
* Function Name  	: Cam_Send
* Return         	: None
* Parameters 		: None
* Description		: Send take data command to cam 
*************************************************************************************/
void Cam_Send(void)
{
	char Command[14]={NULL}, buf[2];
	uint8_t Buffer[14];
	char Header[]={"5545"}, Endl[]={"0023"};
	
	strcat(Command,Header);
	sprintf(buf,"%02X",Cam_Data.ID_Cam);
	strcat(Command,buf);
	sprintf(buf,"%02X",Cam_Data.Count);
	strcat(Command,buf);
	strcat(Command,Endl);
	
	
	Cam_Data.Count++;
	if(Cam_Data.NumberofPackage==0)
		Cam_Data.Count=1;
	
	hex_decode(Command,14,Buffer);
	Sendstring4s((uint8_t *)Buffer,6);
	memset(Buffer,0,14);
}
