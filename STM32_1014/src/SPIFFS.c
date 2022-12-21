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
//sig[0]=(uint32_t )(buf[1]<<24|buf[2]<<16|buf[3]<<8|buf[4]);

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

uint32_t check_signature(void)
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
		WaitWriteInProcess();
		Delay_ms(10);
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

 bool Create(const char * filename,uint32_t length, uint32_t align)
{
	
	uint32_t maxfile, stringsize, stradd, address, len;
	uint32_t index,buff[3],a;
	uint8_t xbuff[12];
	char buf[50];
	
	Sendstring("\rCreate !\r");
	maxfile= check_signature();
	//maxfile=18F60258
	if(!maxfile)	return false;
	stringsize=(maxfile&0xFFFF0000)>>14;
	
	maxfile &= 0xFFFF;
	index= find_first_unallocated_file_index(maxfile);
	
	stradd=8+maxfile*12; // 7208
	
	if(index==0)
	{
		address=stradd+stringsize; //  32768
	}
	else
	{
//		buff[2]=0;
//		FLASH_ReadBuffer(8+maxfile*2+(index-1)*10,(uint16_t *)buff,10);
//		address=buff[0]+buff[1];
//		stradd += buff[2]*4;
//		stradd += string_lenght(stradd);
//		stradd =(stradd+3)&0x0003FFFC;
	}
	if (align > 0) 
	{
		address += align - 1;
		address /= align;
		address *= align;
			
		length += align - 1;
		length /= align;
		length *= align;
	}
	else
	{
		address=(address+255) & 0xFFFFFF00; //128
	}
	
	len=strlen(filename);
	
	//
	FLASH_WriteBuffer((uint8_t *)filename,stradd,len+1); // 7208
	buff[0]=address; //32768
	buff[1]=length;	// 10
	buff[2]=(stradd-(8+maxfile*12))/4; // 
	Convert32to8(xbuff,buff,3);
	Delay_ms(5);
	FLASH_WriteBuffer(xbuff,8+maxfile*2+index*10,10);  // 1208
	WaitWriteInProcess();
	Delay_ms(5);
	buff[0]=  filename_hash(filename);
	FLASH_WriteBuffer((uint8_t *)buff,8+index*2,2); // 8
	Sendstring("Complete create !\r");
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
	maxfile= check_signature();
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

