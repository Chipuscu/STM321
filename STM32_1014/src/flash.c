#include "flash.h"
#include "STM32F2xx.h" 
#include "main.h"
#include "LED.h"
#include "UART.h"
#include "InitSystem.h"
#include "Structure_Data.h"


//extern DataStructure BGT;
extern SystemVariables		    	xSystem;
extern GPS_Structure         		GPS_xData;
extern GPSControlStruct       	GPS_Temp,GPSControl;
extern CamStructure							Camera;

/**************************************************************************************
* Function Name  	: ReadStatusFlash
* Return         	: None
* Parameters 		: None
* Description		: Read a byte from Flashs
**************************************************************************************/
uint8_t ReadStatusFlash(void)
{		
	uint8_t Status=0;
	FLASH_Enable();
	SPI_Sendata(0x05);
	Status=SPI_Sendata(0xFF);
	FLASH_Disable();
	return Status;
}


/**************************************************************************************
* Function Name  	: FLASH_Hardware_Init
* Return         	: None
* Parameters 		: None
* Description		: Initialization Flash
**************************************************************************************/
void FLASH_Hardware_Init(void)
{
	/*Khai bao cau truc*/
	GPIO_InitTypeDef GPIOInitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	NVIC_InitTypeDef NVIsC_InitStructure;
	
	/* DeInit SPI1 */
	SPI_I2S_DeInit(SPI1);
	SPI_Cmd(SPI1, DISABLE);
	
	/*Enable Clock*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC,ENABLE);

	/*PA5,PA6,PA7_SCK*/
	GPIOInitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIOInitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_Speed=GPIO_Speed_2MHz ;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIOInitStructure);
	// Connect SPI3 pins to SPI alternate function
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
	/*Khoi tao SPI*/
	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex ;
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master ;
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b ;
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low ;
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge ;
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft ;
	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8 ;
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB ;
	SPI_InitStructure.SPI_CRCPolynomial=7;
	SPI_Init(SPI1,&SPI_InitStructure);
	SPI_CalculateCRC(SPI1, DISABLE);
	SPI_Cmd(SPI1, ENABLE); 
	//Configure /*PA5_NSS*/
	GPIOInitStructure.GPIO_Mode= GPIO_Mode_OUT;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_Pin=GPIO_Pin_5 ;
	GPIOInitStructure.GPIO_Speed=GPIO_Speed_2MHz ;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIOInitStructure);
	//Reset
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOC, &GPIOInitStructure);

		/*Disable Pin_CS Flash */
	FLASH_Disable();
	/*Enable SPI1*/
	GPIO_SetBits(GPIOC, GPIO_Pin_5);

}
/**************************************************************************************
* Function Name  	: SPI_Sendata
* Return         	: None
* Parameters 		: None
* Description		: Send 
**************************************************************************************/
/*Ham gui du lieu SPI*/

static uint8_t SPI_Sendata(uint8_t u8Data)
{
	uint16_t Timeout=1000;
	/* Loop while DR register in not emplty */
	while((SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET)&&Timeout)
		{
			Timeout--;
		}
	if(Timeout==0)
	{
		return 0;
	}
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI1,u8Data);
	/* Wait to receive a byte */
	Timeout=1000;
	while((SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET)&&Timeout)
	{
		Timeout--;
	}
	return SPI_I2S_ReceiveData(SPI1);
}

/**************************************************************************************
* Function Name  	: FLASH_Erase_Sector
* Return         	: None
* Parameters 		: None
* Description		: Erase 4kB
**************************************************************************************/
void FLASH_Erase_Sector(uint32_t Address)
{

	FLASH_EnableWrite();
	//delay1ms();
	FLASH_Enable();
	SPI_Sendata(SectorErase);
	/*Gui 3 byte dia chi*/
	SPI_Sendata((Address >> 16)&0xFF);
	SPI_Sendata((Address >> 8)&0xFF);
	SPI_Sendata(Address&0xFF);
	FLASH_Disable();
	delay_ms(1);
	WaitWriteInProcess();

}
/**************************************************************************************
* Function Name  	: FLASH_Erase_Sector
* Return         	: None
* Parameters 		: None
* Description		: Erase 4kB
**************************************************************************************/
void FLASH_Erase_Block(uint32_t Address)
{

	FLASH_EnableWrite();
	//delay1ms();
	FLASH_Enable();
	SPI_Sendata(SectorErase4b);
	/*Gui 3 byte dia chi*/
	SPI_Sendata((Address >> 16)&0xFF);
	SPI_Sendata((Address >> 8)&0xFF);
	SPI_Sendata(Address&0xFF);
	FLASH_Disable();
	delay_ms(1);
	WaitWriteInProcess();

}

/**************************************************************************************
* Function Name  	: FLASH_Erase_Sector
* Return         	: None
* Parameters 		: None
* Description		: Erase Chip
**************************************************************************************/
void FLASH_Erase(void)
{

	FLASH_EnableWrite();
	FLASH_Enable();
	SPI_Sendata(0xC7);
	FLASH_Disable();
	WaitWriteInProcess();

}
/*Ham cho phep ghi*/
void FLASH_EnableWrite(void)
{
	FLASH_Enable();
	SPI_Sendata(EnableWrite);
	FLASH_Disable();
	
}
/*****************************************************************************************************/
/*Ham ghi mot mang byte vao Flash*/
void FLASH_Writepage(int Address, uint8_t *Buffer,uint16_t Len)
{
	FLASH_EnableWrite();
	FLASH_Enable();
	SPI_Sendata(PageProgram);
	SPI_Sendata((Address >> 16)&0xFF);
	SPI_Sendata((Address >> 8)&0xFF);
	SPI_Sendata(Address&0xFF);
	while(Len--)
	{
		SPI_Sendata(*Buffer);
		Buffer++;
	}
	FLASH_Disable();
	//WaitWriteInProcess();
}
/*****************************************************************************************************/
//void WaitWriteInProcess(void)
//{
//	uint32_t	TimeOut = 0xFFFFFFF;
//	uint8_t		Status = 0;

//	/* Doc thanh ghi */
//	FLASH_Enable();
//	SPI_Sendata(0xD8);
//	while(TimeOut)
//		{
//			Status = (uint8_t)SPI_Sendata(FLASH_SPI_DUMMY);		
//			TimeOut--;
//			if((Status & 1) == 0)
//				break;
//		}
//	FLASH_Disable();
//}
bool WaitWriteInProcess(void)
{
	uint32_t	timeout = 0xFFFFFFF;
	uint8_t		Status;

	while(timeout>0)
	{
		Status=ReadStatusFlash()&0x01;
		if(Status==0)
		{
			return true;
		}
		Delay_ms(1);
		timeout--;
	}
	return false;
}
/*****************************************************************************************************/
/*ham doc mot mang byte*/
void FLASH_ReadBuffer(int Address, uint16_t *Buffer,uint16_t Len)
{
	FLASH_Enable();
	SPI_Sendata(Readdata);
	SPI_Sendata((Address >> 16)&0xFF);
	SPI_Sendata((Address >> 8)&0xFF);
	SPI_Sendata(Address&0xFF);	while(Len--)
	{
		*Buffer=SPI_Sendata(FLASH_SPI_DUMMY);
		Buffer++;
	}
	FLASH_Disable();
}


void FLASH_ReadBuffer8(int Address, uint8_t *Buffer,uint16_t Len)
{
	FLASH_Enable();
	SPI_Sendata(Readdata);
	SPI_Sendata((Address >> 16)&0xFF);
	SPI_Sendata((Address >> 8)&0xFF);
	SPI_Sendata(Address&0xFF);	while(Len--)
	{
		*Buffer=SPI_Sendata(FLASH_SPI_DUMMY);
		Buffer++;
	}
	FLASH_Disable();
}



/*****************************************************************************************************/
void FLASH_ReadBuffer1(uint32_t Address,char *Buffer,uint16_t Len)
{
	FLASH_Enable();
	
	SPI_Sendata(Readdata);
	SPI_Sendata((Address >> 16) & 0xFF);		
	SPI_Sendata((Address >> 8) & 0xFF);
	SPI_Sendata(Address  & 0xFF);
	while(Len--)
	{
		*Buffer = SPI_Sendata(FLASH_SPI_DUMMY);	
		Buffer++;
	}
	FLASH_Disable();
	
}
/*****************************************************************************************************/
void FLASH_ReadBuffer2(uint32_t Address, char *Buffer,uint16_t Len,uint16_t Size)
{
	uint16_t Count,Count1;
	Count=Len/Size;
	Count1=Len%Size;

	if(Count>0)
	{
		while(Count--)
		{
			FLASH_ReadBuffer1(Address,Buffer,Size);
			Address+=Size;
			Buffer+=Size;
			
		}
	}
		FLASH_ReadBuffer1(Address,Buffer,Count1);
}
/*****************************************************************************************************/
uint32_t sFLASH_ReadID(void)
{
	 uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /*!< Select the FLASH: Chip Select low */
  FLASH_Enable();

  /*!< Send "RDID " instruction */
  SPI_Sendata(0x9F);

  /*!< Read a byte from the FLASH */
  Temp0 = SPI_Sendata(sFLASH_DUMMY_BYTE);

  /*!< Read a byte from the FLASH */
  Temp1 = SPI_Sendata(sFLASH_DUMMY_BYTE);

  /*!< Read a byte from the FLASH */
  Temp2 = SPI_Sendata(sFLASH_DUMMY_BYTE);

  /*!< Deselect the FLASH: Chip Select high */
  FLASH_Disable();

  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}
/*****************************************************************************************************/
void FLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t Len)
{
	
  uint16_t Addr = 0, count = 0,CountPage=0;
	int temp = 0;
	Addr = WriteAddr % FLASH_SPI_PAGESIZE;	

	if (Addr+Len<=FLASH_SPI_PAGESIZE) 
	{
			FLASH_Writepage((int)WriteAddr,pBuffer,Len);
			
	}
	else
	{
		count=FLASH_SPI_PAGESIZE-Addr;
		FLASH_Writepage((int)WriteAddr,pBuffer,count); 
		delay_ms(1);
		temp=(int)(Len-count);
		CountPage=temp/FLASH_SPI_PAGESIZE; 
		WriteAddr+=count;					
		pBuffer+=count;
		temp=temp%FLASH_SPI_PAGESIZE; 
		if(CountPage>0)
		{
			while(CountPage--)
			{
				FLASH_Writepage((int)WriteAddr,pBuffer,FLASH_SPI_PAGESIZE);
				pBuffer+=FLASH_SPI_PAGESIZE;
				WriteAddr+=FLASH_SPI_PAGESIZE;
				delay_ms(1);
			}
		}
		FLASH_Writepage((int)WriteAddr,pBuffer,(uint16_t)temp);
	}
}
/*********************************************************************/

/**************************************************************************************
* Function Name  	: mem_set
* Return         	: None 
* Parameters 		: None
* Description		: Thuc hien dat gia tri cho toan bo ham
**************************************************************************************/
void mem_set(void* dst, int val, int cnt) 
{
	char *d = (char*)dst;
	while (cnt--) *d++ = (char)val;
}
/*********************************************************************/
void FLASH_Init(void)
{
//	Sendstring("KhoitaoFlash\n");
	FLASH_Hardware_Init();
	FLASH_EnableWrite();
	
}
/**************************************************************************************
* Function Name  	: Erase_Sectors
* Return         	: None
* Parameters 		: None
* Description		: Thuc hien xoa du lieu theo ngay
**************************************************************************************/
void Erase_Sectors(uint8_t Day)
{
	uint16_t Count1,Count2;
	Sendstring("\rErase Sectors");
	
	//Tinh dia chi bat dau Xoa
	Count1=Day+310;      // Moi ngay 10 Sector
	Count2=Count1+10;
	while(Count1<Count2)
	{
		FLASH_Erase_Sector(Count1*4*1024);
		Count1++;
		delay_ms(250);
	}
}

/**************************************************************************************
* Function Name  	: Cam_Erase_Data
* Return         	: None
* Parameters 		: NumberOfPhoto  : Tên của ảnh tương ứng(1,2,3)
* Description		: Thuc hien xoa du lieu theo ngay
**************************************************************************************/
void Cam_Erase_Data(uint8_t NumberOfPhoto)
{
	uint16_t Count1=0,Count2=0;
	if(NumberOfPhoto<1||NumberOfPhoto>3)
	{
		Sendstring("\rError:Number of Photo ");
	}
	switch(NumberOfPhoto)
	{
		case 1:		// Photo 1
			Sendstring("\rErase Photo 1\r");
			Camera.AddtoWrite=1269761;
			Count1=310; 
			Count2=320;
			break;
		
		case 2:  //Photo 2
			Sendstring("\rErase Photo 2\r");
			Camera.AddtoWrite=1310722;
			Count1=320;
			Count2=330;
			break;
		
		case 3:
			Sendstring("\rErase Photo 3\r");
			Camera.AddtoWrite=1351683;
			Count1=330; 
			Count2=340;
			break;
	}
	while(Count1<Count2)
			{
				FLASH_Erase_Sector(Count1*4*1024);
				Count1++;
				delay_ms(250);
			}
}
