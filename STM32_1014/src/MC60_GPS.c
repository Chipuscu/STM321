#include "GPS_Process.h"
#include "main.h"
#include "string.h"
#include "flash.h"
#include "Data.h"
#include "stdlib.h"
#include "LED.h"
#include "Structure_Data.h"
#include "math.h"
#include "RTC.h"
#include "MC60_GPS.h"


extern SystemVariables          xSystem;
extern GPS_Structure           GPS_xData;
extern GPSControlStruct        GPS_Temp,GPSControl;  //Bien tam trong qua trinh xu ly data

extern GPS_Structure						GPS;
extern MC60Structure					 GSM;
/*******************************************************************************
 * Function Name  	:GPS_Initialize
 * Return         	: None
 * Parameters 		: 
 * Description		: Khoi tao module GPS
*******************************************************************************/
void MC60_GPS_Init(uint32_t Baud)
{
	GPS_InitHardware(Baud);
	GPS_SendString("$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n");
	delay_ms(500);
	GPS_SendString("$PQBAUD,W,9600*4B\r\n");
	delay_ms(500);
	GPS_InitHardware(9600);
	GPS_SoftwareKhoiTao();
	delay_ms(500);
	GPS_SendCommand(GPS_ONLY_GPRMC_CMD );  // chi gui ban tin GPRMC
	delay_ms(500);
	GPS_SendCommand(GPS_ONLY_GPRMC_CMD );  // chi gui ban tin GPRMC
	delay_ms(500);
	Watchdog_Reset();
	
	//Sendstring((char*)GPS.RxBuffer);

	
}

/*******************************************************************************
 * Function Name  	: GPS_SendByte
 * Return         	: None
 * Parameters 		: Byte can gui sang module
 * Description		: Gui mot byte sang module GPS
*******************************************************************************/
static void GPS_SendByte(char Byte)
{
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == 0);
	USART_SendData(USART2, (uint16_t)Byte);
}

/*******************************************************************************
 * Function Name  	: GPS_SendString
 * Return         	: None
 * Parameters 		: Chuoi can gui sang module
 * Description		: Gui mot chuoi sang module GPS
*******************************************************************************/
void GPS_SendString(const char* s)
{
	char c;
	while ( ( c = *s++) != '\0' )
		GPS_SendByte(c);	
}

/*******************************************************************************
 * Function Name  	: GPS_HardwareInitialize
 * Return         	: None
 * Parameters 		: Toc do giao tiep cong Com
 * Description		: Khoi tao module giao tiep voi GPS
*******************************************************************************/
static void GPS_InitHardware(uint32_t Baudrate)
{
	GPIO_InitTypeDef 			GPIO_InitStructure;
	USART_InitTypeDef 			USART_InitStructure;
	NVIC_InitTypeDef 			NVIC_InitStructure;
	
	USART_DeInit(USART2);
	USART_Cmd(USART2, DISABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	
	/* luon luon enable clock truoc khi goi cac ham` config ngoai vi */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOA, ENABLE);

	/* Connect Pin to USART */
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

	/* Configure TX, RX pin */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  	USART_InitStructure.USART_BaudRate = Baudrate;
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  	USART_InitStructure.USART_Parity = USART_Parity_No ;
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	/* Configure Interrupt */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    USART_Cmd(USART2, ENABLE);
	
	/* Chan GPS POWER */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Bat nguon, chan nay co tac dung voi MC60 */
	GPS_On();
	Watchdog_Reset();
}
/*******************************************************************************
 * Function Name  	: GPS_HardwareInitialize
 * Return         	: None
 * Parameters 		: Toc do giao tiep cong Com
 * Description		: Khoi tao module giao tiep voi GPS
*******************************************************************************/
void GPS_ControlCommand(uint8_t u8_ControlState)
{
	switch(u8_ControlState)
	{
		case	GPS_CTRL_PWR_OFF:
			/* Chan trans cap nguon */
			GPS_Off();		
		break;
		
		case	GPS_CTRL_PWR_ON:
			GPS_On();
		break;
		
		case GPS_CTRL_RST_COLD:
			GPS_SendString("$PMTK103*30\r\n");
		break;
		
		case GPS_CTRL_RST_HOT:
			GPS_SendString("$PMTK101*32\r\n");
		break;
		
		case GPS_RESET_FACTORY:
			GPS_SendString("$PMTK104*37\r\n");
		break;	
	}
}
/*******************************************************************************
 * Function Name  	: GPS_Initialize
 * Return         	: None
 * Parameters 		: None
 * Description		: Khoi dong module GPS
*******************************************************************************/
void GPS_KhoiTao(uint32_t Baudrate)
{
	/* Khoi tao phan cung module GPS */
	GPS_InitHardware(Baudrate);
	GPS_SendString("$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n");
	delay_ms(500);
	GPS_SendString("$PQBAUD,W,9600*4B\r\n");
	delay_ms(500);
	GPS_InitHardware(9600);
	/* Khoi tao cac bien dung trong module GPS */
	GPS_SoftwareKhoiTao();
	delay_ms(500);
	/* Cau hinh cho module chi gui ban tin GPRMC, GGA */
	GPS_SendCommand(GPS_ONLY_GPRMC_GPGGA_CMD);
	delay_ms(500);
	GPS_SendCommand(GPS_ONLY_GPRMC_GPGGA_CMD);
	Sendstring("\rKhoi Tao Module GPS");
}
/*******************************************************************************
 * Function Name  	: GPS_SendCommand
 * Return         	: None
 * Parameters 		: Lenh gui xuong module GPS
 * Description		: Ham gui lenh xuong module GPS
*******************************************************************************/
void GPS_SendCommand(uint8_t Command)
{
	switch(Command)
	{
		case GPS_GET_VERSION_CMD:
			GPS_SendString("$PMTK605*31\r\n");
			break;
			
			case GPS_ONLY_GPRMC_CMD:
			GPS_SendString("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
			break;          
			
 		  case GPS_ONLY_GPGGA_CMD:
		  GPS_SendString("$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
			break;        
			
 		  case GPS_ONLY_GPRMC_GPGGA_CMD:
		  GPS_SendString("$PMTK314,0,1,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*2A\r\n");
			break;        
			
 		  case GPS_DISABLE_OUTPUT_CMD:
		  GPS_SendString("$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n");
			break;      
			
			case GPS_CTRL_RST_WARM:
			GPS_SendString("$PMTK102*31\r\n");
		  break;
	}
}
/*******************************************************************************
 * Function Name  	: GPS_SoftwareInitialize
 * Return         	: None
 * Parameters 		: None
 * Description		: Khoi tao cac bien dung trong module GPS
*******************************************************************************/
void GPS_SoftwareKhoiTao(void)
{
	GPS_ResetData();
	
//	//Thoi gian timeout dau tien
//	BaudRateCount = CHANGEBAUDCOUNT;
//	GPSResetCount = GPSMAXTIMEOUT;
//	GPS.IsBaudrateValid = GPS_INVALID;
//	GPS.Debug = DISABLE;
//	GPS.brokenmax=0;
//	GPS.failmax=0;
//	GPS.TimeOutConnection = 0;
//	GPS.check_RMC=0;
}
/*******************************************************************************
 * Function Name  	: GPS_ResetData
 * Return         	: None
 * Parameters 		: None
 * Description		: Khoi tao cac bien
*******************************************************************************/
void GPS_ResetData(void)
{
	GPSControl.Longitude = 0;
	GPSControl.Latitude = 0;
	
	GPSControl.GPSTime.year = 0;
	GPSControl.GPSTime.month = 0;
	GPSControl.GPSTime.mday = 0;
	GPSControl.GPSTime.hour = 0;
	GPSControl.GPSTime.min = 0;
	GPSControl.GPSTime.sec = 0;
	
	GPSControl.GPSSpeed = 0;
	GPSControl.Course = 0;
	
	GPSControl.Valid  = 0;
	GPSControl.Status = 0;
}
