#include "stm32f2xx.h"                  // Keil::Device:Startup
#include "main.h"
#include "MC60_GSM.h"
#include "string.h"
#include "Structure_Data.h"
#include "stdlib.h"
#include "LED.h"
#include "GSM_GPRS.h"




extern MC60Structure			GSM;
extern SystemVariables          xSystem;
extern GPRS_Structure			GPRS;
char 							MainIPAddr[60];
extern ConfStructure 		    Config;
extern ConfigUnion           	xConfig;
//extern GPRSDataStructure     GPRSData;


uint16_t			SendDataTimeOut = 0;
static 	uint8_t		ConnectFailCount = 0;




/*******************************************************************************
 * Function Name  	:MC60_SendCommand
 * Return         	: None
 * Parameters 		: None
 * Description		: 
*******************************************************************************/
void CheckSignalQuality(void)
	
{
	char CSQ[3]={NULL},buf[20];
	uint8_t xCSQ;
	MC60_XoaBoDemNhan();
	if(MC60_SendCommand("AT+CSQ\r",OKRes,500))
	{ 
		if(strstr((char*)GSM.RxBuffer,"ERROR"))
		{
			return;
		}
		if(strstr((char*)GSM.RxBuffer,"+CSQ"))
		{
			uint8_t Count=0;	
			for(Count=0;Count<strlen((char*)GSM.RxBuffer);Count++)
			{
				if(GSM.RxBuffer[Count]==':')
				{
					CSQ[0]=GSM.RxBuffer[Count+1];
					CSQ[1]=GSM.RxBuffer[Count+2];
					CSQ[2]=GSM.RxBuffer[Count+3];
				}
				xCSQ=atoi(CSQ);
			}
			sprintf(buf,"+CSQ= %d\r",xCSQ);
			Sendstring(buf);
		}
	Watchdog_Reset();
		
	}
}
/*******************************************************************************
 * Function Name  	:MC60_SendCommand
 * Return         	: None
 * Parameters 		: None
 * Description		: 
*******************************************************************************/
void MC60_Process_100ms(void)
{
	char buff[50];
	char SMS1[]="AT+QIOPEN=\"TCP\",\"";
	char SMS2[]="\",\"";
	char SMS3[]="\"";
		Watchdog_Reset();
	if(xSystem.Logined == 0)
		xSystem.TimeNotLogin++;
	else xSystem.TimeNotLogin = 0;
	if(xSystem.TimeNotLogin >= 10000)
	{
		//led_toggle1();
	}
	//Ket noi that bai qua 5 lan thi reset module
	if(ConnectFailCount>5)
	{
		Sendstring("\rReset Module GSM\r\t");
		GSM.TickCounter = 0 ;
		GPRS.Mode = GSM_RESET;
		ConnectFailCount = 0;
		
		
	}
	switch(GPRS.Mode)
	{
		/*Trang thai on dinh*/
		case GSM_OK:
			Watchdog_Reset();
			GSM.TickCounter = 0;
			/*Neu connect OK*/
			if(xSystem.ConnectOK == -1)
			{
			MC60_ChangeMode(GSM_RECONNECT);
			}
			break;
		case GSM_RESET:
		{
			Watchdog_Reset();
			MC60_Resetstep();	
		}
		break;
		case GSM_RECONNECT:
		{
			Watchdog_Reset();
			xSystem.Logined=0;
			if(GSM.TickCounter==5)
			{
				MC60_SendATs("AT+QICLOSE\r");
				xSystem.ConnectOK = -1;
				Sendstring("Close Connect\r");
			}
			if(GSM.TickCounter == 10)
			{
				xSystem.ConnectOK = -1;
				strcpy(MainIPAddr,SMS1);
				strcat(MainIPAddr,xConfig.Cauhinh.IP);
				strcat(MainIPAddr,SMS2);
				strcat(MainIPAddr,xConfig.Cauhinh.PORT);
				strcat(MainIPAddr,SMS3);
				MC60_SendATs(MainIPAddr);
				sprintf(buff,"Ket noi den dia chi: %s\r",xConfig.Cauhinh.PORT);
				Sendstring(buff);
				ConnectFailCount++;
//					MC60_SendCommand("AT+CSQ\r",OKRes,500);
			}
		// Ket noi thanh cong
			if(GSM.TickCounter > 10)//20
			{
				/* Neu connect OK thi chuyen sang che do doi */
					if(xSystem.ConnectOK == 1) 
					{
						MC60_ChangeMode(GSM_OK);	
						Sendstring("\rGPRS: Ket noi server OK");
						ConnectFailCount = 0;
					}
			}
			if(GSM.TickCounter > 200)//350
			{								
			/* Neu connect fail, chuyen ve che do connect lai */
				if(xSystem.ConnectOK != 1)
				{
					MC60_ChangeMode(GSM_RECONNECT);										
				}
			}	
		}
	}
	GSM.TickCounter++;
}
/*******************************************************************************
 * Function Name  	:MC60_SendCommand
 * Return         	: None
 * Parameters 		: None
 * Description		: 
*******************************************************************************/
void MC60_ChangeMode(uint8_t Mode)
{
	GPRS.Mode=Mode;
	GSM.TickCounter=0;
	if(GPRS.Mode!= GSM_RECONNECT && GPRS.Mode!=GSM_RESET)
	{
	}		

}
