#ifndef __MAIN_H
#define  __MAIN_H

#include "STM32F2xx.h" 
#include "stdio.h"
#include "Structure_data.h"

void Delay_ms(uint32_t time);
void delay1ms(void);
void delay_ms(uint32_t u32DelayInMs);
void Sendbyte(char byte);
void Sendstring(const char *s);
void USART1_IRQhandler(void);
void USART1_InterruptHandler(void);
char UARTx_Getc(USART_TypeDef* USARTx);
void DebugPrint(const char* str, ...);
void DebugPut(char Byte);
void DebugItoa(long val, int radix, int len);
void DebugPuts(const char* s);
uint16_t GetChecksum(uint8_t *Buffer, uint16_t DataLength);
void SysTickConfig(void);
void Sendbyte1(uint16_t ch);
void Send(char * Data);
//void Send1(uint16_t Data[255],uint16_t Len);
void Sendstring1(char *s,uint16_t Len);
//static void XuLySuKien_100ms(void);
void System_Init(void);
void KhoiTao(void);
void Process_1000ms(void);
void Process_100ms(void);
void Watchdog_Reset(void);
void Watchdog_Init(void);
void Buffer_Init(Buffer_TypeDef *Buffer);
void Buffer_Handler(void);
#endif
