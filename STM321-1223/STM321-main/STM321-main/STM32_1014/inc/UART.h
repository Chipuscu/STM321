#ifndef __UART_H_
#define __UART_H_

#include "STM32F2xx.h" 
#include "Structure_Data.h"

void UART1_Init(uint32_t Baud);
void USART_TR(uint16_t Buff);
void USART1_IRQHandler(void);
void USART_ReceiData1(uint16_t Data);
void Hardware_XoaCoLoi(void);
void MC60_GSM_ReceiData(char Data);
void MC60_GPS_ReceiData(char Data);
void MC60_GSM_ReceiData1(char Data);
void USART1_ReceiData(char Data);
void Ring_Buffer_Write(Buffer_TypeDef *Buffer,uint8_t Data);
uint8_t Ring_Buffer_Read(Buffer_TypeDef *Buffer);
void UART4_ReceiData(char Data);
void Sendstring4s(uint8_t *s, uint8_t Len);

void CAM_SendByte1(uint16_t Byte);
void Sendstring4(const char *s, uint8_t Len);
//void Send14(uint16_t Data[255],uint16_t Len);
void CAM_SendByte(char Byte);
void Send1(uint8_t Data[255],uint16_t Len);
void Send2(uint16_t Data[255],uint16_t Len);
void Send3(uint16_t Data[255],uint16_t Len);
#endif
