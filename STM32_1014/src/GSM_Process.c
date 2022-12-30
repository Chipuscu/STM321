#include "main.h"
#include "GSM_Process.h"
#include "string.h"
#include "Structure_Data.h"
#include "MC60_GSM.h"

extern SystemVariables          xSystem;
extern GPS_Structure           GPS_xData;
extern GPSControlStruct        GPS_Temp,GPSControl; 
extern GPS_Structure						GPS;
extern MC60Structure					 GSM;
extern ConfStructure 					 Config;




void MC60_ProcessDataGSMReceive(void)
{
	//Watchdog_Reset();
	if(strstr((char*)GSM.RxBuffer,"CONNECT OK"))
	{
		xSystem.ConnectOK=1;
		return;
	}
}
