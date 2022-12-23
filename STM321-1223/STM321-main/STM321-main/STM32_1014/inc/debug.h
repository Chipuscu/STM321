#ifndef __DEBUG__H
#include "STM32F2xx.h" 
#include "Structure_Data.h"


//void Config_Process(void);
uint8_t Config_Process(void);
void hex_decode(const char *in, uint32_t len,uint8_t *out);
uint16_t GetCrc16(const uint8_t* pData, uint32_t nLength);


#endif
