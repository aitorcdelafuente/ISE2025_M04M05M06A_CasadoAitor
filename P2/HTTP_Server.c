/*
  Autor: Aitor Casado de la Fuente
  Grupo: M04M05M06A
*/

/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "main.h"

#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "Board_Buttons.h"              // ::Board Support:Buttons
#include "leds.h"
#include "lcd.h"
#include "adc.h"
#include "rtc.h"
#include "sntp.h"

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

extern uint16_t AD_in          (uint32_t ch);
extern uint8_t  get_button     (void);
extern void     netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len);

extern bool LEDrun;
extern char lcd_text[2][20+1];
extern char rtc_text[2][20+1];

extern osThreadId_t TID_Display;
extern osThreadId_t TID_Led;
extern osThreadId_t TID_RTC;
extern osThreadId_t TID_Alarm;
extern osThreadId_t TID_User;

bool LEDrun;
char lcd_text[2][20+1] = { "LCD line 1",
                           "LCD line 2" };
char rtc_text[2][20+1] = {0};
ADC_HandleTypeDef adchandle;
                           
uint8_t aShowTime[10] = {0};
uint8_t aShowDate[10] = {0}; 

/* Thread IDs */
osThreadId_t TID_Display;
osThreadId_t TID_Led;
osThreadId_t TID_RTC;
osThreadId_t TID_Alarm;
osThreadId_t TID_User;

/* Timers IDs */
osTimerId_t id_tim_1s;
osStatus_t status_1s;

osTimerId_t id_tim_6s;
osStatus_t status_6s;

osTimerId_t id_tim_3m;
osStatus_t status_3m;

static void SetTimers (void);
void Timer_Callback_1s (void);
void Timer_Callback_6s (void);
void Timer_Callback_3m (void);

/* Timers IDs*/
uint8_t cincoSeg = 0;
uint8_t dosSeg = 0;
uint8_t tim_1s = 0;
uint8_t tim_1s_vcc = 0;
uint8_t timersError = 0;

/* Thread declarations */
static void BlinkLed (void *arg);
static void Display  (void *arg);
static void RealTimeClock (void *arg);
static void AlarmHandler (void *arg);
static void UserHandler (void *arg);

__NO_RETURN void app_main (void *arg);

/* Read analog inputs */
uint16_t AD_in (uint32_t ch) {
  int32_t val = 0;
  
  switch(ch){
    case 0:
      val = ADC_getVoltage(&adchandle, 13);
      break;
    
    case 1:
      val = ADC_getVoltage(&adchandle, 10);
      break;
  }
  return ((uint16_t)val);
}

/* Read digital inputs */
uint8_t get_button (void) {
  return ((uint8_t)Buttons_GetState ());
}

/* IP address change notification */
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

  (void)if_num;
  (void)val;
  (void)len;

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
    /* IP address change, trigger LCD update */
    osThreadFlagsSet (TID_Display, 0x01);
  }
}

/*----------------------------------------------------------------------------
  Thread 'Display': LCD display handler
 *---------------------------------------------------------------------------*/
static __NO_RETURN void Display (void *arg) {
  
  //Inicialización LCD
  LCD_Initialize ();
  uint32_t flagLCD = 0x00;

  (void)arg;
  
//  LEDrun = false;
  
  while(1) {

    flagLCD = osThreadFlagsWait (0x50, osFlagsWaitAll, osWaitForever);
    if(flagLCD == 0x50){
      /* Display user text lines */
      escrituraLCD_V2(1, lcd_text[0]);
      escrituraLCD_V2(2, lcd_text[1]);
    }
    //osDelay(100);
    LCD_Update ();
  }
}

/*----------------------------------------------------------------------------
  Thread 'BlinkLed': Blink the LEDs on an eval board
 *---------------------------------------------------------------------------*/
static __NO_RETURN void BlinkLed (void *arg) {
  const uint8_t led_val[16] = { 0x48,0x88,0x84,0x44,0x42,0x22,0x21,0x11,
                                0x12,0x0A,0x0C,0x14,0x18,0x28,0x30,0x50 };
  uint32_t cnt = 0U;
                                
  (void)arg;

  LEDrun = false; //CAMBIAR
  while(1) {
    /* Every 100 ms */
    if (LEDrun == true) {
      ledsON (led_val[cnt]);
      if (++cnt >= sizeof(led_val)) {
        cnt = 0U;
      }
    }
    osDelay (100);
  }
}

static __NO_RETURN void RealTimeClock (void *arg){
  
  (void)arg;
  osTimerStart(id_tim_6s, 6000);
  
  while(1){
    RTC_Show(aShowTime, aShowDate);
    
    memcpy(lcd_text[0], aShowTime, sizeof(aShowTime));
    lcd_text[0][sizeof(aShowTime)] = '\0';
    
    memcpy(lcd_text[1], aShowDate, sizeof(aShowDate));
    lcd_text[1][sizeof(aShowDate)] = '\0';
    
    //Envía al hilo del LCD para escribir
    osThreadFlagsSet (TID_Display, 0x50);
    
  }
}

static __NO_RETURN void AlarmHandler (void *arg){
  
  (void)arg;
  uint32_t alarmFlag = 0x0U;
  
  while(1){
      
    alarmFlag = osThreadFlagsWait(0x01U, osFlagsWaitAll, osWaitForever);
    if(alarmFlag == 0x01){
      for(cincoSeg = 0; cincoSeg < 10; cincoSeg++){
      HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
      osDelay(500);
      }
    }
  }
}

static __NO_RETURN void UserHandler (void *arg){
  
  (void)arg;
  uint32_t userFlag = 0x0U;
  
  while(1){
    userFlag = osThreadFlagsWait(0x01U, osFlagsWaitAll, osWaitForever);
    
    if(userFlag == 0x01){
      RTC_Time_Config(0, 0, 0);
      RTC_Date_Config(1, 1, 0, 1);
      //Pulso boton usuario -> Enciendo led azul
      for(cincoSeg = 0; cincoSeg < 10; cincoSeg++){
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
        osDelay(100);
      }
    }
  }
}

/**---------------------------------------------------------------------------*
  * Main Thread 'main': Run Network                                           *
  *---------------------------------------------------------------------------**/
__NO_RETURN void app_main (void *arg) {
  (void)arg;

  //Inicialización LED
  LED_Initialize();
  
  //Inicialización Ethernet
  netInitialize ();
  
  //Inicializacion ADC
  ADC1_pins_F429ZI_config ();
  ADC_Init_Single_Conversion (&adchandle, ADC1);
  
  //Inicialización Real Time Clock
  RTC_Init ();
  
  //Init Timers
  SetTimers ();
  
  //Init Botton User
  init_User();
  
  TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
  TID_Display = osThreadNew (Display,  NULL, NULL);
  TID_RTC     = osThreadNew (RealTimeClock, NULL, NULL);
  TID_Alarm   = osThreadNew (AlarmHandler, NULL, NULL);
  TID_User    = osThreadNew (UserHandler, NULL, NULL);
  
  osThreadExit();
}

static void SetTimers (void){
  
  id_tim_1s = osTimerNew((osTimerFunc_t)&Timer_Callback_1s, osTimerPeriodic, NULL, NULL);
  id_tim_6s = osTimerNew((osTimerFunc_t)&Timer_Callback_6s, osTimerOnce, NULL, NULL);
  id_tim_3m = osTimerNew((osTimerFunc_t)&Timer_Callback_3m, osTimerPeriodic, NULL, NULL);
  
  if(id_tim_6s != NULL || id_tim_3m != NULL || id_tim_1s != NULL){
    if(status_6s != osOK || status_3m != osOK || status_1s != osOK){
      timersError = 0;
    }
  }
}

void Timer_Callback_1s (void){

  //Lo enciendo en la configuración inicial y tras 3 minutos
  if(tim_1s < 10){
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
    tim_1s += 1;
  }else{
    osTimerStop(id_tim_1s);
    tim_1s = 0;
  }
}

void Timer_Callback_6s (void){
  
  osTimerStart(id_tim_1s, 200); //Pongo esto para la configuración inicial al conectar Vcc
  SNTP_Init ();
  osTimerStart(id_tim_3m, 180000);
}

void Timer_Callback_3m (void){
  
  osTimerStart(id_tim_1s, 200); //Tras sincronizar
  SNTP_Init ();
}


