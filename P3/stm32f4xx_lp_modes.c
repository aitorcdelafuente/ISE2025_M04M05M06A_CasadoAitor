/**
  ******************************************************************************
  * @file    PWR/PWR_CurrentConsumption/stm32f4xx_lp_modes.c 
  * @author  MCD Application Team
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the STM32F4xx Low Power Modes:
  *           - Sleep Mode
  *           - STOP mode with RTC
  *           - Under-Drive STOP mode with RTC
  *           - STANDBY mode without RTC and BKPSRAM
  *           - STANDBY mode with RTC
  *           - STANDBY mode with RTC and BKPSRAM
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_lp_modes.h"


/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup PWR_CurrentConsumption
  * @{
  */ 

#define PHY_PWR_DOWN       (1<<11)
#define PHY_ADDRESS        0x00 /* default ADDR for PHY: LAN8742 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LAN8742A_PHY_ADDRESS            0x00U	/* LAN8742A PHY Address*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* RTC handler declaration */
RTC_HandleTypeDef RTCHandle;

/* Private function prototypes -----------------------------------------------*/
//static void SYSCLKConfig_STOP(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function configures the system to enter Sleep mode for
  *         current consumption measurement purpose.
  *         Sleep Mode
  *         ==========  
  *            - System Running at PLL (100MHz)
  *            - Flash 3 wait state
  *            - Instruction and Data caches ON
  *            - Prefetch ON   
  *            - Code running from Internal FLASH
  *            - All peripherals disabled.
  *            - Wakeup using EXTI Line (user Button)
  * @param  None
  * @retval None
  */
void SleepMode_Measure(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Disable USB Clock */
  __HAL_RCC_USB_OTG_FS_CLK_DISABLE();

  /* Disable Ethernet Clock */
  __HAL_RCC_ETH_CLK_DISABLE();

  /* Configure all GPIO as analog to reduce current consumption on non used IOs */
  /* Enable GPIOs clock */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = GPIO_PIN_All;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /* Disable GPIOs clock */
  __HAL_RCC_GPIOE_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();
  __HAL_RCC_GPIOI_CLK_DISABLE();
  __HAL_RCC_GPIOJ_CLK_DISABLE();
  __HAL_RCC_GPIOK_CLK_DISABLE();

  /* Configure user Button */
//  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);
  Init_BlueButton_WAKEUP();
  
  /* Disable ALARM IT*/
  HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);//OJO, la alarma va por IT -> Despierta de modo sleep !
  
  /*Encender led rojo*/
  LED_RED_Initialize ();
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);

  /* Suspend Tick increment to prevent wakeup by Systick interrupt. 
     Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base) */
  HAL_SuspendTick();

  /* Request to enter SLEEP mode */
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

  /* Resume Tick interrupt if disabled prior to sleep mode entry */
  HAL_ResumeTick();
  
  __HAL_RCC_ETH_CLK_ENABLE();
  __HAL_RCC_USB_OTG_FS_CLK_ENABLE();
  
  /* Exit Ethernet Phy from LowPower mode */
  ETH_PhyExitFromPowerDownMode(); 
  
  /*Apagar led rojo*/
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
  
  /* Vuelvo a activar la alarma */
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
  
}

void Init_BlueButton_WAKEUP (void){
  
  GPIO_InitTypeDef GPIO_InitStruct;
  
  __HAL_RCC_GPIOC_CLK_ENABLE(); // Se habilita el reloj del GPIO C dado que el boton esta asociado al GPIOC13
  
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Pull = GPIO_NOPULL; // No hay R pullup ni pulldown
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // Detecta los flancos de bajada

  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); // Se inicia el pin
  
  /* Enable and set Button EXTI Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0x0F, 0x00); /* The lower is the priority, the more prioritary is*/
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); // Las interrupciones del boton azul corresponden a las EXTI15-10
  
}

/**
  * @brief  This function configures the ETH PHY to enter the power down mode
  *         This function should be called before entering the low power mode.
  * @param  None
  * @retval None
  */
void ETH_PhyEnterPowerDownMode(void)
{
  ETH_HandleTypeDef heth;
  GPIO_InitTypeDef GPIO_InitStruct;
  uint32_t phyregval = 0; 
   
  /* This part of code is used only when the ETH peripheral is disabled 
	   when the ETH is used in the application this initialization code 
	   is called in HAL_ETH_MspInit() function  ***********************/
	
	/* Enable GPIO clocks*/
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  /* Configure PA2: ETH_MDIO */
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL; 
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /* Configure PC1: ETH_MDC */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  /* Enable the ETH peripheral clocks */
  __HAL_RCC_ETH_CLK_ENABLE();
  
  /* Set ETH Handle parameters */
  heth.Instance = ETH;
  heth.Init.PhyAddress = LAN8742A_PHY_ADDRESS;

  /* Configure MDC clock: the MDC Clock Range configuration
	   depends on the system clock: 180Mhz/102 = 1.76MHz  */
  /* MDC: a periodic clock that provides the timing reference for 
	   the MDIO data transfer which shouldn't exceed the maximum frequency of 2.5 MHz.*/
  heth.Instance->MACMIIAR = (uint32_t)ETH_MACMIIAR_CR_Div102;

  /*****************************************************************/
  
  /* ETH PHY configuration in Power Down mode **********************/ 
  
  /* Read ETH PHY control register */
  HAL_ETH_ReadPHYRegister(&heth, PHY_BCR, &phyregval);
  
  /* Set Power down mode bit */
  phyregval |= PHY_POWERDOWN;
  
  /* Write new value to ETH PHY control register */
  HAL_ETH_WritePHYRegister(&heth, PHY_BCR, phyregval);
  
  /*****************************************************************/
  
  /* Disable periph CLKs */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_ETH_CLK_DISABLE();
}

/**
  * @brief  This function wakeup the ETH PHY from the power down mode
  *         When exiting from StandBy mode and the ETH is used in the example
  *         its better to call this function at the end of HAL_ETH_MspInit() 
  *         then remove the code that initialize the ETH CLKs ang GPIOs.
  * @param  None
  * @retval None
  */
void ETH_PhyExitFromPowerDownMode(void)
{
   ETH_HandleTypeDef heth;
   GPIO_InitTypeDef GPIO_InitStruct;
   uint32_t phyregval = 0;
   
  /* ETH CLKs and GPIOs initialization ******************************/
  /* To be removed when the function is called from HAL_ETH_MspInit() when 
     exiting from Standby mode */
  
  /* Enable GPIO clocks*/
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  /* Configure PA2 */
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL; 
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /* Configure PC1*/
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  /* Enable ETH CLK */
  __HAL_RCC_ETH_CLK_ENABLE();
  /*****************************************************************/
  
  /* ETH PHY configuration to exit Power Down mode *****************/
  /* Set ETH Handle parameters */
  heth.Instance = ETH;
  heth.Init.PhyAddress = LAN8742A_PHY_ADDRESS;
  
  /* Configure MDC clock: the MDC Clock Range configuration
    depends on the system clock: 180Mhz/102 = 1.76MHz  */
  /* MDC: a periodic clock that provides the timing reference for 
    the MDIO data transfer which shouldn't exceed the maximum frequency of 2.5 MHz.*/
  heth.Instance->MACMIIAR = (uint32_t)ETH_MACMIIAR_CR_Div102; 
  
  /* Read ETH PHY control register */
  HAL_ETH_ReadPHYRegister(&heth, PHY_BCR, &phyregval);
  
  /* check if the PHY  is already in power down mode */
  if ((phyregval & PHY_POWERDOWN) != RESET)
  {
    /* Disable Power down mode */
    phyregval &= ~ PHY_POWERDOWN;
    
    /* Write value to ETH PHY control register */
    HAL_ETH_WritePHYRegister(&heth, PHY_BCR, phyregval);
  }
  /*****************************************************************/
}

void LED_RED_Initialize (void) {
  
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* Enable LED STM32F429ZI */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  /* Configure GPIO LED pins */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
}

/**
  * @}
  */ 

/**
  * @}
  */ 
