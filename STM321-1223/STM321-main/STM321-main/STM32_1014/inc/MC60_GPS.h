#ifndef __MC60_GPS_H__
#define __MC60_GPS_H__


#include "STM32F2xx.h"                  // Device header



#define		GPS_On()				GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define		GPS_Off()				GPIO_SetBits(GPIOA,GPIO_Pin_1)


static void GPS_SendByte(char Byte);
void GPS_SendString(const char* s);
static void GPS_InitHardware(uint32_t Baudrate);
void GPS_SendCommand(uint8_t Command);
void GPS_SoftwareKhoiTao(void);
void GPS_ResetData(void);
void MC60_GPS_Init(uint32_t Baud);

/* Dinh nghia cac macro dung de gui lenh command */
#define		GPS_GET_VERSION_CMD	          	0
#define		GPS_ONLY_GPRMC_CMD		          1
#define   GPS_ONLY_GPGGA_CMD              2
#define   GPS_ONLY_GPRMC_GPGGA_CMD        3
#define   GPS_DISABLE_OUTPUT_CMD          4
#define		GPS_CTRL_PWR_ON			1
#define		GPS_CTRL_PWR_OFF		2
#define		GPS_CTRL_RST_COLD		3
#define		GPS_CTRL_RST_HOT		4
#define   GPS_RESET_FACTORY   5
#define   GPS_CTRL_RST_WARM   6

#endif
