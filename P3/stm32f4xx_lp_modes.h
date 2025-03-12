/**
  ******************************************************************************
  * @file    PWR/PWR_CurrentConsumption/stm32f4xx_lp_modes.h
  * @author  MCD Application Team
  * @brief   Header for stm32f4xx_lp_modes.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_LP_MODES_H
#define __STM32F4xx_LP_MODES_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
//#include "stm32f4xx_it.h"
//#include "stm32f4xx_hal_eth.h"
//#include "lan8742A.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SleepMode_Measure(void);
void ETH_PhyEnterPowerDownMode(void);
void ETH_PhyExitFromPowerDownMode(void);

#endif /* __STM32F4xx_LP_MODES_H */
