#include "debug.h"
#include "main.h"
#include "string.h"
#include "Structure_Data.h"
#include "GPS_Process.h"
#include "data.h"
#include "stdlib.h"
#include "MC60_GSM.h"
#include "stdio.h"
#include "UART.h"
#include "LED.h"
#include "SPIFFS.h"


extern GPS_Structure				GPS;
extern ConfStructure  				Config;
extern ConvDataGPS            		DataGPSConverted;
extern CamDataStruct				Cam_Data;



/* crc table for calculate crc */
const uint16_t crctab16[] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};
/**************************************************************************************
* Function Name  	: Config_Process
* Return         	: None
* Parameters 		: None
* Description		: Xu ly du lieu nhan duoc tu cong Com
**************************************************************************************/
uint8_t Config_Process(void)
{
	static uint32_t Add=0;
	char Address[10],buf[2048],D[5],NoP[14],Number[1],IP[10],send[50];						//	Nop : Number of Photo (1,2,3)
	int xSec,xMin,xHour,xDay,i=0,j=0,Count=0;
	uint8_t Command[14],Data[4096];
	uint16_t c,xData[2];
	uint32_t AddtoRead;
	File_structure File;
	File=Init();
	

	/**********************************************/
	if(strstr((char*)Config.RxBuffer, "COM")!=NULL)
	{
		Count=1;
		Config_CheckFuntion((char*)Config.RxBuffer);
	}
	
	/**********************************************/
	if(strstr((char*)Config.RxBuffer,"RADD")!=NULL)
	{
		Count=1;
		mem_set(Address,0,10);
		TachDuLieu((char*)Config.RxBuffer,Address,'(',')');
		AddtoRead = atoi((const char*)Address);
		FLASH_ReadBuffer(AddtoRead,Config.TxBuffer,1024);
		Send2(Config.TxBuffer,1024);
	}
	
	/**********************************************/
	if(strstr((char*)Config.RxBuffer,"RADE")!=NULL)
	{
		Count=1;
		mem_set(Address,0,10);
		TachDuLieu((char*)Config.RxBuffer,Address,'(',')');
		AddtoRead = atoi((const char*)Address);
		FLASH_ReadBuffer8(AddtoRead,Data,4096);
		Sendstring1((char *)Data,4096);
	}
	
	/**********************************************/
	if(strstr((char*)Config.RxBuffer,"AT")!=NULL)
	{
		Count=1;
		MC60_SendCommand("AT+QISEND\r",OKRes,1000);
		MC60_SendATs((char*)GPS.RxBuffer);
		MC60_SendATs("\x1A");
	}
	
	/**********************************************/
	if(strstr((char*)Config.RxBuffer,"SEND"))
	{
		Count=1;
		FLASH_WriteBuffer((uint8_t * )Config.RxBuffer,Add,1024);
		delay_ms(40);
		
		sprintf(buf,"Add= %d \r",Add);
		Sendstring(buf);

	}
	
	/**********************************************/
	if(strstr((char*)Config.RxBuffer,"READ"))
	{
		Count=1;
		mem_set(Address,0,10);
		TachDuLieu((char*)Config.RxBuffer,Address,'(',')');
		AddtoRead = atoi((const char*)Address);
		FLASH_ReadBuffer(AddtoRead,(uint16_t *)Config.TxBuffer,1024);
		delay_ms(2);
		Sendstring("\r");
		for(i=0; i<1024;i++)
		{
			sprintf(buf,"%02X",Config.TxBuffer[i]);
			Sendstring(buf);
		}
		return 0;
	}	
	
	/**********************************************/

	if(strstr((char*)Config.RxBuffer,"CAMIP"))
	{
		TachDuLieu(Config.RxBuffer,IP,'(',')');
		hex_decode(IP,10,(uint8_t *)Command);
		Sendstring4s((uint8_t *)Command,5);
		
	}
	/**********************************************/
	if(strstr((char*)Config.RxBuffer,"TakeaPhoto"))
	{
		TachDuLieu(Config.RxBuffer,NoP,'(',',');
		TachDuLieu(Config.RxBuffer,Number,',',')');
		Cam_Data.Name=atoi(Number);
		Cam_Erase_Data(Cam_Data.Name);
		hex_decode(NoP,14,Command);
		Sendstring4s((uint8_t *)Command,7);
	}
	if(strstr((char*)Config.RxBuffer,"GetaPhoto"))
	{
		TachDuLieu(Config.RxBuffer,Number,'(',')');
		Cam_Data.Name=atoi(Number);
		FLASH_ReadBuffer((300+Cam_Data.Name*10)*4*1024+Cam_Data.Name-1,(uint16_t *)Number,1);
		while(Number[0]--)
		{
			FLASH_ReadBuffer((300+Cam_Data.Name*10)*4*1024+Cam_Data.Name+j,(uint16_t *)Config.TxBuffer,1024);
			j+=1024;
			delay_ms(2);
			for(i=0; i<1024;i++)
			{
				sprintf(buf,"%02X",Config.TxBuffer[i]);
				Sendstring(buf);
			}
		}
	}
	if(strstr((char*)Config.RxBuffer,"Create"))
	{
		File.xCreate("HN.Capital");
	}
	if(strstr((char*)Config.RxBuffer,"HCM"))
	{
		File.xCreate("HCM.city");
	}
	if(strstr((char*)Config.RxBuffer,"Write"))
	{
		uint8_t  Data1[]=("In Vietnam, there are many special gestures and customs. First of all, in terms of gestures, Vietnamese people greet each other by shaking hands. This is also a polite way of greeting that many people use. When meeting young people, they greet with a slight nod and when meeting elderly people, they often fold their arms and bow. When we meet friends, we often raise our hands. Vietnamese people often exchange business cards in the first meeting. When handing business cards to each other, they often use two hands to show respect for each other. In addition, Vietnamese people often talk about daily life, job, food or interest in small talk. They should avoid sensitive topics for small talk such as age, salary because it can make the others unpleasant. In short, we should keep and pass down our customs through the generations.");
		File.xWrite("HN.Capital",Data1,800);
	}
	if(strstr((char*)Config.RxBuffer,"Read"))
	{
		File.xRead("HN.Capital");
	}
	if(strstr((char*)Config.RxBuffer,"R_Address"))
	{
		memset(buf,NULL,2048);
		Read_Address("HN.Capital",(uint8_t *)buf,3900,1000);
		Sendstring1(buf,1000);
	}
	if(strstr((char*)Config.RxBuffer,"Listfile"))
	{
		Listfile();
	}
	if(strstr((char*)Config.RxBuffer,"SearchFile"))
	{
		Search_File("HN.Capital");
	}
	if(strstr((char*)Config.RxBuffer,"Erase"))
	{
		File.xErase("HN.Capital");
	}
}
/**************************************************************************************
* Function Name  	: hex_code
* Return         	: None
* Parameters 		: None
* Description		: Convert char(Hex) to Hex
**************************************************************************************/
void hex_decode(const char *in, uint32_t len,uint8_t *out)
{
        unsigned int i, t, hn, ln;
        for (t = 0,i = 0; i < len; i+=2,t++) 
				{
                hn = in[i] > '9' ? in[i] - 'A' + 10 : in[i] - '0';
                ln = in[i+1] > '9' ? in[i+1] - 'A' + 10 : in[i+1] - '0';
                out[t] = (hn << 4 ) | ln;
        }
        return ;
}
/**************************************************************************************
* Function Name  	: CRC_Checksum- Pakage _Camera
* Return         	: None
* Parameters 		: None
* Description		: Checksum CRC 16
*************************************************************************************/
uint16_t GetCrc16(const uint8_t* pData, uint32_t nLength){
    uint16_t fcs = 0xffff; // Initialize
    while(nLength>0){
        fcs = (fcs >> 8) ^ crctab16[(fcs ^ *pData) & 0xff];
        nLength--;
        pData++;
    }
    return ~fcs;
}

