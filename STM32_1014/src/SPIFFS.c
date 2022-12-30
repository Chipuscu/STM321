#include "STM32F2xx.h"                  // Device header
#include "main.h"
#include "SPIFFS.h"
#include "flash.h"
#include "string.h"
#include "Structure_Data.h"
#include "UART.h"



/*******************************************************************************
 * Function Name  	: 
 * Return         	: 
 * Parameters 			:
 * Description			: 
*******************************************************************************/
File_structure Init()//const char* filename)
{
	struct File_structure File={Create,Write,Read,Erase};
	//File.xCreate(filename);
	return File;
}

/*******************************************************************************
 * Function Name  	: 
 * Return         	: 
 * Parameters 			:
 * Description			: 
*******************************************************************************/
 uint16_t filename_hash(const char *filename)
{
	uint32_t hash= 2166136261;
	const char* p;
	for(p=filename;*p;p++)
	{
		hash ^=*p;
		hash *=16777619;
	}
	hash=(hash%(uint32_t)0xFFFE)+1;
	return hash;
}
/*******************************************************************************
 * Function Name  	: 
 * Return         	: 
 * Parameters 			:
 * Description			: 
*******************************************************************************/

void Convert8to16(uint8_t * Data8,uint16_t* Data16, uint8_t length)
{
	uint16_t i;
	
	for(i=0;i<length;i++)
	{
		Data16[i]=(uint16_t )(Data8[i*2]|Data8[i*2+1]<<8);
	}
}
void Convert16to8(uint8_t * Data8,uint16_t* Data16,uint8_t length)
{
	uint16_t i;
	for(i=0;i<length;++i)
	{
		Data8[(i*2)]=Data16[i];
		Data8[(i*2)+1]=Data16[i]>>8;
	}
}
void Convert8to32(uint8_t * Data8,uint32_t* Data32, uint8_t length)
{
	uint16_t i;
	
	for(i=0;i<length;i++)
	{
		Data32[i]=(uint32_t )(Data8[i*4]|Data8[i*4+1]<<8|Data8[i*4+2]<<16|Data8[i*4+3]<<24);
	}
}
void Convert32to8(uint8_t * Data8,uint32_t* Data32,uint8_t length)
{
	uint16_t i;
	for(i=0;i<length;++i)
	{
		Data8[(i*4)]=Data32[i];
		Data8[(i*4)+1]=Data32[i]>>8;
		Data8[i*4+2]=Data32[i]>>16;
		Data8[i*4+3]=Data32[i]>>24;
	}
}


/*******************************************************************************
 * Function Name  	: 
 * Return         	: 
 * Parameters 			:
 * Description			: 
*******************************************************************************/

static uint32_t find_first_unallocated_file_index(uint32_t Address,uint16_t maxfiles)
{
	uint8_t hashtable[512];
	uint32_t n,i,index=0;
	do{
			n=512;
			if(index+n>maxfiles)
				n=maxfiles-index;
			FLASH_ReadBuffer8(HEADER_SIZE8+Address+index,hashtable,n);
			for(i=0;i<n;i++)
			{
				if(hashtable[i]==0xFF&&hashtable[i+1]==0xFF)
				{
					return index+i;
				}
			}
			index +=n;
		}
	while(index<maxfiles);
	return 0xFFFFFFFF;
}

/*******************************************************************************
 * Function Name  	: 
 * Return         	: 
 * Parameters 			:
 * Description			: 
*******************************************************************************/
uint32_t Create(const char * filename)
{
	Header_Structure	Header;
	uint8_t *p;
	uint8_t Count[2],len,Data8[HEADER_SIZE];
	uint16_t index,i,k,t,Data16[2];
	uint32_t stradd,Address;
	Sendstring("\rCreating !\r");
	
	Header.Number=1;
	memset(Header.Filename,NULL,12);
	// Check file's exist?
	for(k=0;k<3;k++)
	{
		for(i=0;i<(MAXFILE/HEADER_SIZE);i++)
		{
			FLASH_ReadBuffer8((k*MAXFILE/HEADER_SIZE+i)*HEADER_SIZE,Data8,2);
			WaitWriteInProcess();
			Convert8to16(Data8,&Header.hash_filename,1);
			if(Header.hash_filename==filename_hash(filename) )
			{
					Header.Number++;	
			}
		}
	}
	
	// Find locattion empty
	for(k=0;k<3;k++)
	{
		for(i=0;i<(MAXFILE/HEADER_SIZE);i++)
		{
			FLASH_ReadBuffer8((k*MAXFILE/HEADER_SIZE+i)*HEADER_SIZE,Count,2);
			WaitWriteInProcess();
			if(Count[0]== 0xFF&&Count[1]==0xFF)
			break;
		}
		break;
	}
	Header.Address=(k*(MAXFILE/HEADER_SIZE)+i+10)*MAXFILE;
	len=strlen(filename);
	//Write in flash
	Header.hash_filename=  filename_hash(filename);
	for(t=0;t<len;t++)
	{
		Header.Filename[t]=(uint8_t)(*filename++);
	}
	p=(uint8_t *)&Header;
	FLASH_WriteBuffer(p,HEADER_SIZE*(k*(MAXFILE/HEADER_SIZE)+i),20); 
	WaitWriteInProcess();
	if(Header.Number>1)
	{
		Sendstring("This filename is exist!\r");
		FLASH_WriteBuffer(p,Header.Address,8); 
	}
	else
	{
		Sendstring("Creating a new file!\r");
		FLASH_WriteBuffer(p,Header.Address,8);
	
	}
	WaitWriteInProcess();
	Delay_ms(1);
	Sendstring("Complete create !\r");
	return Header.Address;
}

/*******************************************************************************
 * Function Name  	: 
 * Return         	: 
 * Parameters 			:
 * Description			: 
*******************************************************************************/
void Read(const char* filename)
{
	uint8_t Number=1,Data8[8],xData8[DATA_SIZE];
	uint16_t i,Data16[2],k;
	uint32_t Address;
	
	REST:
	for(k=0;k<3;k++)
	{
		for(i=0;i<(MAXFILE/HEADER_SIZE);++i)
		{
			FLASH_ReadBuffer8((k*MAXFILE/HEADER_SIZE+i)*HEADER_SIZE,Data8,HEADER_SIZE);
			Convert8to16(Data8,Data16,2);
			if(Data16[0]==filename_hash(filename))
			{
				Convert8to32(&Data8[4],&Address,1);
				if(Data16[1]==Number)
				{
					FLASH_ReadBuffer8(Address+HEADER_SIZE8,xData8,DATA_SIZE);
					Sendstring1((char *)xData8,DATA_SIZE);
					Number++;
					goto REST;
				}
			}
		}
		
	}
	if(Number==1)
	{
		Sendstring("\rFile does not exist!");
		return;
	}
}
/*******************************************************************************
 * Function Name  	: 
 * Return         	: 
 * Parameters 			:
 * Description			: 
*******************************************************************************/
void Read_Address(const char* filename,uint8_t *data,uint32_t Location,uint32_t Length)
{
	uint8_t Data8[8],Count=0;
	uint16_t k,i,t,Data16[2],xData16[2],Number=1;
	uint32_t Address,xLocated;
	
	if(Location>DATA_SIZE)
		{
			Number=Location/(DATA_SIZE)+1; //ten so luong cua file
			xLocated=Location%(DATA_SIZE); // vi tri trong data file
		}
	else
	{
		Number=1;
		xLocated=Location;
	}

	for(k=0;k<3;k++)
	{
		for(i=0;i<(MAXFILE/HEADER_SIZE);++i)
		{
			FLASH_ReadBuffer8((k*MAXFILE/HEADER_SIZE+i)*HEADER_SIZE,Data8,HEADER_SIZE8);
			WaitWriteInProcess();
			Convert8to16(Data8,Data16,2);
			
			if(Data16[0]==filename_hash(filename)&&Data16[1]==Number)
			{
				
				Convert8to32(&Data8[4],&Address,1);
				if(Length+xLocated<DATA_SIZE)
				{
					FLASH_ReadBuffer8(Address+HEADER_SIZE8+xLocated,data,Length);
					WaitWriteInProcess();
					return;
				}
				else
				{
					FLASH_ReadBuffer8(Address+HEADER_SIZE8+xLocated,data,(DATA_SIZE)-xLocated);
					WaitWriteInProcess();
					Number++;
					for(t=0;t<(MAXFILE/HEADER_SIZE);++t)
					{						
						FLASH_ReadBuffer8((k*MAXFILE/HEADER_SIZE+t)*HEADER_SIZE,Data8,HEADER_SIZE8);
						WaitWriteInProcess();
						Convert8to16(Data8,xData16,2);
						if(xData16[0]==Data16[0]&&xData16[1]==Number)
						{
							Convert8to32(&Data8[4],&Address,1);
							FLASH_ReadBuffer8(Address+HEADER_SIZE8,&data[DATA_SIZE-xLocated],Length-(DATA_SIZE-xLocated));
															return;
						}
					}
				}
			}
		}
	}
}

/*******************************************************************************
 * Function Name  	: 
 * Return         	: 
 * Parameters 			:
 * Description			: 
*******************************************************************************/
void Write(const char* filename,uint8_t * Buffer,uint16_t length)
{
	uint8_t		Data8[8],count;
	uint16_t	i,Data16[2],index,n,k;
	Header_Structure Header;
	
	Header.Number=0;
	Sendstring("Write !\r");
	/// Tim kiem vi tri lon nhat cua filename trong listfile
	for(k=0;k<3;k++)
	{
		for(i=0;i<(MAXFILE/HEADER_SIZE);i++)
		{
			FLASH_ReadBuffer8((k*MAXFILE/HEADER_SIZE+i)*HEADER_SIZE,Data8,8);
			Convert8to16(Data8,Data16,2);
			if(Data16[0]==filename_hash(filename)&&Data16[1]>Header.Number)
			{	
					Header.Number=Data16[1];
					Convert8to32(&Data8[4],&Header.Address,1);
			}
		}
	}
	if(Header.Number==0)
	{
		Sendstring("\rFile does nor exist!");
		return;
	}
	
	index=find_first_unallocated_file_index(Header.Address,MAXFILE);
	if(length+index+HEADER_SIZE8>MAXFILE)
	{
		n=MAXFILE-index-HEADER_SIZE8;
		FLASH_WriteBuffer(Buffer,HEADER_SIZE8+Header.Address+index,n+1);
		WaitWriteInProcess();
		Header.Address=Create(filename);
		length-=n;
		FLASH_WriteBuffer(&Buffer[n],HEADER_SIZE8+Header.Address,length);
		WaitWriteInProcess();
	}
	else
	{
		FLASH_WriteBuffer(Buffer,HEADER_SIZE8+Header.Address+index,length);
	}
	WaitWriteInProcess();
Sendstring("Writen !\r");
	
}

/*******************************************************************************
 * Function Name  :	
 * Return         :	
 * Parameters 		:	
 * Description		:	
*******************************************************************************/
void Erase(const char* filename)
{
	uint8_t y,xfilename[MAXFILE+4],Data8[7],Copy,t=0;
	uint16_t i,Number,Data16[2];
	uint32_t 	Address;
	
	
	Sendstring("Erase!\r");
	for(t=0;t<3;t++)
	{
		Copy=0;
		FLASH_ReadBuffer8(t*MAXFILE,xfilename,MAXFILE+4);
		WaitWriteInProcess();
		/// Tim kiem vi tri, so luong cua filename trong listfile
		for(i=0;i<(MAXFILE)/HEADER_SIZE;++i)
		{
			Convert8to16(&xfilename[i*HEADER_SIZE],Data16,2);
			Convert8to32(&xfilename[i*HEADER_SIZE+4],&Address,1);
			if(Data16[0]==filename_hash(filename))
			{	
				Copy++;
				for(y=0;y<HEADER_SIZE;++y)
				{
					xfilename[i*HEADER_SIZE+y]=0xFF; // Erase all Header-filename in Listfile
				}
				FLASH_Erase_Sector((uint32_t)Address+1); // Erase Data in flash
				WaitWriteInProcess();
			}
		}
		if(Copy>>0)
		{
			Sendstring("\rFile  exist!");
			FLASH_Erase_Sector(t*MAXFILE);
			WaitWriteInProcess();
			FLASH_WriteBuffer(xfilename,t*MAXFILE,MAXFILE);
			WaitWriteInProcess();
		}
		else
		Sendstring("\rFile doesn't exist in this Sector!");
	}
	Sendstring("Complete Erase!\r");
}
/*******************************************************************************
 * Function Name  :	
 * Return         :	
 * Parameters 		:	
 * Description		:	
*******************************************************************************/
void Listfile()
	{
		char	buf[100];		
		uint8_t Data8[HEADER_SIZE];
		uint16_t i,k,Data16[2];
		uint32_t Address;
		Sendstring("\r		 		List_Files!\r");
		
		for(k=0;k<3;k++)
		{
			for(i=0;i<MAXFILE/HEADER_SIZE;++i)
			{
				FLASH_ReadBuffer8((k*MAXFILE/HEADER_SIZE+i)*HEADER_SIZE,Data8,HEADER_SIZE);
				WaitWriteInProcess();
				Convert8to16(Data8,Data16,2);
				Convert8to32(&Data8[4],&Address,1);
				
				if(Data16[0]!=0xFFFF)
				{
					sprintf(buf,"%d.%d	%s	Address: %d\r",i,Data16[1],&Data8[8],Address);
					Sendstring(buf);
					Delay_ms(1);
				}
			}
		}
		Sendstring("\r		 		End_Files!\r");
	}
/*******************************************************************************
 * Function Name  :	
 * Return         :	
 * Parameters 		:	
 * Description		:	
*******************************************************************************/
void Search_File(const char* filename)
{
	char	buf[100];		
		uint8_t Data8[HEADER_SIZE];
		uint16_t i,k,m=0,Data16[2];
		uint32_t Address;
		Sendstring("\r		 		Search!\r");
		
		for(k=0;k<3;k++)
		{
			for(i=0;i<MAXFILE/HEADER_SIZE;++i)
			{
				FLASH_ReadBuffer8((k*MAXFILE/HEADER_SIZE+i)*HEADER_SIZE,Data8,HEADER_SIZE);
				WaitWriteInProcess();
				Convert8to16(Data8,Data16,2);
				Convert8to32(&Data8[4],&Address,1);
				
				if(Data16[0]==filename_hash(filename))
				{
					m++;
					sprintf(buf,"%d.%d	%s	Address: %d\r",i,Data16[1],&Data8[8],Address);
					Sendstring(buf);
					Delay_ms(1);
				}
			}
		}
		if(m==0)
		Sendstring("\r File doesn't exist ! \r");
		Sendstring("\r		 		End_Search!\r");
	
}