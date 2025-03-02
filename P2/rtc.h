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
#include "lcd.h"

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif

/* Exported constants --------------------------------------------------------*/
/* Defines related to Clock configuration */
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

/* Exported functions --------------------------------------------------------*/
void RTC_Init (void);
void RTC_Time_Config (uint8_t hor, uint8_t min, uint8_t seg);
void RTC_Date_Config (uint8_t dia, uint8_t mes, uint8_t anio);
void RTC_Show(uint8_t *showtime, uint8_t *showdate);

#endif /* __RTC_H */