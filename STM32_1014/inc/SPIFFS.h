#ifndef _SPIFFS_H_
#define	_SPIFFS_H_

#include "STM32F2xx.h" 
#include "stdbool.h"


#define DEFAULT_MAXFILES				600
#define DEFAULT_STRINGS_SIZE		25560
#define MAXFILE									1024

// Typedef
typedef struct File_structure File_structure,*pno;
struct File_structure
{
	uint32_t (*xCreate)(const char *filename);
	void (*xWrite)(const char* filename,uint8_t * Buffer,uint16_t length);
	void (*xRead)(const char* filename,uint16_t length);
	File_structure (*xOpen)(const char *);
	
	uint32_t address;
	uint32_t length;
	uint32_t offset;
	uint32_t dir_index;
	
	
	
};
//Protocol
void Read(const char* filename,uint16_t length);
static uint32_t find_first_unallocated_file_index(uint32_t Address,uint16_t maxfiles);
void Write(const char* filename,uint8_t * Buffer,uint16_t length);
File_structure Init(const char* filename);
uint32_t Create(const char * filename);
static uint32_t string_lenght(uint32_t addr);
static uint16_t filename_hash(const char *filename);
//bool Create(const char * filename,uint32_t length, uint32_t align);
static bool filename_compare(const char *filename, uint32_t stradd);
File_structure Open(const char * filename);
void Convert8to16(uint8_t * Data8,uint16_t* Data16, uint8_t length);
uint32_t check_signature(uint32_t Addr);
void Convert16to8(uint8_t * Data8,uint16_t* Data32,uint8_t length);
#endif