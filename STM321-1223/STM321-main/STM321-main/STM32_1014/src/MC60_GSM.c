#include "MC60_GSM.h"
#include "main.h"
#include "flash.h"
#include "Structure_Data.h"
#include "string.h"
#include "GSM_GPRS.h"


extern SystemVariables          xSystem;
extern GPS_Structure           GPS_xData;
extern GPSControlStruct        GPS_Temp,GPSControl; 
extern MC60Structure					 GSM;
extern GPRS_Structure					 GPRS;
/*******************************************************************************
 * Function Name  	:
 * Return         	: None
 * Parameters 		: None
 * Description		: 
*******************************************************************************/
uint8_t MC60_PWROn(void)
{
	
	uint8_t		Status = 0;
	uint8_t		StatusAT = 0;
	uint8_t		ResetCount = 0;
	ResetCount = 0;
	
	REPEART:
	Watchdog_Reset();
	StatusAT = MC60_CheckConnect();
	/* Chan trang thai o? muc 1,thuc hien lenh AT, module chay OK */
	if(StatusAT)
		return 1;
	if(StatusAT == 0 || ResetCount > 1)
	{	
		//Reset module mem
		MC60_PWKEY_LOW();
		delay_ms(500);
		MC60_PWKEY_HIGH();
		delay_ms(500);	
	}
		if(ResetCount>=3)
	{
	}
	if(Status == 0)
		goto REPEART; 
	
	return 1;
}
/*******************************************************************************
 * Function Name  	:
 * Return         	: None
 * Parameters 		: None
 * Description		: 
*******************************************************************************/
void MC60_GSM_Init(uint32_t Baudrate )
{
	MC60_HardwareInit(Baudrate);
	MC60_IOInit();
	MC60_SoftwareInit();
	
}
/*******************************************************************************
 * Function Name  	:
 * Return         	: None
 * Parameters 		: None
 * Description		: 
*******************************************************************************/
void MC60_HardwareInit(uint32_t Baudrate)
{	
	GPIO_InitTypeDef 			  GPIO_Structure;
	USART_InitTypeDef 			USART_Structure;
	NVIC_InitTypeDef  	  	 NVIC_Structure;
		
	USART_DeInit(USART3);
	USART_Cmd(USART3,DISABLE);
	USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);
	
	//* Enable Clock for USART3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	// Conection Pin to USART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
	//Configure PinTx
	GPIO_Structure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_Structure.GPIO_OType=GPIO_OType_PP;
	GPIO_Structure.GPIO_Pin=GPIO_Pin_10;
	GPIO_Structure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Structure.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_Structure);
	//Configure PinRx
	GPIO_Structure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_Structure.GPIO_Pin=GPIO_Pin_11;
	GPIO_Structure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_Structure);
	// Configure USART3
	USART_Structure.USART_BaudRate = Baudrate;
  USART_Structure.USART_WordLength = USART_WordLength_8b;
  USART_Structure.USART_StopBits = USART_StopBits_1;
  USART_Structure.USART_Parity = USART_Parity_No ;
  USART_Structure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Structure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_Structure);
	/* Configure Interrupt */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_Structure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_Structure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_Structure.NVIC_IRQChannelSubPriority = 2;
  NVIC_Structure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_Structure);

  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	/* Enable USART */
  USART_Cmd(USART3, ENABLE);
}
	
/*******************************************************************************
 * Function Name  	:
 * Return         	: None
 * Parameters 		: None
 * Description		: 
*******************************************************************************/
void MC60_IOInit(void)
{
	GPIO_InitTypeDef  GPIO_Structure;
	
	//Clock PowerKey, NetLight
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	// Configure Powerkey
	GPIO_Structure.GPIO_OType = GPIO_OType_PP;
	GPIO_Structure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Structure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_Structure);
	// Power Control
	GPIO_Structure.GPIO_Pin =GPIO_Pin_6 ;
	GPIO_Init(GPIOC, &GPIO_Structure);
	// Net Light
//	GPIO_Structure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Structure.GPIO_Pin = GPIO_Pin_10;
//	GPIO_Init(GPIOE, &GPIO_Structure);
	// Powerctrl_Hight
	MC60_PWCTRL_HIGH();
	MC60_PWKEY_HIGH();
}
/*******************************************************************************
 * Function Name  	:
 * Return         	: None
 * Parameters 		: None
 * Description		: 
*******************************************************************************/
void MC60_SoftwareInit(void)
{
	mem_set(GSM.RxBuffer, 0, MC60_RX_BUFFER);
	GSM.RxCounter = 0;
	GSM.TimeOutReceive = 0;
	GSM.TimeOutConnection = 0;
	GSM.TickCounter = 0;

}
/*******************************************************************************
 * Function Name  	:
 * Return         	: None
 * Parameters 		: None
 * Description		: 
*******************************************************************************/
uint8_t MC60_CheckConnect(void)
{
	uint8_t RetryCount = 0;
	Sendstring("\rKiem Tra Ket Noi Module : ");
	for(RetryCount = 0; RetryCount < 5; RetryCount++)
	{
		if(MC60_SendCommand("AT\r",OKRes,300)) 
		{
			Sendstring("[YES]\r");
			return 1;
		}
		Watchdog_Reset();
	}

	Sendstring("[NO]\r");
//	Sendstring((char*)GSM.RxBuffer);
	return 0;    
}

	


/*******************************************************************************
 * Function Name  	:MC60_SendCommand
 * Return         	: None
 * Parameters 		: None
 * Description		: 
*******************************************************************************/
uint8_t MC60_SendCommand(const char* str, const char* response, int32_t TimeOut)
{
	uint8_t Result = 0;
	/* Xoa buffer du lieu */
	MC60_XoaBoDemNhan();
	Watchdog_Reset();
	/*Gui lenh den hop den*/
	MC60_SendATs(str);
	xSystem.NewGSMData = 0;

	while(TimeOut)
	{
		
		if(xSystem.NewGSMData)
		{
			if(strstr((char*)GSM.RxBuffer, response)) 
			{
				Result = 1; 			
				break;
			}

      MC60_XoaBoDemNhan();                		//Nhan duoc ket qua khong dung thi xoa du lieu bo dem vua nhan 		
			xSystem.NewGSMData = 0;	                    //Xoa co bao co du lieu
		}
		if(xSystem.Flag10ms >= 10)
		{
			TimeOut -= 10;
			xSystem.Flag10ms = 0;
		}
	}	
	//Sendstring((char*)GSM.RxBuffer);
	return Result;
}
/*******************************************************************************
 * Function Name  	:MC60_SendCommand
 * Return         	: None
 * Parameters 		: None
 * Description		: 
*******************************************************************************/
void MC60_XoaBoDemNhan(void)
{
	//Sendstring((char*)GSM.RxBuffer);
	GSM.RxCounter=0;
	xSystem.NewGPSData=0;
	mem_set(GSM.RxBuffer, 0, MC60_RX_BUFFER);
}

/*******************************************************************************
 * Function Name  	: GSM_SendByte
 * Return         	: None
 * Parameters 		: Byte can gui sang module
 * Description		: Gui mot byte du lieu sang module GSM
*******************************************************************************/
void MC60_SendByte(char Byte)
{
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == 0)
	{}
	USART_SendData(USART3, (uint16_t)Byte);
}

/*******************************************************************************
 * Function Name  	: GSM_SendBytes
 * Return         	: None
 * Parameters 		: Mang du lieu can gui sang module
 * Description		: Gui mang byte du lieu sang module GSM
*******************************************************************************/
void MC60_SendBytes(uint8_t *Buffer, uint16_t Length)
{
	uint16_t i = 0;
	for(i = 0; i < Length; i++)
		MC60_SendByte((char) Buffer[i]);
}

/*******************************************************************************
 * Function Name  	: GSM_SendATs
 * Return         	: None
 * Parameters 		: None
 * Description		: Gui mot chuoi sang module GSM
*******************************************************************************/
void MC60_SendATs(const char* str)
{
	while (*str)
	{
		MC60_SendByte(*str);
		Watchdog_Reset();
    if(GSM.Debug)
	  Sendbyte(*str);
     str++;
	}
}
/*******************************************************************************
 * Function Name  	: GSM_Init
 * Return         	: None
 * Parameters 		: None
 * Description		: Trinh tu Khoi tao cac chuc nang trong GSM
*******************************************************************************/
void GSM_Init(void)
{
	MC60_GSMConfig();
	Delay_ms(50);
	MC60_GPRSConfig();
	Delay_ms(50);
	MC60_GPRS_Init();
	Delay_ms(50);
	MC60_SMS_Init();
	GSM.RxCounter=0;
}
/*******************************************************************************
 * Function Name  	: MC60_GSMConfig
 * Return         	: None
 * Parameters 		: None
 * Description		: Cau hinh cho module MC60 vao che do GSM
*******************************************************************************/
void MC60_GSMConfig(void)
{
	int Count;
	uint8_t ErrorOccur = 0;
	
	//Kiem tra ket noi den Module
	for(Count = 0; Count < 5; Count++)  //10
	{ 
		if(MC60_SendCommand("AT\r",OKRes,300))   // 5
		{
			Sendstring("*******Configure GSM*******\r");
			break;
		}
		else 
		{
			ErrorOccur++;
			delay_ms(200);//5
		}
	}
	Watchdog_Reset();
	/**********************Lay GSM IMEI*********************/
	for(Count = 0; Count < 5; Count++)   
	{
		mem_set(xSystem.GSM_IMEI, 0, 50);
		MC60_GetIMEI(GSMIMEI);
		if(xSystem.GSM_IMEI[0] != 0 && xSystem.GSM_IMEI[1] != 0 &&
			 xSystem.GSM_IMEI[2] != 0 && xSystem.GSM_IMEI[3] != 0 &&
			 xSystem.GSM_IMEI[4] != 0 && xSystem.GSM_IMEI[5] != 0) 
		Count = 21;
		Watchdog_Reset();
	}
	delay_ms(200);
	
	/**********************Lay SIM IMEI*********************/
	for(Count = 0; Count < 5; Count++)   
	{
		mem_set(xSystem.SIM_IMEI, 0, 50);
		MC60_GetIMEI(SIMIMEI);
		if(xSystem.SIM_IMEI[0] != 0 && xSystem.SIM_IMEI[1] != 0 &&
			 xSystem.SIM_IMEI[2] != 0 && xSystem.SIM_IMEI[3] != 0 &&
			 xSystem.SIM_IMEI[4] != 0 && xSystem.SIM_IMEI[5] != 0) 
		Count = 21;//51
		Watchdog_Reset();
	}
	delay_ms(200);//5
	/**********************Send ATZ*********************/
	Sendstring("GSM_Define_Profile:");
	for(Count=0;Count<5;Count++)
	{
		if(MC60_SendCommand("ATZ\r",OKRes,100))
		{
			Sendstring("[YES]\r");
			break;
		}
		else
		{
			Sendstring("[NO]\r");
			ErrorOccur=1;
		}
	}
	delay_ms(20);
	Watchdog_Reset();
	/**********************Send ATE*********************/
	Sendstring("GSM_Echo_Command:");
	for(Count=0;Count<5;Count++)
	{
		if(MC60_SendCommand("ATE\r",OKRes,100))
		{
			Sendstring("[YES]\r");
			break;
		}
		else
		{
			Sendstring("[NO]\r");
			ErrorOccur=1;
		}
	}
	delay_ms(20);
	Watchdog_Reset();
	/**********************Send ATCLIP*********************/
	Sendstring("GSM_ATCLIP:");
	for(Count=0;Count<5;Count++)
	{
		if(MC60_SendCommand("AT+CLIP=1\r",OKRes,100))
		{
			Sendstring("[YES]\r");
			break;
		}
		else
		{
			Sendstring("[NO]\r");
			ErrorOccur=1;
		}
	}
	delay_ms(20);
	Watchdog_Reset();
	/**********************Send Save Configure*********************/
	Sendstring("Save Configure Current:");
	if(ErrorOccur!=1)
	{
		if(MC60_SendCommand("AT&W\r",OKRes,100))
		{
			Sendstring("[YES]\r");
		}
		else
		{
			Sendstring("[NO]\r");
			ErrorOccur=1;
		}
	}
	delay_ms(20);
	Watchdog_Reset();
	/**********************Send SMS Message Format*********************/
	Sendstring("Select SMS Message Format:");
	for(Count=0;Count<5;Count++)
	{
		if(MC60_SendCommand("AT+CMGF=1\r",OKRes,100))
		{
			Sendstring("[TEXT]\r");
			break;
		}
		else
		{
			Sendstring("[ERROR]\r");
			ErrorOccur=1;
		}
	}
	delay_ms(20);
		Watchdog_Reset();
	/**********************Send SMS Message Format*********************/
	Sendstring("SMS Format(Sua lai lenh AT sau):");
	for(Count = 0; Count < 5; Count++)//10
	{
		if(MC60_SendCommand("AT+CNMI=2,1,0,0\r",OKRes,300))
		{
			Sendstring("[YES]\r");
			break;
		}
		else
		{
			Sendstring("[NO]\r");
		}
	}
	delay_ms(20);	
	Watchdog_Reset();
}
/*******************************************************************************
 * Function Name  	: MC60_GPRSConfig
 * Return         	: None
 * Parameters 		: None
 * Description		: Cau hinh cho module MC60 vao che do GPRS
*******************************************************************************/
void MC60_GPRSConfig(void)
{
	uint8_t Count=0;
	/**********************Send Add an IP Header*********************/
	Sendstring("Add an IPHeader:");
	for(Count=0;Count<5;Count++)
	{
		if(MC60_SendCommand("AT+QIHEAD=1\r",OKRes,100))
		{
			Sendstring("[YES]\r");
			break;
		}
		else
		{
			Sendstring("[NO]\r");
		}
	}
	delay_ms(20);
	Watchdog_Reset();
	/**********************Send Select a Context *********************/
	Sendstring("Select a Context :");
	for(Count=0;Count<5;Count++)
	{
		if(MC60_SendCommand("AT+QIFGCNT=0\r",OKRes,100))
		{
			Sendstring("[YES]\r");
			break;
		}
		else
		{
			Sendstring("[NO]\r");
		}
	}
	delay_ms(20);
	Watchdog_Reset();
	/**********************Send Select TCP/IP Transfer Mode *********************/
	Sendstring("Select TCP/IP :");
	for(Count=0;Count<5;Count++)
	{
		if(MC60_SendCommand("AT+QIMODE=0\r",OKRes,100))
		{
			Sendstring("[YES]\r");
			break;
		}
		else
		{
			Sendstring("[NO]\r");
		}
	}
	delay_ms(20);
	Watchdog_Reset();
	/**** Start TCP/IP Task and Set APN, User Name and Password ******/
	Sendstring("Start TCP/IP Task :");
	for(Count=0;Count<5;Count++)
	{
		if(MC60_SendCommand("AT+QIREGAPP\r",OKRes,100))
		{
			Sendstring("[YES]\r");
			break;
		}
		else
		{
			Sendstring("[NO]\r");
		}
	}
	delay_ms(20);
	Watchdog_Reset();
	/********************** Show CCID*********************/
	Sendstring("Show CCID:");
	for(Count=0;Count<5;Count++)
	{
		if(MC60_SendCommand("AT+QCCID\r",OKRes,100))
		{
			Sendstring("[YES]\r");
			break;
		}
		else
		{
			Sendstring("[NO]\r");
		}
	}
	delay_ms(20);
	Watchdog_Reset();
	/*************** Network Registration Status***************/
	Sendstring("Network Registration Status:");
	for(Count=0;Count<5;Count++)
	{
		if(MC60_SendCommand("AT+CREG=2\r",OKRes,500))
		{
			Sendstring("[YES]\r");
			break;
		}
		else
		{
			Sendstring("[NO]\r");
		}
	}
	delay_ms(20);
	
	/******** Connect with IP Address or Domain Name Server **********/
	Sendstring("Connect with IP Address:");
	for(Count=0;Count<5;Count++)
	{
		if(MC60_SendCommand("AT+QIDNSIP=1\r",OKRes,500))
		{
			Sendstring("[YES]\r");
			break;
		}
		else
		{
			Sendstring("[NO]\r");
		}
	}
	delay_ms(20);
	Watchdog_Reset();
}
/*******************************************************************************
 * Function Name  	: MC60_GPRS_Init
 * Return         	: None
 * Parameters 		: None
 * Description		: Khoi tao cac bien su dung trong GPRS
*******************************************************************************/
void MC60_GPRS_Init(void)
{
	GPRS.NumberSendofLogins=0;
	GPRS.Mode=GSM_RECONNECT;
	GPRS.TimeTickLogins=0;
	GSM.TickCounter=0;
	xSystem.Logined=0;
	xSystem.NewGSMData=0;
	
}

/*******************************************************************************
 * Function Name  	: MC60_SMS_Init
 * Return         	: None
 * Parameters 		: None
 * Description		: Khoi tao module SMS
*******************************************************************************/
void MC60_SMS_Init(void)
{
	SMS_Delete_15SMS();
}
/*******************************************************************************
 * Function Name  	: SMS_Delete_15SMS
 * Return         	: None
 * Parameters 		: None
 * Description		: Xoa 15 tin nhan gan day
*******************************************************************************/
void SMS_Delete_15SMS(void)
{
	Watchdog_Reset();
	Sendstring("Delete 15 SMS Message\r");
	MC60_SendCommand("AT+CMGD=1\r",OKRes,1000);
	MC60_SendCommand("AT+CMGD=2\r",OKRes,1000);
	MC60_SendCommand("AT+CMGD=3\r",OKRes,1000);
	MC60_SendCommand("AT+CMGD=4\r",OKRes,1000);
	MC60_SendCommand("AT+CMGD=5\r",OKRes,1000);
	MC60_SendCommand("AT+CMGD=6\r",OKRes,1000);
	MC60_SendCommand("AT+CMGD=7\r",OKRes,1000);
	MC60_SendCommand("AT+CMGD=8\r",OKRes,1000);
	MC60_SendCommand("AT+CMGD=9\r",OKRes,1000);
	MC60_SendCommand("AT+CMGD=10\r",OKRes,1000);
	MC60_SendCommand("AT+CMGD=11\r",OKRes,1000);
	MC60_SendCommand("AT+CMGD=12\r",OKRes,1000);
	MC60_SendCommand("AT+CMGD=13\r",OKRes,1000);
	MC60_SendCommand("AT+CMGD=14\r",OKRes,1000);
	MC60_SendCommand("AT+CMGD=15\r",OKRes,1000);
}
/*******************************************************************************
 * Function Name  	: MC60_GetIMEI
 * Return         	: None
 * Parameters 		: None
 * Description		: Nhan thong tin IMEI SIM va GSM
*******************************************************************************/
void MC60_GetIMEI(uint8_t Type)
{
	char Buffer[50];
	uint16_t TimeOut = 100;
	uint8_t Count = 0,Count1=0;
		Watchdog_Reset();
	/* Xoa buffer du lieu */
    MC60_XoaBoDemNhan();
	/*Gui lenh kiem tra module*/
    if(Type == GSMIMEI)
      MC60_SendATs("AT+GSN\r");
    else
      MC60_SendATs("AT+QCCID\r");
		 /* Xu ly trong 2s */
    xSystem.NewGSMData = 0;
		while (TimeOut)
		{
			if (xSystem.NewGSMData)
			{ 
				
				for (Count=0;Count<strlen((char *)GSM.RxBuffer);Count++)
				{
					if(GSM.RxBuffer[Count]>'0'&&GSM.RxBuffer[Count]<'9')
					{
						if(Type==GSMIMEI)
							xSystem.GSM_IMEI[Count1++]=GSM.RxBuffer[Count];
						else
							xSystem.SIM_IMEI[Count1++]=GSM.RxBuffer[Count];
					}
					if (Count1 >= 20) break;
				}
			}	
			if (xSystem.Flag10ms >= 10)
				{
				TimeOut -= 10;
				xSystem.Flag10ms = 0;
				}
			Watchdog_Reset();
	 }
	 
	 if(Type == GSMIMEI) 
   {
		 sprintf(Buffer,"GSM_IMEI: %s\r",xSystem.GSM_IMEI);
		 Sendstring(Buffer);
	 }
    else
    {
			sprintf(Buffer,"SIM_IMEI: %s\r",xSystem.SIM_IMEI);
			Sendstring(Buffer);
    }
	 
}
/*******************************************************************************
 * Function Name  	: MC60_ResetStep
 * Return         	: None
 * Parameters 		: None
 * Description		: Cac buoc thuc hien reset module
*******************************************************************************/
void MC60_Resetstep(void)
{
	switch(GSM.TickCounter)
	{
		case 2:
			break;
		case 4:
			Sendstring("\r Thuc hien TAT module GSM\r");
			MC60_SendATs("AT+QPOWD=1\r");
			break;
		case 10:
			Sendstring("TAT NGUON GSM\r");
			MC60_PWKEY_LOW();
			break;
		case 100:
			Sendstring("BAT NGUON GSM\r");
			MC60_PWKEY_HIGH();
			break;
		case 160:
			GSM_Init();
			xSystem.ConnectOK = -1;
			xSystem.Logined=0;
			MC60_ChangeMode(GSM_OK);
			break;
	}
}
