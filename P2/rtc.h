/*
  Autor: Aitor Casado de la Fuente
  Grupo: M04M05M06A
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H
#define __RTC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <time.h>
#include "stdio.h"
#include "string.h"
#include "stdbool.h"

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif

/* Exported constants --------------------------------------------------------*/
/* Defines related to Clock configuration */
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */ //(Valor = 127)
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */ //(Valor = 255)

/* Exported variables --------------------------------------------------------*/
extern RTC_HandleTypeDef rtchandler;
extern volatile bool alarmCheck;

/* Exported functions --------------------------------------------------------*/
void RTC_Init (void);
void RTC_Time_Config (uint8_t hh, uint8_t mm, uint8_t ss);
void RTC_Date_Config (uint8_t dd, uint8_t ms, uint8_t yr);
void RTC_Show(uint8_t *showtime, uint8_t *showdate);
static void init_LSE_Clock (void);
void RTC_SetAlarm (void);

#endif /* __RTC_H */
