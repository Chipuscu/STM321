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
File_structure Init(const char* filename)
{
	struct File_structure File={Create,Write,Read,Erase,Listfile};
	File.xCreate(filename);
	return File;
}

/*******************************************************************************
 * Function Name  	: 
 * Return         	: 
 * Parameters 			:
 * Description			: 
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
 * Function Name  	: 
 * Return         	: 
 * Parameters 			:
 * Description			: 
*******************************************************************************/
uint32_t Create(const char * filename)
{
	
	uint8_t Count[2],i,len,Data8[5];
	uint16_t index,Data16[2];
	uint16_t stradd,address;
	Sendstring("\rCreating !\r");
	
	Data8[4]=1;
	// 
	for(i=0;i<30;i++)
	{
		FLASH_ReadBuffer8(30*i,Data8,4);
		Convert8to16(Data8,Data16,2);
		if(Data16[0]==filename_hash(filename) )
		{
				Data8[4]++;
				Sendstring("This filename is exist!\r");
		}
	}
	// 
	for(i=0;i<30;i++)
	{
		FLASH_ReadBuffer8(30*i,Count,2);
		if(Count[0]== 0xFF)
		{
				break;
		}
	}
	address=(i+1)*MAXFILE;
	len=strlen(filename);
	//Write in flash
	Data16[0]=  filename_hash(filename);
	Data16[1]=address;
	Convert16to8(Data8,Data16,2);
	FLASH_WriteBuffer(Data8,address,5);  // 1208
	Delay_ms(5);
	FLASH_WriteBuffer((uint8_t *)filename,5+address,len); // 8
	// Write in table of contents
	Delay_ms(5);
	FLASH_WriteBuffer((uint8_t *)filename,30*i+5,len);
	Delay_ms(5);
	FLASH_WriteBuffer(Data8,30*i,5);
	Delay_ms(2);
	Sendstring("Complete create !\r");
	return address;
}

/*******************************************************************************
 * Function Name  	: 
 * Return         	: 
 * Parameters 			:
 * Description			: 
*******************************************************************************/
void Read(const char* filename,uint16_t length)
{
	uint8_t i,Number=1,Data8[5],xData8[4066];
	uint16_t Data16[2],Address;
	
	for(i=0;i<30;++i)
	{
		FLASH_ReadBuffer8(30*i,Data8,5);
		Convert8to16(Data8,Data16,2);
		if(Data16[0]==filename_hash(filename))
		{
			Address=Data16[1];
			if(Data8[4]==Number)
			{
				FLASH_ReadBuffer8(Address+30,xData8,4066);
				Send3((uint16_t *)xData8,4066);
				Number++;
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
void Write(const char* filename,uint8_t * Buffer,uint16_t length)
{
	uint8_t i,Data8[5],Copy=0;
	uint16_t Data16[2],Address,index,n,m;
	Sendstring("Write !");
	/// Tim kiem vi tri, so luong cua filename trong listfile
	for(i=0;i<30;i++)
	{
		FLASH_ReadBuffer8(30*i,Data8,5);
		Convert8to16(Data8,Data16,2);
		if(Data16[0]==filename_hash(filename))
		{	
			if(Data8[4]>Copy)
			{
				Copy=Data8[4];
				Address=Data16[1];
			}
		}
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
		FLASH_WriteBuffer(&Buffer[n],30+Address,m);
		
	}
	else
	FLASH_WriteBuffer(Buffer,30+Address+index,length);
	Delay_ms(2);
	
}

/*******************************************************************************
 * Function Name  :	
 * Return         :	
 * Parameters 		:	
 * Description		:	
*******************************************************************************/
void Erase(const char* filename)
{
	uint8_t i,y,xfilename[LISTFILE_SIZE],Data8[5],Copy=0;
	uint16_t Number,Data16[2],Address;
	Sendstring("Erase!\r");
	
	FLASH_ReadBuffer8(0,xfilename,LISTFILE_SIZE);
	Delay_ms(5);
	/// Tim kiem vi tri, so luong cua filename trong listfile
	for(i=0;i<30;++i)
	{
		Convert8to16(&xfilename[i*30],Data16,2);
		if(Data16[0]==filename_hash(filename))
		{	
			Copy++;
			for(y=0;y<30;++y)
			{
				xfilename[i*30+y]=0xFF;
			}
			Address=Data16[1];
			FLASH_Erase_Sector((uint32_t)Address);
			Delay_ms(2000);
		}
	}
	if(Copy==0)
	{
		Sendstring("\rFile does not exist!");
		return;
	}
	FLASH_Erase_Sector(100);
	Delay_ms(2000);
	for(y=0;y<30;y++)
	{
		if(xfilename[y*30]!=0xFF)
		{
			FLASH_WriteBuffer(&xfilename[y*30],y*30,30);
			Delay_ms(10);
		}
	}
	Sendstring("Compelte Erase!\r");
}
/*******************************************************************************
 * Function Name  :	
 * Return         :	
 * Parameters 		:	
 * Description		:	
*******************************************************************************/
void Listfile()
	{
		char	buf[50];
		uint8_t i=0,Data8[30];
		uint16_t Data16[2];
		Sendstring("\r		 List_Files!\r");
		
		for(i=0;i<30;++i)
		{
			FLASH_ReadBuffer8(30*i,Data8,30);
			Convert8to16(Data8,Data16,2);
			Delay_ms(5);
			if(Data16[0]!=0xFF)
			{
				sprintf(buf,"%d.%d	%s	Address: %d\r",i,Data8[4],&Data8[5],Data16[1]);
				Sendstring(buf);
			}
		}
	}