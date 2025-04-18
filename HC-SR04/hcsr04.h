#ifndef __HCSR04_H
#define __HCSR04_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stdio.h"

/* Exported types ------------------------------------------------------------*/
extern TIM_HandleTypeDef htim4;
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void TriggerPulse_Init (void);
void EchoPulse_Init (void);
void IC_TIM4_Initialization (void);
void Measure_Moment (void);

#endif /* __HCSR04_H */