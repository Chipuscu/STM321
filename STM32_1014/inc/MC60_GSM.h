#ifndef	__MC60_HARDWARE_H__
#define	__MC60_HARDWARE_H__

#include "main.h"
void MC60_GSM_Init(uint32_t Baudrate );
void MC60_HardwareInit(uint32_t Baudrate);
void MC60_IOInit(void);
void MC60_SoftwareInit(void);
void MC60_XoaBoDemNhan(void);
void MC60_GetIMEI(uint8_t Type);

uint8_t MC60_PWROn(void);
uint8_t MC60_CheckConnect(void);
uint8_t MC60_SendCommand(const char* str, const char* response, int32_t TimeOut);
void MC60_SendByte(char Byte);
void MC60_SendBytes(uint8_t *Buffer, uint16_t Length);
void MC60_SendATs(const char* str);

void GSM_Init(void);
void MC60_GSMConfig(void);
void MC60_GetIMEI(uint8_t Type);
void MC60_GPRSConfig(void);
void MC60_GPRS_Init(void);
void MC60_SMS_Init(void);
void SMS_Delete_15SMS(void);

void MC60_Resetstep(void);



/* Dinh nghia cac macro dieu khien module */
#define		MC60_PWKEY_HIGH()			GPIO_SetBits(GPIOC, GPIO_Pin_7)
#define		MC60_PWKEY_LOW()				GPIO_ResetBits(GPIOC, GPIO_Pin_7)

#define		MC60_PWCTRL_HIGH()			GPIO_SetBits(GPIOC, GPIO_Pin_6)
#define		MC60_PWCTRL_LOW()			GPIO_ResetBits(GPIOC, GPIO_Pin_6)

#define  	GSMIMEI				0
#define  	SIMIMEI				1
#define 	OKRes 				"OK\r"



#endif
