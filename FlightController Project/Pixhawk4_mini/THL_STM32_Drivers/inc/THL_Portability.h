#ifndef __THL_Portability_H
#define __THL_Portability_H


/*========================Global Setting Macros===========================*/
#define USE_STM32F7
//#define SysTick_Reserved  //SysTick reserved for RTOS
/*========================================================================*/


/*==========================USART Setting=================================*/
#define USART_TxBuffer_Size 1000
#define USART_RxBuffer_Size 100
#define USART_Default_TxTimeOut 0xFFFF  // 1 second
#define USART_Default_RxTimeOut 0xFFFFFFFF  // 1 second
/*========================================================================*/













#ifdef USE_STM32F1
#include "stm32f1xx_hal.h"
#endif

#ifdef USE_STM32F4
#include "stm32f4xx_hal.h"
#endif

#ifdef USE_STM32F7
#include "stm32f7xx_hal.h"
#endif






#endif
