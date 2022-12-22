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

static uint32_t find_first_unallocated_file_index(uint32_t Address,uint16_t maxfiles)
{
	uint8_t hashtable[50];
	uint32_t n,i,index=0;
	do{
			n=50;
			if(index+n>maxfiles)
				n=maxfiles-index;
			FLASH_ReadBuffer8(30+Address+index,hashtable,n);
			for(i=0;i<50;i++)
			{
				if(hashtable[i]==0xFF)
				{
					return index+i;
				}
			}
			index +=50;
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
uint32_t Create(const char * filename)
{
	char buf[50];
	uint8_t Count[2],i,Add=0,len,xbuff[5];
	uint16_t maxfile,index,buff[2];
	uint16_t stradd,address;
	Sendstring("\rCreating !\r");
	
	maxfile =MAXFILE;
	xbuff[4]=1;
	for(Add=0,i=0;i<30;i++)
	{
		FLASH_ReadBuffer8(Add,xbuff,4);
		Convert8to16(xbuff,buff,2);
		if(buff[0]==filename_hash(filename) )
		{
				xbuff[4]++;
				Sendstring("This filename is exist!");
		}
		Add+=30;
	}
	for(Add=0,i=0;i<30;i++)
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
	//index= find_first_unallocated_file_index(maxfile);
	//stradd= 24+index+maxfile*(i+1); // Dia chi bat dau viet
	len=strlen(filename);
	//
	//Ghi vao  flash
	buff[0]=  filename_hash(filename);
	buff[1]=address;
	Convert16to8(xbuff,buff,2);
	FLASH_WriteBuffer(xbuff,maxfile*(i+1),5);  // 1208
	Delay_ms(5);
	FLASH_WriteBuffer((uint8_t *)filename,5+maxfile*(i+1),len); // 8
	// Write in table of contents
	Delay_ms(5);
	FLASH_WriteBuffer((uint8_t *)filename,30*i+5,len);
	Delay_ms(5);
	FLASH_WriteBuffer(xbuff,30*i,5);
	Delay_ms(2);
	Sendstring("Complete create !\r");
	return address;
}


/*******************************************************************************
 * Function Name  	: System_KhoiTaoCacBien
 * Return         	: 
 * Parameters 		:
 * Description		: Khai bao
*******************************************************************************/
File_structure Init(const char* filename)
{
	struct File_structure File={Create,Write,Read,Open};
	File.xCreate(filename);
	return File;
}
/*******************************************************************************
 * Function Name  	: System_KhoiTaoCacBien
 * Return         	: 
 * Parameters 		:
 * Description		: Khai bao
*******************************************************************************/
void Read(const char* filename,uint16_t length)
{
	uint8_t i,Add,Data8[5],Copy=0,Number=1;
	uint16_t Data16[2],Address,xData8[994];
	for(Add=0,i=0;i<30;++i)
	{
		FLASH_ReadBuffer8(Add,Data8,5);
		Convert8to16(Data8,Data16,2);
		if(Data16[0]==filename_hash(filename))
		{
			if(Data8[4]>Copy)
					Copy=Data8[4];
			Address=Data16[1];
			if(Data8[4]==Number)
			{
				FLASH_ReadBuffer(Address+30,xData8,length);
				Send2((uint16_t *)xData8,length);
				Number++;
			}
		}
	}
	if(Copy==0)
	{
		Sendstring("\rFile does nor exist!");
		return;
	}
	
}

/*******************************************************************************
 * Function Name  	: System_KhoiTaoCacBien
 * Return         	: 
 * Parameters 		:
 * Description		: Khai bao
*******************************************************************************/
void Write(const char* filename,uint8_t * Buffer,uint16_t length)
{
	uint8_t i,Count[5],Copy=0;
	uint16_t Add=0,Data16[2];
	uint16_t Address,index,n,m;
	Sendstring("Write !");
	for(i=0;i<30;i++)
	{
		FLASH_ReadBuffer8(Add,Count,5);
		Convert8to16(Count,Data16,2);
		if(Data16[0]==filename_hash(filename))
		{	
			if(Count[4]>Copy)
					Copy=Count[4];
			Address=Data16[1];
		}
		else
		Add+=30;
	}
	if(Copy==0)
	{
		Sendstring("\rFile does nor exist!");
		return;
	}
	index=find_first_unallocated_file_index(Address,MAXFILE);
		if(length+index>MAXFILE)
		{
			n=MAXFILE-index;
			m=length-n;
			FLASH_WriteBuffer(Buffer,30+Address+index,n);
			Sendstring("Creating a new file!\r ");
			Address=Create(filename);
			FLASH_WriteBuffer(Buffer,30+Address,m);
			
		}
		FLASH_WriteBuffer(Buffer,30+Address+index,length);
		Delay_ms(2);
	
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