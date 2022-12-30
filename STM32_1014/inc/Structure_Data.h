#ifndef __STRUCTURE_DATA_H__
#define __STRUCTURE_DATA_H__


#include "STM32F2xx.h" 
#include "stdio.h"



#define CAM_HEADER_PACKAGE			 0x55
#define CAM_COMMAND_ID					 0x44
#define CAM_COMMAND_SNAPSHOT		 0x48
#define CAM_COMMAND_GET_DATA		 0x45	
#define CAM_COMMAND_BYTE_H			 0x46
#define CAM_COMMAND_BYTE_R			 0x52
#define CAM_FINISH_PACKAGE			 0x23
#define CAM_PACKAGE_SIZE_2			 512
#define CAM_PACKAGE_SIZE_4			 1024
#define CAM_RX_DATA							 1037

#define	BGT_DATA_HEADER				 	 0xAA
#define	BGT_DATA_SIGN					 	 0x55
#define	BGT_BUFFER_LENGTH				 250
#define LenBuffer          			 200
#define GPS_RXBUFFER             512
#define GPS_RXBODEM							 100
#define GPS_PROCESSBUFF   			 512
#define GPS_LENDATA              100
#define	MC60_RX_BUFFER					 200 
#define GSM_RXBUFFER           		512
#define USART1_RX_DATA						4096

//#define		MC60_GPS_RX_BUFFER				2048


/*******************************************/
typedef struct
{
	uint8_t 	RxBuffer[GPS_RXBUFFER];
	uint8_t 	TimeOutReceive;
	uint8_t		Buffer[GPS_RXBODEM];
	uint8_t		Pointer;
	uint16_t 	RxCounter;
	uint8_t		Packing;
	uint8_t		Valid;
	uint8_t 	ProcessBuffer[GPS_PROCESSBUFF];
	uint8_t		TimeOutConnection;
	uint32_t  count;
	uint8_t	  IsBaudrateValid;
	uint8_t	  check_RMC;
	uint8_t		StatusTime;
	uint8_t 	Debug;
	uint8_t  	SaveDataGPS;
	uint16_t  TimeoutSaveData; 
}   GPS_Structure;
/*******************************************/
typedef struct
{
  uint16_t Read;
  uint16_t Write;
  uint8_t Memory[USART1_RX_DATA];
  uint16_t Count;
} Buffer_TypeDef;
/*******************************************/
typedef struct
{
	uint8_t Debug;
	uint16_t RxCounter;
	uint8_t	RxBuffer[MC60_RX_BUFFER];
	uint16_t TimeOutReceive;
	uint32_t TimeOutConnection;
	uint8_t Mode;
	uint16_t TickCounter;
	uint8_t CuongDoTH;
	uint8_t  Tick1000ms;
	/* Trang thai song cua module */
	uint8_t	TOn;		// Thoi gian TOn o chan NetLight
	uint16_t TOff;		// Thoi gian TOff o chan NetLight
	uint8_t	Status;		// Trang thai song cua module
} MC60Structure;
/*******************************************/
typedef struct
{
	uint8_t NumberSendofLogins;  // So lan gui dang nhap
	uint8_t	TimeTickLogins;   // So lan dang nhap
	uint8_t Mode;
}	GPRS_Structure;
/*******************************************/
typedef struct
{	
	char 			Buffer[512];
	uint16_t 	BufferIndex;
	uint8_t   State;
} DataStructure;

/*******************************************/
typedef struct
{ 
	char 		Buffer[200];
	uint8_t 	BufferIndex;
	uint8_t   	State;
} LoginStructure;
/*******************************************/

typedef struct
{ 
    DataStructure	GPRS[512];
    LoginStructure	LoginBuffer;
    uint8_t NewDataToSend;
    uint16_t DataLengthToSend;
	
} GPRSDataStructure;

/*******************************************/
typedef struct 
{
	uint16_t year;        /* 1..4095 */
	uint8_t  month;        /* 1..12 */
	uint8_t  mday;        /* 1..31 */
	uint8_t  wday;        /* 0..6, Sunday = 0*/
	uint8_t  hour;        /* 0..23 */
	uint8_t  min;        /* 0..59 */
	uint8_t  sec;        /* 0..59 */
	uint8_t  dst;        /* 0 Winter, !=0 Summer */
} RTC_t;
/*******************************************/
typedef struct
{
	
	uint32_t Delay_ms;
	char     Buff[LenBuffer];
	uint16_t Buffer[20];
	uint16_t RxCounter;
	uint16_t Rxbuffer[LenBuffer];
	
	uint16_t RxData[LenBuffer];
	uint8_t		Flag10ms;		  /* Co 10ms    */
	uint8_t		Flag100ms;		/* Co 100ms   */
	uint16_t	Flag1000ms;		/* Co 1000ms  */
	uint32_t	Flag2000ms;
	RTC_t     RTCSystem;     /*RTC System*/
	
	uint8_t		NumberofPhoto;
	uint8_t 	NewConfData;
	uint8_t   NewGPSData;
	uint8_t   NewGSMData;
	uint8_t   NewCAMData;
	
	char			GSM_IMEI[20];
	char			SIM_IMEI[20];	
	
	uint8_t   Logined;
	uint16_t TimeNotLogin;
	
	uint8_t GPRSClose;
	int 		ConnectOK;
} 	SystemVariables;
/*******************************************/

typedef struct
{
	uint32_t AddtoWrite;	
	char RxBuffer[CAM_RX_DATA];
	char RxBuffer1[CAM_PACKAGE_SIZE_4];
	char Tx[CAM_PACKAGE_SIZE_4];
	uint16_t TxBuffer[CAM_PACKAGE_SIZE_4];
	uint8_t TimeOutReceive;
	char Buffer[CAM_PACKAGE_SIZE_4];
	uint16_t RxCounter;
} CamStructure;
/*******************************************/
typedef struct
{
	uint16_t RxCounter;
	char RxBuffer[USART1_RX_DATA];
	char RxBuffer1[USART1_RX_DATA];
	char Tx[USART1_RX_DATA];
	uint16_t TxBuffer[USART1_RX_DATA];
	uint8_t TimeOutReceive;
	char Buffer[USART1_RX_DATA];
	
} ConfStructure;
/*******************************************/
typedef struct
{
	uint16_t Header;
	RTC_t		GPSTime;			
	float		Longitude;			
	float		Latitude;
	uint16_t	GPSSpeed;
} ConvDataGPS;

/*******************************************/

/*******************************************/
typedef struct
{
	uint8_t Data[GPS_LENDATA];
	uint32_t UnixTime;
	float		Longitude;			// Kinh do
	float		Latitude;			// Vi do
	RTC_t		GPSTime;			// ThoiGian
	uint16_t	GPSSpeed;			// Van toc GPS
	uint16_t 	Course;				// Huong di chuyen
	uint8_t		Valid;				// Co valid hay khong
	uint8_t		Status;				// Trang thai hoat dong cua module
	uint8_t		Number;				// So ve tinh nhin thay
	float	  Degree;
	float 	HDOP;
	uint8_t		DongCoGPS;
	uint8_t		THS;
	uint8_t		KS;
	float		KinhDo;			// Kinh do
	float		ViDo;			// Vi do
	char    VD[50];
	char    KD[50];
	int ToaDoMD;
} GPSControlStruct;

/*******************************************/
typedef struct
{
	uint8_t Header;
	char BienSo[10];
	char ID1[30];
	char ID[30];
	char IP[30];
	char PORT[10];
}	ConfigDataStruct;

/*******************************************/
typedef struct
{
	uint8_t  Count;
	uint8_t  NumberofPackage;
	uint8_t  ID_Cam;
	uint8_t  Name;
	uint8_t  Command;

	uint8_t  SizePackage;
	uint8_t  Data[CAM_PACKAGE_SIZE_4];
	uint16_t Checksum;
	
} CamDataStruct;


/*******************************************/
typedef union
{
	unsigned long int Value;
	uint8_t byte[4];
}  union_float;
/*******************************************/
typedef union 
{
	uint32_t Value;
	uint8_t bytes[4];
}  union_uint32_t;
typedef union
{
	ConfigDataStruct  Cauhinh;
	char							Byte[sizeof(ConfigDataStruct)];
}ConfigUnion;
#endif
