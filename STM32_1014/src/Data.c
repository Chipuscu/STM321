#include "main.h"
#include "string.h"
#include "LED.h"
#include "Data.h"
#include "flash.h"
#include "stdlib.h"
#include "Structure_Data.h"
#include "GPS_Process.h"
#include "time.h"

extern SystemVariables          xSystem;
extern ConvDataGPS              DataGPSConvert;
extern GPSControlStruct        GPS_Temp,GPSControl;

/**************************************************************************************
* Function Name  	: 
* Return         	: None
* Parameters 		: None
* Description		:  
**************************************************************************************/
uint8_t TachDuLieu(char* String, char* Buffer,char start, char end)
{
	int16_t	Address1= TimViTriKyTu(start, String);
	int16_t	Address2 = TimViTriKyTu(end,String);
	int16_t	tmpCount,i = 0;

	/* Kiem tra dinh dang du lieu */
	if((Address1 == -1) || Address2 == -1)
		return 0;
	if(Address2 - Address1 <= 1)
		return 0;

	for(tmpCount =Address1 + 1; tmpCount < Address2; tmpCount++)
	{
		Buffer[i++] = String[tmpCount];	
	}
		
	return 1;	
		
}

/**************************************************************************************
* Function Name  	: 
* Return         	: None
* Parameters 		: None
* Description		: 
**************************************************************************************/
int16_t	TimViTriKyTu(char ch, char *Buffer)
{
	uint8_t		tmpCount = 0;

	/* Do dai du lieu */

	for(tmpCount = 0; tmpCount < strlen(Buffer); tmpCount ++)
	{
		if(Buffer[tmpCount] == ch)
			return tmpCount;
	}
	return -1;
}
/**************************************************************************************
* Function Name  	: 
* Return         	: None
* Parameters 		: None
* Description		: 
**************************************************************************************/
int16_t	TimViTriKyTu2(char ch, char *Buffer)
{
	uint8_t tmpCount;
	int i=0;
	for(tmpCount=0;tmpCount < strlen(Buffer);tmpCount++)
	{
		if(Buffer[tmpCount]==ch)
		{
			i=i+1;	
		}
		if(i==2)
			break;
	}
	return (Buffer[tmpCount]);
}

/**************************************************************************************
* Function Name  	: 
* Return         	: None
* Parameters 		: None
* Description		: 
**************************************************************************************/
int random(int max)
{
	int i=0;
//	srand(xSystem.RTCSystem.sec);
	i=(rand()-xSystem.RTCSystem.sec);
	return(i%max);
}
/**************************************************************************************
* Function Name  	: Caculator_Add2Write
* Return         	: None
* Parameters 		: None
* Description		: Tính toán địa chỉ cuối cùng của dữ liệu, thời gian 30s/lần, mỗi lần 31 byte
**************************************************************************************/
uint32_t Caculator_Add2Write(uint8_t Day, uint8_t Hour, uint8_t Min, uint8_t Sec)
{
	uint32_t AddtoWrite;
	if(Sec<30)
	{
	AddtoWrite=220+(Day-1)*90112+(Hour*60+Min)*62; // 3byte dau ghi D/M/Y,90112 la so byte cua 22 sectors
	}        
	else
	{
	AddtoWrite=220+(Day-1)*90112+Hour*60*62+Min*62+31;  
	}
	return(AddtoWrite);
}
/**************************************************************************************
* Function Name  	: 
* Return         	: None
* Parameters 		: None
* Description		: 
**************************************************************************************/
uint32_t Caculator_Add2Write16(uint8_t Day, uint8_t Hour, uint8_t Min, uint8_t Sec)
{
	uint32_t AddtoWrite;
	if(Sec<30)
	{
	AddtoWrite=3+(Day-1)*40960+(Hour*60+Min)*28; // 3byte dau ghi D/M/Y, 40960  la so byte cua 10 sectors
	}        
	else
	{
	AddtoWrite=3+(Day-1)*40960+(Hour*60+Min)*28+14;   
	}
	return(AddtoWrite);
}
