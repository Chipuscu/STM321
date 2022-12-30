#ifndef __GPS_PROCESS_H_
#define __GPS_PROCESS_H_

#include "STM32F2xx.h" 
#include "flash.h"
#include "Structure_Data.h"


#define FAIL        						 0
#define OK							 				 ~0;


#define	GPRMCBUFFER							 80
#define DATA_INVALID             0
#define DATA_VALID               1
void GPS_Manufacture(void);
uint8_t GPS_Process(void);
uint32_t	GetNumberFromString(uint16_t	BeginAddress, char* Buffer);
void CopyDataGPS(void);
int nmea_checksum(char *nmea_data);
void Package_Data_GPS(uint8_t *Data);
void Send_Data_GPS(uint8_t *Data);
void ConvertGPS(ConvDataGPS *Dataout);
void GPS_Receidata(uint8_t Byte);
void Config_CheckFuntion(char *Buffer);
void GPS_ProcessData(void);
#endif
