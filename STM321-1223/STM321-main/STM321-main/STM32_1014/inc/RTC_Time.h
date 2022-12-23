/*********************************************************************************************************
*
* File                : RTC_Time.h
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

#ifndef _RTC_TIME_H_
#define _RTC_TIME_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

/* Private function prototypes -----------------------------------------------*/
void RTC_Init1(void);
void Time_Display(void);
void Time_Regulate(void);
time_t Time_ConvCalendarToUnix(struct tm t);
struct tm Time_ConvUnixToCalendar(time_t t);
time_t Time_GetUnixTime(void);
struct tm Time_GetCalendarTime(void);
void Time_SetUnixTime(time_t t);
void Time_SetCalendarTime(struct tm t);

/* Private variables ---------------------------------------------------------*/
extern FunctionalState TimeDisplay;

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
