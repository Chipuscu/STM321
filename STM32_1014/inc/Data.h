#ifndef __DATA_H_
#define __DATA_H_
#include "STM32F2xx.h" 

#define   Dulieu1ngay 86400 
void ProcessCommand(char* CommandBuffer);
int16_t	TimViTriKyTu(char ch, char *Buffer);
int16_t	TimViTriKyTu2(char ch, char *Buffer);
uint8_t TachDuLieu(char* String, char* Buffer,char start, char end);
uint8_t TachDuLieu2(char* String, char* Buffer,char start, char end);
int random(int max);
uint32_t Caculator_Add2Write16(uint8_t Day, uint8_t Hour, uint8_t Min, uint8_t Sec);

uint32_t Caculator_Add2Write(uint8_t Day, uint8_t Hour, uint8_t Min, uint8_t Sec);
#endif
