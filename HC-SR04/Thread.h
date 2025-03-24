#ifndef __THREAD_H
#define __THREAD_H

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "hcsr04.h"

/* Exported types ------------------------------------------------------------*/
extern osThreadId_t tid_Thread;                 // extern thread id
/* Exported constants --------------------------------------------------------*/
#define WATER_LEVEL 0x01U
#define TIMER_EVENT_FLAG 0x02U
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
int Init_Thread (void);
#endif /* __THREAD_H */