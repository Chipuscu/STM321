#ifndef __LED_H_
#define __LED_H_
#include "STM32F2xx.h" 
void LED_Init(void);
void LED_Nhay(GPIO_TypeDef *GPIO,uint16_t GPIO_Pin);
void led_toggle(void);
#endif
