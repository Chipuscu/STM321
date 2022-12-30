/* Includes */
#include "stm32f2xx.h"
#include "stdarg.h"
#include "string.h"
#include "main.h"
#include "flash.h"
#include "UART.h"
#include "LED.h"
#include "TIM.h"
#include "InitSystem.h"
#include "RTC.h"
#include "GPS_Process.h"
#include "Structure_Data.h"
#include "Data.h"
#include "MC60_GSM.h"
#include "MC60_GPS.h"
#include "GSM_GPRS.h"
#include "debug.h"
#include "GSM_Process.h"
#include "Camera.h"


//#define  BufferSize (countof(Tx_Buffer)-1)
//#define countof(a) (sizeof(a) / sizeof(*(a)))


extern SystemVariables         xSystem;
extern GPS_Structure           GPS_xData;
extern GPSControlStruct        GPS_Temp,GPSControl; 
extern GPS_Structure					 GPS;
extern MC60Structure					 GSM;
extern ConfStructure 					 Config;
extern Buffer_TypeDef					UART1_Data;
extern CamStructure							Camera;
extern CamDataStruct						Cam_Data;



/*******************************************************************************
 * Function Name  	:  delay1ms,delay_ms
 * Return         	: None
 * Parameters 		: None
 * Description		: Ham delay
*******************************************************************************/

/*Ham delay 1ms*/
void delay1ms(void)
{
	TIM_SetCounter(TIM2,0);
	while(TIM_GetCounter(TIM2)<2)
		{}
}

/*Ham delay nms*/
void delay_ms(uint32_t u32DelayInMs)
{
	while(u32DelayInMs)
		{
			delay1ms();
			--u32DelayInMs;
		}
}
/*ham delay theo watchdog*/
void Delay_ms(uint32_t time)
{
	xSystem.Delay_ms=time;
	while(xSystem.Delay_ms)
	{
		Watchdog_Reset();
	}
}

/***************************************************************************************/

int main(void)
{	
	System_Init();
	KhoiTao();
	Sendstring("\rKhoitaoXong!");
	/****************************/
	while(1)
	{	
		Buffer_Handler();

		/* Xu ly tin hieu moi 100ms*/
		if(xSystem.Flag100ms==100)
		{
			xSystem.Flag100ms=0;
			Process_100ms();
			Watchdog_Reset();
		}
		
		/* Co tin hieu lenh moi tu cong Com */
		if(xSystem.NewConfData == 1)
		{
			xSystem.NewConfData = 0;
			Config_Process();
			UART1_Data.Count=0;
			memset(Config.RxBuffer,0,USART1_RX_DATA);
			memset(Config.RxBuffer1,0,USART1_RX_DATA);
			Watchdog_Reset();
		}
		/* Co tin hieu lenh moi tu Camera */
		if(xSystem.NewCAMData == 1)
		{
			xSystem.NewCAMData = 0;
			Camera.RxCounter=0;
			CAM_process();
			memset(Camera.RxBuffer,0,CAM_RX_DATA);
			memset(Cam_Data.Data,0,1024);
			Watchdog_Reset();
		}
		if (Cam_Data.NumberofPackage>0)
		{
			Cam_Data.NumberofPackage--;
			Cam_Send();
			delay_ms(500);
			
		}
		/* Xu ly du lieu nhan duoc tu module GPS */
		if(xSystem.NewGPSData == 1)
		{
			xSystem.NewGPSData = 0;
			GPS_ProcessData();
			Watchdog_Reset();
		}
		/* Xu ly du lieu nhan duoc tu module GSM */
		if(xSystem.NewGSMData == 1)
		{
			xSystem.NewGSMData = 0;
			MC60_ProcessDataGSMReceive();
			Watchdog_Reset();
		}

		/* Xu ly luu du lieu GPS vao Flash*/
		if(GPS.SaveDataGPS==1)
		{
			GPS.SaveDataGPS=0;
			Process_1000ms();
			Watchdog_Reset();
		}
		
	}
}

/*******************************************************************************
 * Function Name  	: Process_1000ms
 * Return         	: None
 * Parameters 		: None
 * Description		: Moi 30s truy cap 1 lan 
*******************************************************************************/
void Process_1000ms(void)
{
	uint8_t Valid;
	Valid=GPS_Process();
	if(Valid)
	{
	Package_Data_GPS(GPSControl.Data);
	Send_Data_GPS(GPSControl.Data);
	}
}
/*******************************************************************************
 * Function Name  	:  GetChecksum
 * Return         	: None
 * Parameters 		: None
 * Description		: Ham checksum: Kiem tra 
*******************************************************************************/
uint16_t GetChecksum(uint8_t *Buffer, uint16_t DataLength)
{
	uint16_t Checksum = 0;
	uint16_t i = 0;
	
	for(i = 0; i < DataLength; i++)
		Checksum += Buffer[i];
	return Checksum;		
}



/*******************************************************************************
 * Function Name  	:  SysTickConfig
 * Return         	: None
 * Parameters 		: None
 * Description		: Ham Cau Hinh Systick
*******************************************************************************/
void SysTickConfig(void)
{	
	/*Setup Systick 1ms interupts*/
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		while(1);
	}
NVIC_SetPriority(SysTick_IRQn,0x0);
}
/*******************************************************************************
 * Function Name  	: System_Initialize
 * Return         	: None
 * Parameters 		: None
 * Description		: Khoi tao phan cung module he thong
*******************************************************************************/
void System_Init(void)
{
	int k;
	SystemInit();
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
	SysTickConfig();
	//Watchdog_Init();
	Delay_ms(2000);
	UART1_Init(115200);
	MC60_GSM_Init(115200);
	UART4_Init(115200);
	FLASH_Init();
	Watchdog_Reset();
	RTC_Init1();
	TIM2_Init();
	TIM3_Init();
	LED_Init();
	for(k=0;k<4;k++)
	{
		FLASH_Erase_Block(k*64*1024);
	}
	//FLASH_Erase();
	//Erase_Sectors(1);
	Watchdog_Reset();
	
}

/*******************************************************************************
 * Function Name  	: KhoiTao
 * Return         	: None
 * Parameters 		: None
 * Description		: Khoi tao hoat dong cho cac module he thong
*******************************************************************************/
void KhoiTao(void)
{
	System_KhoiTaoCacBien();
	//MC60_PWROn();
	//MC60_GPS_Init(9600);
	//GSM_Init();
	//CheckSignalQuality();
	Watchdog_Reset();
	
}
/*******************************************************************************
 * Function Name  	: Process_100ms
 * Return         	: None
 * Parameters 		: None
 * Description		: Xu ly cac thong tin cua MC60
*******************************************************************************/
void Process_100ms(void)
{
	MC60_Process_100ms();
}
/*******************************************************************************
 * Function Name  	: Watchdog_Initialize
 * Return         	: None
 * Parameters 		: None
 * Description		: Khoi tao Internal Watchdog 
*******************************************************************************/
void Watchdog_Init(void)
{
		/* Enable write access to IWDG_PR and IWDG_RLR registers */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	/* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	
	/* Set counter reload value to 349 */
	IWDG_SetReload(0xFFF);
	
	/* Reload IWDG counter */
	IWDG_ReloadCounter();
	
	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	IWDG_Enable();
	
	/* Khoi tao watchdog ngoai */
	//ExternalWatchdog_Initialize();
}
/*******************************************************************************
 * Function Name  	: Buffer_Init
 * Return         	: None
 * Parameters 		: None
 * Description		: Khoi tao buffer nhan du lieu
*******************************************************************************/
void Buffer_Init(Buffer_TypeDef *Buffer)
{
	Buffer->Read=0;
	Buffer->Write=0;
	Buffer->Count=0;
}
/*******************************************************************************
 * Function Name  	: Buffer_Init
 * Return         	: None
 * Parameters 		: None
 * Description		: Khoi tao buffer nhan du lieu
*******************************************************************************/
static void Buffer_Handler(void)
{
	while(UART1_Data.Read!=UART1_Data.Write)
	{
		uint8_t byteRead=Ring_Buffer_Read(&UART1_Data);

		Config.RxBuffer[UART1_Data.Count++]=byteRead;
	}
	
}

/*******************************************************************************
 * Function Name  	: Watchdog_Reset
 * Return         	: None
 * Parameters 		: None
 * Description		: Dua gia tri Reset watchdog ve mac dinh
*******************************************************************************/

void Watchdog_Reset(void)
{
	IWDG_ReloadCounter();
}
