#ifndef _SPIFFS_H_
#define	_SPIFFS_H_

#include "STM32F2xx.h" 
#include "stdbool.h"

#define FLASH_MEMORY						8388608 // 8Mb
#define MAXFILE									4096		// 4Kb

#define HEADER_SIZE							20		//2hash+4Add+1Par+13Name
#define HEADER_SIZE8						8
#define DATA_SIZE								MAXFILE-HEADER_SIZE8
struct File_structure
{
	uint32_t (*xCreate)(const char *filename);
	void (*xWrite)(const char* filename,uint8_t * Buffer,uint16_t length);
	void (*xRead)(const char* filename);
	void (*xErase)(const char* filename);
	
	uint32_t address;
	uint32_t length;
	uint32_t offset;
	uint32_t dir_index;
};
typedef struct File_structure File_structure;
typedef struct 
{
	
	uint16_t	hash_filename;
	uint16_t 	Number;
	uint32_t 	Address;
	uint8_t 	Filename[12];
}	Header_Structure;
//Protocol

File_structure Init();//const char* filename);

uint32_t Create(const char * filename);
void Write(const char* filename,uint8_t * Buffer,uint16_t length);
void Erase(const char* filename);
void Read(const char* filename);
void Listfile();
void Search_File(const char* filename);
void Read_Address(const char* filename,uint8_t *data,uint32_t Location,uint32_t Length);

 uint16_t filename_hash(const char *filename);
static uint32_t find_first_unallocated_file_index(uint32_t Address,uint16_t maxfiles);
void Convert8to16(uint8_t * Data8,uint16_t* Data16, uint8_t length);
void Convert16to8(uint8_t * Data8,uint16_t* Data32,uint8_t length);
void Convert8to32(uint8_t * Data8,uint32_t* Data32, uint8_t length);
void Convert32to8(uint8_t * Data8,uint32_t* Data32,uint8_t length);

#endif