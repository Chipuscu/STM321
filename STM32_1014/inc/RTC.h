#ifndef _RTC_TIME_H_
#define _RTC_TIME_H_

/* Includes ------------------------------------------------------------------*/
#include "STM32F2xx.h" 
#include "time.h"
#include <stdio.h>
#include <string.h>
#include "stdbool.h"
#include "stm32f2xx_rcc.h"
#include "Structure_Data.h"
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
void rtc_gettime (RTC_t *rtc);
void RTC_ProcessSystem(void);
time_t RTC_ConvertDateToUnix(GPSControlStruct *ThoiGian);
bool rtc_settime (RTC_t *rtc);

void RTC_ConvertUnixToDate(uint32_t time, RTC_t *dt);
/* Private variables ---------------------------------------------------------*/
extern FunctionalState TimeDisplay;

#endif
