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
 * Parameters 		:
 * Description		: 
*******************************************************************************/
static bool filename_compare(const char *filename, uint32_t stradd)
{
	uint8_t	 buf[16],i;
	const char* p;
	p=filename;
	while(1)
	{
		FLASH_ReadBuffer8(stradd,buf,sizeof(buf));
		stradd += sizeof(buf);
		for(i=0;i<sizeof(buf);++i)
		{
			if(*p++!=buf[i]) return false;
			if(buf[i]==0)		return true;
		}
	}
}
/*******************************************************************************
 * Function Name  	: 
 * Return         	: 
 * Parameters 		:
 * Description		: 
*******************************************************************************/
static uint16_t filename_hash(const char *filename)
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
 * Parameters 		:
 * Description		:
*******************************************************************************/
void Convert8to32(uint8_t * Data8,uint32_t* Data32, uint8_t length)
{
	uint16_t i;
	
	for(i=0;i<length;i++)
	{
		Data32[i]=(uint32_t )(Data8[i*4]|Data8[i*4+1]<<8|Data8[i*4+2]<<16|Data8[i*4+3]<<24);
	}
}
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

/*******************************************************************************/

uint32_t check_signature(uint32_t Addr)
{
	uint32_t sig[2],*p;
	uint8_t Data[8];
	uint8_t buf[8],xbuf[8],*q;

	FLASH_ReadBuffer8(0,Data, 8);
	
	Convert8to32((uint8_t *)Data,sig,2);
	
	if (sig[0] == 0xFA96554C) return sig[1];
	if (sig[0] == 0xFFFFFFFF) 
	{
		sig[0] = 0xFA96554C;
		sig[1] = (uint32_t)(((DEFAULT_STRINGS_SIZE/4) << 16)|DEFAULT_MAXFILES);
		Convert32to8(buf,sig,2);
		FLASH_WriteBuffer(buf,0, 8);
		Delay_ms(1);
		FLASH_ReadBuffer8(0,Data, 8);
		//Send1(Data,8);
		Convert8to32((uint8_t *)Data,sig,2);
		if (sig[0] == 0xFA96554C)
		{
			return sig[1];
		}
	}
	return 0;
	
}
/*******************************************************************************
 * Function Name  	: System_KhoiTaoCacBien
 * Return         	: 
 * Parameters 		:
 * Description		: Khoi tao cac bien dung trong he thong
*******************************************************************************/

static uint32_t find_first_unallocated_file_index(uint32_t maxfiles)
{
	uint8_t xhashtable[16];
	uint16_t hashtable[8];
	uint32_t n,i,index=0;
	do{
			n=8;
			if(index+n>maxfiles)
				n=maxfiles-index;
			FLASH_ReadBuffer8(8+index*2,xhashtable,n*2);
			Convert8to16(xhashtable,hashtable,n);
			for(i=0;i<n;i++)
			{
				if(hashtable[i]==0xFFFF)
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
 * Function Name  	: System_KhoiTaoCacBien
 * Return         	: 
 * Parameters 		:
 * Description		: Khoi tao cac bien dung trong he thong
*******************************************************************************/

 static uint32_t string_lenght(uint32_t addr)
 {
	 char buf[16];
	 const char *p;
	 uint32_t len=0;
	while(1)
	{
		FLASH_ReadBuffer(addr,(uint16_t *)buf,sizeof(buf));
		for(p=buf;p<buf+sizeof(buf);p++)
		{
			len++;
			if(*p==0) return len;
		}
		addr += sizeof(buf);
	}
 }
 
 
/*******************************************************************************
 * Function Name  	: 
 * Return         	: 
 * Parameters 		:
 * Description		: Khoi tao cac bien dung trong he thong
*******************************************************************************/
 bool Create(const char * filename)
{
	uint8_t Count[2],i,Add=0,len,xbuff[4];
	uint16_t maxfile,index,buff[2];
	uint16_t stradd,address;
	Sendstring("\rCreating !\r");
	
	maxfile =1024;
	
	for(i=0;i<30;i++)
	{
		FLASH_ReadBuffer8(Add,Count,2);
		if(Count[0]== 0xFF)
		{
				break;
		}
		else
		Add+=30;
	}
	address=(i+1)*maxfile;
	index= find_first_unallocated_file_index(maxfile);
	stradd= 24+index+maxfile*(i+1); // Dia chi bat dau viet
	len=strlen(filename);
	//
	//Ghi vao  flash
	buff[0]=  filename_hash(filename);
	buff[1]=address;
	Convert16to8(xbuff,buff,2);
	FLASH_WriteBuffer(xbuff,maxfile*(i+1),4);  // 1208
	Delay_ms(5);
	FLASH_WriteBuffer((uint8_t *)filename,4+maxfile*(i+1),len); // 8
	// Write in table of contents
	Delay_ms(5);
	FLASH_WriteBuffer((uint8_t *)filename,30*i,len);
	Delay_ms(5);
	FLASH_WriteBuffer(xbuff,len+30*i,4);
	Delay_ms(2);
	Sendstring("Complete create !\r");
}


/*******************************************************************************
 * Function Name  	: System_KhoiTaoCacBien
 * Return         	: 
 * Parameters 		:
 * Description		: Khai bao
*******************************************************************************/
File_structure Init()
{
	struct File_structure File={Create,FLASH_WriteBuffer,FLASH_ReadBuffer8,Open};
	return File;
}
/*******************************************************************************
 * Function Name  	: System_KhoiTaoCacBien
 * Return         	: 
 * Parameters 		:
 * Description		: Khai bao
*******************************************************************************/
void Write(uint8_t * Buffer,uint16_t length)
{
	
}
/*******************************************************************************
 * Function Name  	: System_KhoiTaoCacBien
 * Return         	: 
 * Parameters 		:
 * Description		: Khoi tao cac bien dung trong he thong


*******************************************************************************/
File_structure Open(const char * filename)
{
	uint8_t xhashtable[16],buf[10];
	uint16_t hashtable[8],hash;
	uint32_t maxfile, buff[3];
	uint32_t n,i, index=0,stradd;
	File_structure File;
	
	Sendstring("\r Open!\r");
//	maxfile= check_signature();
	if(!maxfile) return	File;
	maxfile &= 0xFFFF;
	hash= filename_hash(filename);
	
	while(index<maxfile)
	{
		n=8;
		if(index+n>maxfile)
			n=maxfile-index;
		FLASH_ReadBuffer8(8+index*2,xhashtable,n*2);
		
		Convert8to16(xhashtable,hashtable,8);
		for(i=0;i<n;i++)
		{
			if(hashtable[i]==hash)
			{
				buff[2]=0;
				FLASH_ReadBuffer8(8+maxfile*2+(index+i)*10,buf,10);
				Convert8to32(buf,buff,3);
				
				stradd=8+maxfile*12+buff[2]*4;
				if(filename_compare(filename,stradd))
				{
					
					File.address=buff[0];
					File.length=buff[1];
					File.offset=0;
					File.dir_index=index+i;
					Sendstring("Opened!\r");
					return File;
					
				}
				else if (hashtable[i]==0xFFFF)
					return File;
			}
			index+=n;
		}
		return File;					
		//
	}
}
/*******************************************************************************
 * Function Name  :	
 * Return         :	
 * Parameters 		:	
 * Description		:	
*******************************************************************************/
void Spiffs_erase(uint32_t address)
{
	
}