#ifndef _SPIFFS_H_
#define	_SPIFFS_H_

#include "STM32F2xx.h" 
#include "stdbool.h"


#define MAXFILE									4096
#define LISTFILE_SIZE						1024

struct File_structure
{
	uint32_t (*xCreate)(const char *filename);
	void (*xWrite)(const char* filename,uint8_t * Buffer,uint16_t length);
	void (*xRead)(const char* filename,uint16_t length);
	void (*xErase)(const char* filename);
	void (*xListfile)();
	
	uint32_t address;
	uint32_t length;
	uint32_t offset;
	uint32_t dir_index;
};
typedef struct File_structure File_structure;

//Protocol

File_structure Init(const char* filename);

uint32_t Create(const char * filename);
void Write(const char* filename,uint8_t * Buffer,uint16_t length);
void Erase(const char* filename);
void Read(const char* filename,uint16_t length);
void Listfile();

static uint16_t filename_hash(const char *filename);
static uint32_t find_first_unallocated_file_index(uint32_t Address,uint16_t maxfiles);
void Convert8to16(uint8_t * Data8,uint16_t* Data16, uint8_t length);
void Convert16to8(uint8_t * Data8,uint16_t* Data32,uint8_t length);
void Convert8to32(uint8_t * Data8,uint32_t* Data32, uint8_t length);
void Convert32to8(uint8_t * Data8,uint32_t* Data32,uint8_t length);

#endif