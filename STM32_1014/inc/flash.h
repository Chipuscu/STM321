#ifndef __FLASH__H
#define __FLASH__H

#include "STM32F2xx.h"  

void FLASH_Init(void);
void FLASH_Hardware_Init(void);
//uint16_t SPI_Sendata(uint16_t u8Data);
//void SPI_ReceiveData(uint16_t u8Data);
void FLASH_Erase_Sector(uint32_t Address);

 uint8_t SPI_Sendata(uint8_t u8Data);
void FLASH_EnableWrite(void);

void GhiDuLieuTungGiay(uint8_t* InputBufferSpeed,uint8_t Hour, uint8_t Minute);
uint16_t ReadStatusFlash(void);
uint32_t sFLASH_ReadID(void);
void SPI1_IRQHandler(void);

void FLASH_ReadBuffer(int Address, uint16_t *Buffer,uint16_t Len);
void FLASH_ReadBuffer1(uint32_t Address,char *Buffer,uint16_t Len);
void FLASH_ReadBuffer2(uint32_t Address, char *Buffer,uint16_t Len,uint16_t Size);
void FLASH_ReadBuffer8(int Address, uint8_t *Buffer,uint16_t Len);
void FLASH_ReadBuffer32(int Address, uint32_t *Buffer,uint16_t Len);
void Erase_Buffer(void);

void mem_set(void* dst, int val, int cnt) ;
void Erase_Sectors(uint8_t Day);
void Cam_Erase_Data(uint8_t NumberOfPhoto);
void FLASH_Writepage(int Address, uint8_t *Buffer,uint16_t Len);
void float2Bytes(uint8_t * ftoa_bytes_temp,float float_variable);
void Flash_Write_NUM (int StartSectorAddress, float Num);
float Flash_Read_NUM (int StartSectorAddress);
float Bytes2float(uint8_t * ftoa_bytes_temp);
void FLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t Len);
void WaitWriteInProcess(void);
void HexToString(char *Buffer, uint16_t *cs, char Number);

#define FLASH_Disable() GPIO_SetBits(GPIOC,GPIO_Pin_5)
#define FLASH_Enable() GPIO_ResetBits(GPIOC,GPIO_Pin_5)
#define EnableWrite 0x06
#define SectorErase4b 0xD8
#define PageProgram 0x02
#define Readdata 0x03
#define FLASH_SPI_DUMMY 0x00
#define FLASH_DATA_BASE 65536
/* Kich thuoc cac vung du lieu */
#define	DAY_DATA_LEN  86400
#define DULIEU_TUNGGIAY_LENGTH				63
#define sFLASH_DUMMY_BYTE    0xA3
#define sFlASH_ID  0x00EF4017
#define FLASH_SPI_PAGESIZE 256
#endif

