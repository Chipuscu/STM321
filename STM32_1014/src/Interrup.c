#include "Interrup.h"
#include "main.h"
#include "stm32f2xx_it.h"
#include "LED.h"
#include "flash.h"
#include "Data.h"
#include "UART.h"
#include "RTC_Time.h"
#include "GPS_Process.h"
#include "Structure_Data.h"
#include "RTC.h"




extern SystemVariables		     xSystem;
extern GPS_Structure           GPS_xData;
extern GPSControlStruct        GPS_Temp,GPSControl; 
extern MC60Structure					 GSM;
extern GPS_Structure					 GPS;
extern ConfStructure           Config;
extern Buffer_TypeDef					UART1_Data;
extern CamDataStruct						Cam_Data;
extern CamStructure							Camera;


/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : Nhan du lieu tu cong debug
*******************************************************************************/
void USART1_IRQHandler(void)
{   
		uint16_t Buffer;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
			USART_ClearITPendingBit(USART1, USART_IT_RXNE);
			Buffer=USART_ReceiveData(USART1);
			Ring_Buffer_Write(&UART1_Data,Buffer);
			//USART1_ReceiData((char)Buffer);
			Config.TimeOutReceive=50;
		}		
}
/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : GPS
*******************************************************************************/
void USART2_IRQHandler(void)
{
	uint16_t  Data;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		Data = USART_ReceiveData(USART2);
		MC60_GPS_ReceiData(Data);
		//GPS_Receidata(Data);
		GPS.TimeOutReceive = 70	;
		
  }
}
/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : GSM
*******************************************************************************/
void USART3_IRQHandler(void)
{	
	uint16_t Buffer;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
			USART_ClearITPendingBit(USART3, USART_IT_RXNE);
			Buffer=USART_ReceiveData(USART3);
			MC60_GSM_ReceiData(Buffer);
			GSM.TimeOutReceive = 30	;
		}
	
}
/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : GSM
*******************************************************************************/
void UART4_IRQHandler(void)
{
	uint16_t Buffer;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
			USART_ClearITPendingBit(UART4, USART_IT_RXNE);
			Buffer=USART_ReceiveData(UART4);
			UART4_ReceiData(Buffer);
			Camera.TimeOutReceive=70;
		}
}
/*******************************************************************************
* Function Name  	: TIM2_IRQHandler
* Return         	:
* Parameters 		: 
* Created by		: 
* Date created	: 
* Description		: This function handles TIM2 global interrupt request.
* Notes			: 			
*******************************************************************************/
void TIM2_IRQHandler(void)                        //Timer1s
{	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		//led_toggle();
	//	Sendstring4((char *)Config.Buffer,14);;

	}
}

/*******************************************************************************
* Function Name  	: TIM3_IRQHandler
* Return         	: 
* Parameters 		: 
* Created by		:
* Date created	: 
* Description		: This function handles TIM3 global interrupt request.
* Notes			: 
*******************************************************************************/
void TIM3_IRQHandler(void)												//Timer0.5s
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

	}
}

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
*******************************************************************************/
void SysTick_Handler(void)
{	
	/*Countdown time delay*/
	if(xSystem.Delay_ms>0)
	{
		xSystem.Delay_ms--;
	}
	 /* Tao co ngat 100 ms */
	if(xSystem.Flag10ms < 10)
		xSystem.Flag10ms++;
    /* Tao co ngat 100 ms */
  if(xSystem.Flag100ms < 100)
		xSystem.Flag100ms++;
 /* Kiem tra du lieu nhan duoc tu module GSM */
	if(GSM.TimeOutReceive > 0)
		{
			GSM.TimeOutReceive--;
			if(GSM.TimeOutReceive == 0)
				xSystem.NewGSMData = 1;
		}
 /* Kiem tra du lieu nhan duoc tu module GPS */
	if(GPS.TimeOutReceive > 0)
	{
		GPS.TimeOutReceive--;
		if(GPS.TimeOutReceive == 0)
			xSystem.NewGPSData = 1;
	}

	/* Kiem tra du lieu nhan duoc tu cong Debug*/
	if(Config.TimeOutReceive > 0)
	{
		Config.TimeOutReceive--;
		if(Config.TimeOutReceive == 0)
			{
				Config.RxCounter=0;
				xSystem.NewConfData = 1;
			}
	}
	 /* Kiem tra du lieu nhan duoc tu camera */
	if(Camera.TimeOutReceive > 0)
		{
			Camera.TimeOutReceive--;
			if(Camera.TimeOutReceive == 0)
				xSystem.NewCAMData = 1;
		}
	/* Kiem tra Thoi gian de luu ban tin GPS*/
	if(GPS.TimeoutSaveData<30001)
	{
		GPS.TimeoutSaveData++;
		if(GPS.TimeoutSaveData==30000)
		{
			GPS.SaveDataGPS=1;
			GPS.TimeoutSaveData=0;
		}
	}
	/*Kiem tra thoi gian*/
}
/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void RTC_IRQHandler(void)
{
	if (RTC_GetITStatus(RTC_IT_TS) != RESET)
	{
		/* Clear the RTC Second interrupt */
		RTC_ClearITPendingBit(RTC_IT_TS);   
//		led_toggle1();

	}
}
