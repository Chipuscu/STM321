#ifndef _SPIFFS_H_
#define	_SPIFFS_H_

#include "STM32F2xx.h" 
#include "stdbool.h"


#define DEFAULT_MAXFILES				600
#define DEFAULT_STRINGS_SIZE		25560

// Typedef
typedef struct File_structure File_structure,*pno;
struct File_structure
{
	bool (*xCreate)(const char *filename);
	void (*xWrite)(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t Len);
	void (*xRead)(int Address, uint8_t *Buffer,uint16_t Len);
	File_structure (*xOpen)(const char *);
	
	uint32_t address;
	uint32_t length;
	uint32_t offset;
	uint32_t dir_index;
	
	
	
};
//Protocol
File_structure Init();
bool Create(const char * filename);
static uint32_t find_first_unallocated_file_index(uint32_t maxfiles);
static uint32_t string_lenght(uint32_t addr);
static uint16_t filename_hash(const char *filename);
//bool Create(const char * filename,uint32_t length, uint32_t align);
static bool filename_compare(const char *filename, uint32_t stradd);
File_structure Open(const char * filename);
void Convert8to16(uint8_t * Data8,uint16_t* Data16, uint8_t length);
uint32_t check_signature(uint32_t Addr);
void Convert16to8(uint8_t * Data8,uint16_t* Data32,uint8_t length);
#endif