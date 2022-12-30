#include "flash.h"
#include "STM32F2xx.h" 
#include "main.h"
#include "LED.h"
#include "UART.h"
#include "InitSystem.h"
#include "Structure_Data.h"


extern SystemVariables				 xSystem;
extern ConfStructure           Config;
extern MC60Structure					 GSM;
extern GPS_Structure					 GPS;
extern ConfigUnion           xConfig;
extern Buffer_TypeDef					UART1_Data;
extern CamDataStruct						Cam_Data;
/*******************************************************************************
 * Function Name  	: System_KhoiTaoCacBien
 * Return         	: 
 * Parameters 		:
 * Description		: Khoi tao cac bien dung trong he thong
*******************************************************************************/
 void System_KhoiTaoCacBien(void)
{
	xSystem.Flag10ms       = 0;	
	xSystem.Flag100ms      = 0;	
	xSystem.Flag1000ms     = 0;	
	//xSystem.NumberofPhoto  = 1;
	Cam_Data.Count				 = 1;
	Config.RxCounter       = 0;
	GPS.Debug							 = 0;
	Buffer_Init (&UART1_Data);
	//mem_set(Config.RxBuffer,63,1024);
	sprintf(xConfig.Cauhinh.IP,"tcp.ap.ngrok.io");
	sprintf(xConfig.Cauhinh.PORT,"15284");
}
