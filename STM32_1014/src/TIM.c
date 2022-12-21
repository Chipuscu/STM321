#include "TIM.h"
#include "STM32F2xx.h" 
#include "UART.h"
#include "main.h"

/*******************************************************************************
 * Function Name  	: TIM2_Initialize
 * Return         	: None
 * Parameters 		: None
 * Description		: Khoi tao Timer 2
*******************************************************************************/
void TIM2_Init(void)
{
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
	NVIC_InitTypeDef		     	NVIC_InitStructure;
	
		/*Enable _Clock*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	/* Enable the TIM2 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);

	//TIM_Cmd(TIM2, DISABLE);
	
	//TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler=36000-1;
	TIM_TimeBaseStructure.TIM_Period=2000-1;  //  1s
	TIM_TimeBaseStructure.TIM_ClockDivision=0;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up ;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
		/***************************/
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	TIM_Cmd(TIM2,ENABLE);
	
}
/*******************************************************************************
 * Function Name  	: TIM3_Initialize
 * Return         	: None
 * Parameters 		: None
 * Description		: Khoi tao Timer 3
*******************************************************************************/
void TIM3_Init(void)
{
	
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
	NVIC_InitTypeDef		     	NVIC_InitStructure;
	
		/*Enable _Clock*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	/* Enable the TIM2 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);

	//TIM_Cmd(TIM3, DISABLE);
	
	//TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler=36000-1;
	TIM_TimeBaseStructure.TIM_Period=1000-1;  //  0.5s
	TIM_TimeBaseStructure.TIM_ClockDivision=0;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up ;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
		/***************************/
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	TIM_Cmd(TIM3,ENABLE);
}

