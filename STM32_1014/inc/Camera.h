#ifndef __CAMERA_H__
#define __CAMERA_H__


#include "STM32F2xx.h"                  // Device header
#include "Structure_Data.h"
#include "GPS_Process.h"


void UART4_Init(uint32_t Baud);
uint8_t 	Process_data_camera(uint8_t* Buffer,CamDataStruct *Data);
void CAM_process(void);
void Cam_Send(void);
#endif
