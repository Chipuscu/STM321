#include "GPS_Process.h"
#include "main.h"
#include "string.h"
#include "flash.h"
#include "Data.h"
#include "stdlib.h"
#include "LED.h"
#include "Structure_Data.h"
#include "math.h"
#include "RTC.h"


extern SystemVariables          xSystem;
extern GPS_Structure           GPS_xData;
extern GPSControlStruct        GPS_Temp,GPSControl;  //Bien tam trong qua trinh xu ly data
extern ConfStructure           Config;
extern GPS_Structure						GPS;
extern uint8_t 									CONGCOM;

/*******************************************************************************
* Function Name  	: GPS_receidata
* Return         	: 
* Parameters 		: 
* Created by		:
* Date created	: 
* Description		:  Receidata from module GPS
* Notes			: 
************************************************************************************/
void GPS_Receidata(uint8_t Byte)
{
	GPS.count++;
	if(GPS.count>10000)
		GPS.count=0;
	if(Byte == '$')
	{
		GPS.Pointer = 0;
		GPS.Buffer[GPS.Pointer++] = Byte;
	}
	else GPS.Buffer[GPS.Pointer++] = Byte;
	/* Dong goi du lieu GPS vao bo dem */
	if(GPS.Packing == 1)
	{
		GPS.RxBuffer[GPS.RxCounter++] = Byte;
		GPS.RxBuffer[GPS.RxCounter] = 0;
	}
	/* Loc mem, chi nhan ban tin GPRMC, gap ban tin $GPRMC dua con tro ve vi tri dau tien */
	if((GPS.Buffer[0] == '$') && (GPS.Buffer[1] == 'G') && (GPS.Buffer[2] == 'N') && (GPS.Buffer[3] == 'R') && (GPS.Buffer[4] == 'M') &&
		(GPS.Buffer[5] == 'C') && (GPS.Pointer == 6) && (GPS.Packing == 0))
	{
		GPS.RxBuffer[0] = '$';
		GPS.RxBuffer[1] = 'G';
		GPS.RxBuffer[2] = 'N';
		GPS.RxBuffer[3] = 'R';
		GPS.RxBuffer[4] = 'M';
		GPS.RxBuffer[5] = 'C';
	}
	/* Xoa bo loc */
	GPS.Buffer[0] = 0;
	GPS.Buffer[1] = 0;
	GPS.Buffer[2] = 0;
	GPS.Buffer[3] = 0;
	GPS.Buffer[4] = 0;
	GPS.Buffer[5] = 0;
	GPS.RxCounter = 6;
	GPS.IsBaudrateValid = 1;	//Nhan duoc GPRMC thi baud dung
	GPS.Packing = 1;
	if ((GPS.Buffer[0] != '$') &&(GPS.Pointer<6))
	 {
		GPS.IsBaudrateValid = 0;
	 }
	if((Byte == '\r') && (GPS.Packing == 1))
	{
		GPS.Packing = 0;
		xSystem.NewGPSData = 1;
	}
	
	/* Kiem tra cac con tro */
	if(GPS.Pointer >= GPS_RXBODEM)
		 GPS.Pointer = 0;
	if(GPS.RxCounter >= GPS_RXBUFFER)
		GPS.RxCounter = 0;
	
	/* Cau hinh timeout */
	GPS.TimeOutConnection = 5;
}

	/*******************************************************************************
* Function Name  	: 
* Return         	: 
* Parameters 		: 
* Created by		:
* Date created	: 
* Description		:  $GPRMC,203522.00,A,5109.0262308,N,11401.8407342,W,0.004,133.4,130522,0.0,E,D*2B
* Notes			: 
************************************************************************************/
void GPS_Manufacture(void)
{
	char buf[100]={NULL};
	char xGPS[100]={NULL};
	const unsigned char FontHex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	int count=0;
	xGPS[count++]='$';
	xGPS[count++]='G';
	xGPS[count++]='P';
	xGPS[count++]='R';
	xGPS[count++]='M';
	xGPS[count++]='C';
	
	xGPS[count++]=',';
	
	xGPS[count++]=(xSystem.RTCSystem.hour/10)+'0';
	xGPS[count++]=(xSystem.RTCSystem.hour%10)+'0';
	xGPS[count++]=(xSystem.RTCSystem.min/10)+'0';
	xGPS[count++]=(xSystem.RTCSystem.min%10)+'0';
	xGPS[count++]=(xSystem.RTCSystem.sec/10)+'0';
	xGPS[count++]=(xSystem.RTCSystem.sec%10)+'0';
	
	xGPS[count++]=',';
	xGPS[count++]='A';
	xGPS[count++]=',';
	
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]='.';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	
	
	xGPS[count++]=',';
	xGPS[count++]='N';
	xGPS[count++]=',';
	
	xGPS[count++]=(char)(random(1))+'0';
	xGPS[count++]=(char)(random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]='.';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	
	xGPS[count++]=',';
	xGPS[count++]='E'	;
	xGPS[count++]=',';
	
	xGPS[count++]=(char)(random(1))+'0';
	xGPS[count++]=(char)(1+random(1))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]='.';
	xGPS[count++]=(char)(1+random(8))+'0';
	
	xGPS[count++]=',';
	
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]=(char)(1+random(8))+'0';
	xGPS[count++]='.';
	xGPS[count++]=(char)(1+random(8))+'0';

	xGPS[count++]=',';
	xGPS[count++]=(xSystem.RTCSystem.mday/10)+'0';
	xGPS[count++]=(xSystem.RTCSystem.mday%10)+'0';
	xGPS[count++]=(xSystem.RTCSystem.month/10)+'0';
	xGPS[count++]=(xSystem.RTCSystem.month%10)+'0';
	xGPS[count++]=(xSystem.RTCSystem.year/1000)+'0';
	xGPS[count++]=(xSystem.RTCSystem.year%10)+'0';
	xGPS[count++]=',';
	xGPS[count++]='*';

		
	sprintf(buf,"%s",xGPS);
	xGPS[count++]= FontHex[nmea_checksum(buf)/16];
	xGPS[count++]= FontHex[nmea_checksum(buf)%16];
	
	xGPS[count++]='\n';
	xGPS[count++]='\r';
	
	strcpy((char*)GPS.RxBuffer,xGPS);
	sprintf(buf,"%s",xGPS);
	Sendstring(buf);
}

/*******************************************************************************
* Function Name  	: nmea_checksum
* Return         	: checksum
* Parameters 		: 
* Created by		:
* Date created	: 
* Description		:  tao checksum den '*' cua ban tin
* Notes			: 
************************************************************************************/
int nmea_checksum(char *nmea_data)
{
    int crc = 0;
    uint8_t i;

    // the first $ sign and the last two bytes of original CRC + the * sign
    for (i = 1; i < strlen(nmea_data) - 1; i ++) {
        crc ^= nmea_data[i];
    }

    return crc;
}
/*******************************************************************************
* Function Name  	: GPS_Process(
* Return         	: OK/FAIL
* Parameters 		: 
* Created by		:
* Date created	: 
* Description		:  Phan tich ban tin GPS GPRMC
* Notes			: $GNRMC,082221.000,A,2058.9016,N,10548.1004,E,0.47,161.61,170922,,,D*7F
************************************************************************************/
uint8_t GPS_Process(void)
{
	uint8_t Pos=0; // Possition
	char Temp[10]={NULL};
	uint8_t 		i=0;
	float   xTemp,zTemp=0;
	//Incorrect format's news
	if(GPS.RxBuffer[1]!='$'||GPS.RxBuffer[2]!='G'||GPS.RxBuffer[3]!='N'||GPS.RxBuffer[4]!='R'
				||GPS.RxBuffer[5]!='M'||GPS.RxBuffer[6]!='C') return FAIL;
		
	// Copy data sang buffer xu ly, tranh modify data khi process
	CopyDataGPS();
		
	/*   Bo qua ','   */ 
	while(GPS_xData.ProcessBuffer[Pos]!=','&&Pos< GPRMCBUFFER)
	Pos++;
	Pos++; 
	
	// Time
	GPS_Temp.GPSTime.hour=(GPS_xData.ProcessBuffer[Pos]-48)*10+(GPS_xData.ProcessBuffer[Pos+1]-48);
	Pos+=2;
	GPS_Temp.GPSTime.min=(GPS_xData.ProcessBuffer[Pos]-48)*10+(GPS_xData.ProcessBuffer[Pos+1]-48);
	Pos+=2;
	GPS_Temp.GPSTime.sec=(GPS_xData.ProcessBuffer[Pos]-48)*10+(GPS_xData.ProcessBuffer[Pos+1]-48);
	Pos+=2;

	/*   Bo qua ','   */
	while(GPS_xData.ProcessBuffer[Pos] != ',' && Pos < GPRMCBUFFER) 
	Pos++; 
	Pos++;
	
	//Valid or invalid?
	if(GPS_xData.ProcessBuffer[Pos]=='V')
	{
		GPS_Temp.Valid=DATA_INVALID;	
		return FAIL;

	}
	if(GPS_xData.ProcessBuffer[Pos]=='A')
	{
		GPS_Temp.Valid=DATA_VALID;	
//		led_toggle5();
	}
	
	/*   Bo qua ','   */
	while(GPS_xData.ProcessBuffer[Pos] != ',' && Pos < GPRMCBUFFER) 
	Pos++; 
	Pos++;
		
	// Latitude
	GPS_Temp.Latitude=((GPS_xData.ProcessBuffer[Pos]-48)*10+(GPS_xData.ProcessBuffer[Pos+1]-48));
	Pos+=2;
	/*Chuyen doi so thap phan*/
	while(GPS_xData.ProcessBuffer[Pos]!=','&&Pos<GPRMCBUFFER)
	Temp[i++]=GPS_xData.ProcessBuffer[Pos++];
	Pos++;
	if(Pos >= GPRMCBUFFER) return FAIL;
	Temp[i]=0;
	xTemp=(float)atof(Temp);
	/* Công thêm phân` thâp phân*/
	if(GPS_xData.ProcessBuffer[Pos]=='N')
	GPS_Temp.Latitude+=(float)xTemp/60;
	if(GPS_xData.ProcessBuffer[Pos]=='S')
	GPS_Temp.Latitude+=(float)xTemp/60;
	GPS_Temp.Latitude=GPS_Temp.Latitude*(-1);

	/*   Bo qua ','   */
	while(GPS_xData.ProcessBuffer[Pos] != ',' && Pos < GPRMCBUFFER) Pos++;
	Pos++;
	if(Pos >= GPRMCBUFFER) return FAIL;
	
	//Longitude
	memset(Temp,0,10);
	GPS_Temp.Longitude=((GPS_xData.ProcessBuffer[Pos]-48)*100+(GPS_xData.ProcessBuffer[Pos+1]-48)*10
	+(GPS_xData.ProcessBuffer[Pos+2]-48));
	Pos+=3;
	i=0;		
	while(GPS_xData.ProcessBuffer[Pos] != ',' && Pos < GPRMCBUFFER)
	Temp[i++]=GPS_xData.ProcessBuffer[Pos++];
	Pos++;		
	/*Chuyen doi so thap phan*/
	Temp[i]=0;
	xTemp=(float)atof(Temp);
	if(GPS_xData.ProcessBuffer[Pos]=='W')
		GPS_Temp.Longitude+=(float)xTemp/60;
		GPS_Temp.Longitude=GPS_Temp.Longitude*(-1);
	if(GPS_xData.ProcessBuffer[Pos]=='E')
		GPS_Temp.Longitude+=(float)xTemp/60;
	
	/*   Bo qua ','   */
	while(GPS_xData.ProcessBuffer[Pos] != ',' && Pos < GPRMCBUFFER) Pos++;
	Pos++;	

	//Tinh Speed(Knot->Km/h)
	memset(Temp,0,10);
	i=0;
	while(GPS_xData.ProcessBuffer[Pos]!=',')
	{
		if(GPS_xData.ProcessBuffer[Pos]=='.')
		{
			Pos++;
			continue;
		}
		
		Temp[i++]=GPS_xData.ProcessBuffer[Pos++];
		
	}
	zTemp=atoi(Temp);
	zTemp=zTemp*463;
	zTemp=zTemp/25000;
	GPS_Temp.GPSSpeed=(uint8_t)zTemp;

	/*   Bo qua ','   */
	while(GPS_xData.ProcessBuffer[Pos] != ',' && Pos < GPRMCBUFFER)
	Pos++;
	Pos++;
	
	//altitude
	xTemp=((GPS_xData.ProcessBuffer[Pos]-48)*100+(GPS_xData.ProcessBuffer[Pos+1]-48)*10
	+(GPS_xData.ProcessBuffer[Pos+2]-48));
	zTemp=xTemp * 100;
	while(GPS_xData.ProcessBuffer[Pos] != '.' && Pos < GPRMCBUFFER) Pos++;
	Pos++;
	xTemp=(GPS_xData.ProcessBuffer[Pos]-48)*10+(GPS_xData.ProcessBuffer[Pos+1]-48);
	zTemp+=xTemp;
	GPS_Temp.Degree=(uint16_t)zTemp/100;

	// Date
	while(GPS_xData.ProcessBuffer[Pos] != ',' && Pos < GPRMCBUFFER) Pos++;
	Pos++;
	GPS_Temp.GPSTime.mday=(GPS_xData.ProcessBuffer[Pos]-48)*10+(GPS_xData.ProcessBuffer[Pos+1]-48);
	Pos+=2;
	GPS_Temp.GPSTime.month=(GPS_xData.ProcessBuffer[Pos]-48)*10+(GPS_xData.ProcessBuffer[Pos+1]-48);
	Pos+=2;
	GPS_Temp.GPSTime.year=(GPS_xData.ProcessBuffer[Pos]-48)*10+(GPS_xData.ProcessBuffer[Pos+1]-48);
	Pos+=2;
	
	//Copy du lieu vao cau truc chinh', luu du lieu xu ly'
	
	GPSControl.Latitude=GPS_Temp.Latitude;
	GPSControl.Longitude=GPS_Temp.Longitude;
	
	GPSControl.GPSSpeed=GPS_Temp.GPSSpeed;
	
	GPSControl.GPSTime.hour=GPS_Temp.GPSTime.hour;
	GPSControl.GPSTime.min=GPS_Temp.GPSTime.min;
	GPSControl.GPSTime.sec=GPS_Temp.GPSTime.sec;
	GPSControl.GPSTime.mday=GPS_Temp.GPSTime.mday;
	GPSControl.GPSTime.month=GPS_Temp.GPSTime.month;
	GPSControl.GPSTime.year=GPS_Temp.GPSTime.year+2000;
	
	GPSControl.Valid=GPS_Temp.Valid;
	
	GPSControl.Degree=GPS_Temp.Degree;
//	sprintf(x,"Time=%d:%d:%d-%d/%d/%d, Lat= %f, Long= %f, Speed %d\n\r",GPSControl.GPSTime.hour,GPSControl.GPSTime.min,GPSControl.GPSTime.sec,GPSControl.GPSTime.mday,GPSControl.GPSTime.month,GPSControl.GPSTime.year,(double)GPSControl.Latitude,(double)GPSControl.Longitude,GPSControl.GPSSpeed);
//	Sendstring(x);
	
	return OK;
}

/*******************************************************************************
* Function Name  	: 
* Return         	: 
* Parameters 		: 
* Created by		:
* Date created	: 
* Description		: Copy du lieu GPS tu bo de sang bo dem xu ly
* Notes			: 
************************************************************************************/
void CopyDataGPS(void)
{	
	uint16_t i=0;
	// Character orther than "\n" and "\r"
	while(GPS.RxBuffer[i]!= 13 && GPS.RxBuffer[i+1]!=10)  
	{
		GPS_xData.ProcessBuffer[i]=GPS.RxBuffer[i];
		if(++i>=GPS_PROCESSBUFF)
			break;
	}
	GPS_xData.ProcessBuffer[i]=0;
	//Sendstring("ok");
}

/*******************************************************************************
* Function Name  	: 
* Return         	: 
* Parameters 		: 
* Created by		:
* Date created	: 
* Description		: 
* Notes			: 
************************************************************************************/
uint32_t	GetNumberFromString(uint16_t	BeginAddress, char* Buffer)
{
	uint32_t	Value = 0;
	uint16_t	tmpCount = 0;

	tmpCount = 	BeginAddress;
	Value = 0;
	while(Buffer[tmpCount] && tmpCount < 1024)
	{
		if(Buffer[tmpCount]>='0' && Buffer[tmpCount] <= '9')
		{
			Value *= 10;
			Value += Buffer[tmpCount] - 48;	
		}
		else break;

		tmpCount++;
	}	

  	return Value;
}
/*******************************************************************************
* Function Name  	: 
* Return         	: 
* Parameters 		: 
* Created by		:
* Date created	: 
* Description		: Dong' goi' du lieu GPS, truoc khi luu vao Flash
* Notes			: 
************************************************************************************/
void Package_Data_GPS(uint8_t *Data)
{
	uint8_t i=0; 
	union_float xData;
	union_uint32_t zData;
	//Header
	if(GPSControl.GPSTime.sec>15)
	Data[i++]=GPSControl.GPSTime.hour+GPSControl.GPSTime.min;
	else
	Data[i++]=GPSControl.GPSTime.hour+GPSControl.GPSTime.min-1;
	//Time Unix
	GPSControl.UnixTime=RTC_ConvertDateToUnix(&GPSControl);
	//sprintf(buf,"Unix= %d",GPSControl.UnixTime);
	//Sendstring(buf);
	zData.Value=GPSControl.UnixTime;
	Data[i++]=zData.bytes[0];//GPSControl.GPSTime.hour;//
	Data[i++]=zData.bytes[1];//GPSControl.GPSTime.min;//
	Data[i++]=zData.bytes[2];
	Data[i++]=zData.bytes[3];

	//Latitude:*10^6 de? de~ ghi vao Flash
	xData.Value=(GPSControl.Latitude*pow(10,6));
	Data[i++]=xData.byte[0];
	Data[i++]=xData.byte[1];
	Data[i++]=xData.byte[2];
	Data[i++]=xData.byte[3];
	//Longitude
	xData.Value=(GPSControl.Longitude*pow(10,6));
	Data[i++]=xData.byte[0];
	Data[i++]=xData.byte[1];
	Data[i++]=xData.byte[2];
	Data[i++]=xData.byte[3];
	//Speed
	Data[i++]=GPSControl.GPSSpeed&0xFF;
}

/*******************************************************************************
* Function Name  	: 
* Return         	: 
* Parameters 		: 
* Created by		:
* Date created	: 
* Description		: Dong' goi' du lieu GPS, roi luu vao Flash
* Notes			: 
************************************************************************************/
void Send_Data_GPS(uint8_t *Data)
{
	int Add;
	Add=(int)(Caculator_Add2Write16(GPSControl.GPSTime.mday,GPSControl.GPSTime.hour,GPSControl.GPSTime.min,GPSControl.GPSTime.sec));
	FLASH_WriteBuffer(Data,(uint32_t)Add,14);
	memset(Data,0,16);
//	sprintf(buf,"Add= %d,Time=%d:%d:%d-%d/%d/%d, Lat= %f, Long= %f, Speed %d\n\r",Add,GPSControl.GPSTime.hour,GPSControl.GPSTime.min,GPSControl.GPSTime.sec,GPSControl.GPSTime.mday,
//	GPSControl.GPSTime.month,GPSControl.GPSTime.year,(double)GPSControl.Latitude,(double)GPSControl.Longitude,GPSControl.GPSSpeed);
//	Sendstring(buf);
}
/*******************************************************************************
* Function Name  	: 
* Return         	: 
* Parameters 		: 
* Created by		:
* Date created	: 
* Description		: Chuyen doi du lieu sau khi lay ra tu flash
* Notes			: 
************************************************************************************/
void ConvertGPS(ConvDataGPS *Dataout)
{
	//char TimeBuffer[10];
	
	RTC_t *time;
	uint32_t X;
	
	Watchdog_Reset();
	//Header
	Dataout->Header=Config.TxBuffer[0];
	//Time
	X=(uint32_t)(Config.TxBuffer[4]/16*pow(16,7)+Config.TxBuffer[4]%16*pow(16,6)+Config.TxBuffer[3]/16*pow(16,5)+ Config.TxBuffer[3]%16*pow(16,4)
		+Config.TxBuffer[2]/16*pow(16,3)+Config.TxBuffer[2]%16*pow(16,2)+Config.TxBuffer[1]/16*pow(16,1) + (uint32_t)Config.TxBuffer[1]%16);
	
	RTC_ConvertUnixToDate(X,time);
	Dataout->GPSTime.year=time->year;
	Dataout->GPSTime.month=time->month;
	Dataout->GPSTime.mday=time->mday;
	Dataout->GPSTime.hour=time->hour;
	Dataout->GPSTime.min=time->min;
	Dataout->GPSTime.sec=time->sec;
	//latitude
	Dataout->Latitude=(float)((Config.TxBuffer[8]/16*pow(16,7)+Config.TxBuffer[8]%16*pow(16,6)+Config.TxBuffer[7]/16*pow(16,5)+ Config.TxBuffer[7]%16*pow(16,4)
		+Config.TxBuffer[6]/16*pow(16,3)+Config.TxBuffer[6]%16*pow(16,2)+Config.TxBuffer[5]/16*pow(16,1) + (uint32_t)Config.TxBuffer[5]%16)/pow(10,6));
	//Longitude
	Dataout->Longitude=(float)((Config.TxBuffer[12]/16*pow(16,7)+Config.TxBuffer[12]%16*pow(16,6)+Config.TxBuffer[11]/16*pow(16,5)+ Config.TxBuffer[11]%16*pow(16,4)
		+Config.TxBuffer[10]/16*pow(16,3)+Config.TxBuffer[10]%16*pow(16,2)+Config.TxBuffer[9]/16*pow(16,1) + (uint32_t)Config.TxBuffer[9]%16)/pow(10,6));
	//Speed
	Dataout->GPSSpeed=Config.TxBuffer[13];
	
//	sprintf(x,"Speed =%d \n\t",Dataout.GPSSpeed);
//	sprintf(x,"Conv= %d:%d:%d-%d/%d/%d\n\r",Dataout->GPSTime.hour,Dataout->GPSTime.min,Dataout->GPSTime.sec,
//																					Dataout->GPSTime.mday,Dataout->GPSTime.month,Dataout->GPSTime.year);
//	Sendstring(x);
}

/**************************************************************************************
* Function Name  	: Config_KiemTraCacCheDo
* Return         	: None
* Parameters 		: None
* Description		: Xu ly cac che do de test thiet bi
**************************************************************************************/
void Config_CheckFuntion(char *Buffer)
{
	uint8_t Command;
	Command=GetNumberFromString(4,Buffer);
	switch(Command)
	{
		case 1:  //GPS_On//
			Sendstring("**** Hien thi ban tin GPS ****\r");
			GPS.Debug = ENABLE;
			break;
		case 2:  //GPS_On//
			Sendstring("**** Tat ban tin GPS ****\r");
			GPS.Debug = DISABLE;
			break;
		case 3:
			Sendstring("****\rHien thi thoi gian****\r");
			GPS.StatusTime=ENABLE;
			
	}
}

/*******************************************************************************
 * Function Name  	: 
 * Return         	: None
 * Parameters 		: None
 * Description		: Cai dat che do hien thi chuoi nhan duoc hay khong
*******************************************************************************/
void GPS_ProcessData(void)
{
	//ProcessCommand((char*)Config.RxBuffer);
	if(GPS.Debug == ENABLE)
	{
		char buf[100];
		sprintf(buf,"\rGPS: %s",(char*)GPS.RxBuffer);
		Sendstring(buf);
	}
	if(GPS.StatusTime==ENABLE)
	{
	}
}
