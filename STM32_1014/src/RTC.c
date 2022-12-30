#include "STM32F2xx.h" 
#include "stm32f2xx_it.h"
#include "main.h"
#include "flash.h"
#include "RTC.h"
#include "stm32f2xx_rtc.h"              // Keil::Device:StdPeriph Drivers:RTC
#include "Structure_Data.h"
#include <time.h>


extern SystemVariables          xSystem;
extern GPS_Structure           xData_GPS;
extern GPSControlStruct        GPS_Temp,GPSControl;
RTC_t                            temptDatatime;
RTC_InitTypeDef				RTC_InitStructure;
RTC_TimeTypeDef				RTC_TimeStruct;
RTC_DateTypeDef				RTC_DateStruct;

/*******************************************************************************
* Function Name  : Time_ConvUnixToCalendar
* Description    : 
* Input          : 
* Output         : None
* Return         : struct tm
* Attention		 : None
*******************************************************************************/
void RTC_ConvertUnixToDate(time_t Unix, RTC_t *Date)
{
	char buf[50];
	uint32_t a;
	struct tm *ThoiGian;
	time_t localtime=Unix;
	a= localtime;
	sprintf(buf,"X= %d",a);
	Sendstring(buf);
	Date->month = ThoiGian->tm_mon + 1;
	Date->mday = ThoiGian->tm_mday;
	Date->hour = ThoiGian->tm_hour;
	Date->min = ThoiGian->tm_min;
	Date->sec = ThoiGian->tm_sec;
}


/*******************************************************************************
* Function Name  : Time_ConvCalendarToUnix
* Description    : 
* Input          : - t: struct tm
* Output         : None
* Return         : time_t
* Attention		 : None
*******************************************************************************/
time_t RTC_ConvertDateToUnix(GPSControlStruct *ThoiGian)
{
	/* Khai bao cac bien */
	time_t UnixTime;
	struct tm Time;
	
	/* Lay thoi gian hien tai */	
	Time.tm_year = ThoiGian->GPSTime.year - 1900;
	Time.tm_mon = ThoiGian->GPSTime.month - 1;
	Time.tm_mday = ThoiGian->GPSTime.mday;
	Time.tm_hour = ThoiGian->GPSTime.hour;
	Time.tm_min = ThoiGian->GPSTime.min;
	Time.tm_sec = ThoiGian->GPSTime.sec;

	UnixTime = mktime(&Time);

	return UnixTime;
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : RTC_Configuration(
* Description    : Configures the RTC.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
 void RTC_Configuration(void)
{
	uint32_t RTC_TimeOut = 0;
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR , ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
 	RCC_BackupResetCmd(ENABLE);
	RCC_BackupResetCmd(DISABLE);

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
	RTC_TimeOut = 1000;
  while ((RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)&&RTC_TimeOut==1000)
				RTC_TimeOut--;
	if(RTC_TimeOut == 0)
	{}
		
  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Configure the RTC data register and RTC prescaler */
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
	RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_12;
	RTC_Init(&RTC_InitStructure);

	RCC_RTCCLKCmd(ENABLE);
	
	RTC_WaitForSynchro();
	
	/* Enable The TimeStamp */
	RTC_TimeStampCmd(RTC_TimeStampEdge_Falling, ENABLE); 
}
void Time_Regulate(void)
{
//	char Buf[100];
  RTC_t time;
//  memset(&time, 0 , sizeof(time) );
//	Sendstring((char* )Buf);
	/* Set Time hh:mm:ss */
	RTC_TimeStruct.RTC_H12=RTC_H12_AM;
	RTC_TimeStruct.RTC_Hours   = 16;  
	RTC_TimeStruct.RTC_Minutes = 52;
	RTC_TimeStruct.RTC_Seconds = 0;
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
	/* Set Date Week/Date/Month/Year */
	//RTC_DateStruct.RTC_WeekDay = 7;
	RTC_DateStruct.RTC_Date = 14;
	RTC_DateStruct.RTC_Month = 6;
	RTC_DateStruct.RTC_Year = 22;
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);
	
  time.year=2022;
  time.month=9;
  time.mday=26;
  time.hour=12;
  time.min=00;
  time.sec=00;
	
	rtc_settime(&time);  
	RTC_WriteBackupRegister(RTC_BKP_DR0, 0xA5A5);
}


/*******************************************************************************
* Function Name  : KhoitaoRTC
* Description    : Khoi tao module RTC
* Input          : None
* Output         : true neu khoi tao thanh cong, nguoc lai thi la 0
*******************************************************************************/
void RTC_Init1(void)
{
	Sendstring("\rKhoi Tao Module RTC: ");
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0xA5A5)
	{
	//NVIC_Configuration();
  RTC_Configuration();
	Time_Regulate();
	Sendstring("[NO]\n\t");
	}
	else
	{
	Sendstring("[YES]\n\t");
	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);

	/* Wait for RTC APB registers synchronisation */
	RTC_WaitForSynchro();
	}
  return;
}


void Time_Display(void)
{
    RTC_t time;
	xSystem.RTCSystem.sec=(uint8_t)time.sec;
	xSystem.RTCSystem.min=(uint8_t)time.min;
	xSystem.RTCSystem.hour=(uint8_t)time.hour;
	xSystem.RTCSystem.mday=(uint8_t)time.mday;
	xSystem.RTCSystem.month=(uint8_t)time.month;
	xSystem.RTCSystem.year=(uint16_t)time.year;
	
}

/*******************************************************************************
* Function Name  : rtc_settime
* Description    : sets HW-RTC with values from time-struct, takes DST into
*                  account, HW-RTC always running in non-DST time
* Input          : None
* Output         : None
* Return         : not used
*******************************************************************************/
bool rtc_settime (RTC_t *rtc)
{
	RTC_TimeTypeDef		Time;
	RTC_DateTypeDef		Date;
	
	/* Ghi gio vao trong RTC */
	Time.RTC_Hours = rtc->hour;
	Time.RTC_Minutes = rtc->min;
	Time.RTC_Seconds = rtc->sec;
	RTC_SetTime(RTC_Format_BIN, &Time);

	/* Ghi ngay vao trong RTC */	
	Date.RTC_Year = rtc->year - 2000;
	Date.RTC_WeekDay = 7;
	Date.RTC_Month = rtc->month;
	Date.RTC_Date = rtc->mday;
	RTC_SetDate(RTC_Format_BIN, &Date);
	
	return true;
}
/*******************************************************************************
* Function Name  : rtc_gettime
* Description    : populates structure from HW-RTC, takes DST into account
* Input          : None
* Output         : time-struct gets modified
* Return         : always true/not used
*******************************************************************************/
void rtc_gettime (RTC_t *rtc)
{
	RTC_TimeTypeDef		Time;
	RTC_DateTypeDef		Date;
	
	/* Doc thoi gian */
	RTC_GetTime(RTC_Format_BIN, &Time);
	/* Doc ngay */
	RTC_GetDate(RTC_Format_BIN, &Date);
	
	/* Chuyen sang gio he thong */
	rtc->year = Date.RTC_Year + 2000;
	rtc->month = Date.RTC_Month;
	rtc->mday = Date.RTC_Date;
	rtc->hour = Time.RTC_Hours;
	rtc->min = Time.RTC_Minutes;
	rtc->sec = Time.RTC_Seconds;
	return ;
}
/*******************************************************************************
* Function Name  : RTC_ProcessSystem
* Description    : Ham nay su dung de xu ly thoi gian cua he thong
* Input          : None
* Output         : None
*******************************************************************************/
void RTC_ProcessSystem(void)
{
	/* Doc thoi gian vao bo dem */
	rtc_gettime(&temptDatatime);
	
	xSystem.RTCSystem.sec=temptDatatime.sec;
	xSystem.RTCSystem.min=temptDatatime.min;
	xSystem.RTCSystem.hour=temptDatatime.hour;
	xSystem.RTCSystem.mday=temptDatatime.mday;
	xSystem.RTCSystem.month=temptDatatime.month;
	xSystem.RTCSystem.year=temptDatatime.year;

}


