/*
  Autor: Aitor Casado de la Fuente
  Grupo: M04M05M06A
*/

#include "rtc.h"

osMessageQueueId_t mid_MsgQueueRTC; // id de la cola

RTC_HandleTypeDef rtchandler;
RTC_TimeTypeDef rtcTimeConfig = {0};
RTC_DateTypeDef rtcDateConfig = {0};
RTC_AlarmTypeDef alarmConfig  = {0};

uint8_t errorDia = 0;
uint8_t errorHora = 0;
uint8_t errorLSE = 0;
uint8_t errorPeriferico = 0;
uint8_t errorQueue = 0;

t_RTCStruct textoRTC;

volatile bool alarmCheck = false;

/**
  * @brief  Inicialización del RTC
  * @param  None
  * @retval None
  */
void RTC_Init (void){
  
  /*##-1- LSE Initialization */
  init_LSE_Clock ();
  
  __HAL_RCC_RTC_ENABLE ();
  
  /*##-2- Configure the RTC peripheral #######################################*/
  /* Configure RTC prescaler and RTC data registers */
  rtchandler.Instance = RTC; //Asigna la instancia del RTC a la estructura rtchandler, del tipo RTC_HandleTypeDef
  rtchandler.Init.HourFormat = RTC_HOURFORMAT_24; //Configura el modo 24 horas
  rtchandler.Init.AsynchPrediv = RTC_ASYNCH_PREDIV; //Configuración de prescaler asíncrono
  rtchandler.Init.SynchPrediv = RTC_SYNCH_PREDIV; //Configuración de prescaler síncrono
  rtchandler.Init.OutPut = RTC_OUTPUT_DISABLE; //Desactiva a opción de generar la señal de reloj del rtc
  rtchandler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH; //Configura polaridad de salida alta -> Sin efecto si Output está desahabilitado
  rtchandler.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN; //Configura el tipo de salida Open Drain -> Necesitaría resistencia Pull-Up externa si hubiese salida
  __HAL_RTC_RESET_HANDLE_STATE(&rtchandler); //Resetea manejador RTC para que no haya estados previos almacenados en la estructura
  
  if (HAL_RTC_Init(&rtchandler) != HAL_OK)
  {
    /* Initialization Error */
  }
  
    /*##-3- Check if Data stored in BackUp register1: No Need to reconfigure RTC#*/
  /* Read the Back Up Register 1 Data */
  if (HAL_RTCEx_BKUPRead(&rtchandler, RTC_BKP_DR1) != 0x32F2)
  {
    /* Configure RTC Calendar */
//    RTC_Time_Config(hora, min, seg);
//    RTC_Date_Config(dia, mes, anio);
    RTC_SetAlarm ();
  }
  else
  {
    /* Clear source Reset Flag */
    __HAL_RCC_CLEAR_RESET_FLAGS();
  }
  
  
}

/*La razón por la cual se asignan dos prescalers (asíncrono y síncrono) es porque al estar usando el LSE, ya que
  en el lbro indica que si queremos exactitud usemos ese. El LSE tiene un relon típico de 32768 Hz, valor que
  es demasiado rápido para un RTC, por ende se usan dos prescalers.                                               
                                                                                                                 
    - Prescaler asíncrono: divide la frecuencia de la primera etapa, permitiendo ahorrar energía
    - Prescaler síncrono: este controla la resolución del temporizador
                                                                                                                 
  A estos se les asignan dos constantes (asíncrono 0x7F y síncrono 0x00FF). Estos valores corresponden a
  127 y 255, exactamente para generar 1 tick por segundo: 32768/[(127+1)*(255+1)]= 1. Recordatorio de SBM
  que en el prescaler se pone un valor menos siempre.*/

/**
  * @brief  Función que configura la hora
  * @param  Hora, minutos, segundos, dia, mes y años a configurar
  * @retval None
  */
void RTC_Time_Config (uint8_t hh, uint8_t mm, uint8_t ss){
  
  /*##-1- Configure the Time #################################################*/
  /* Set Time: 20:24:58 */
  rtcTimeConfig.Hours = hh;
  rtcTimeConfig.Minutes = mm;
  rtcTimeConfig.Seconds = ss;
  rtcTimeConfig.TimeFormat = RTC_HOURFORMAT_24;
  rtcTimeConfig.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  rtcTimeConfig.StoreOperation = RTC_STOREOPERATION_RESET;
  
  if (HAL_RTC_SetTime(&rtchandler, &rtcTimeConfig, RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    errorHora += 1;
    while(1);
  }
}

/**
  * @brief  Función que configura la fecha
  * @param  Dia, Mes, Año
  * @retval None
  */
void RTC_Date_Config (uint8_t dd, uint8_t ms, uint8_t yr, uint8_t wday){
  
  /*##-2- Configure the Date #################################################*/
  /* Set Date: Sunday March 02nd 2025 */
  rtcDateConfig.Date = dd;
  rtcDateConfig.Month = ms;
  rtcDateConfig.Year = yr; //Sumar 2000 cuando le llamemos
  rtcDateConfig.WeekDay = RTC_WEEKDAY_FRIDAY;
  
  if(HAL_RTC_SetDate(&rtchandler,&rtcDateConfig,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
   // Error_Handler();
    errorDia += 1;
    while(1);
  }
  
  HAL_RTCEx_BKUPWrite(&rtchandler, RTC_BKP_DR1, 0x32F2);
}

void RTC_Show(uint8_t *showtime, uint8_t *showdate){
  
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&rtchandler, &rtcTimeConfig, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&rtchandler, &rtcDateConfig, RTC_FORMAT_BIN);
  
  /* Display time Format : hh:mm:ss */
  sprintf((char *)showtime, "%02d:%02d:%02d", rtcTimeConfig.Hours, rtcTimeConfig.Minutes, rtcTimeConfig.Seconds);
  /* Display date Format : mm-dd-yy */
  sprintf((char *)showdate, "%02d-%02d-%d", rtcDateConfig.Date, rtcDateConfig.Month, 2000 + rtcDateConfig.Year);
}

static void init_LSE_Clock (void){
  
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct = {0};
  RCC_OscInitTypeDef        RCC_OscInitStruct = {0};
  
  /* LSE Enable */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
//  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    errorLSE += 1;
    while (1);  // Error en la configuración
  }
  
  /* Select LSE as RTC source clock*/
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
    errorPeriferico += 1;
    while (1);  // Error en la configuración
  }
}

void RTC_SetAlarm (void){
  
  HAL_RTC_GetTime(&rtchandler, &rtcTimeConfig, RTC_FORMAT_BIN);
  alarmConfig.AlarmTime.Hours = rtcTimeConfig.Hours;
  alarmConfig.AlarmTime.Minutes = rtcTimeConfig.Minutes;
  alarmConfig.AlarmTime.Seconds = 0;
  alarmConfig.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  alarmConfig.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  alarmConfig.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES; //En la máscara se pone lo que NO quiero tener en cuenta
  alarmConfig.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  alarmConfig.AlarmDateWeekDay = 0x1;
  alarmConfig.Alarm = RTC_ALARM_A;
  
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn); //Habilita ambas alarmas
  HAL_RTC_SetAlarm_IT(&rtchandler, &alarmConfig, RTC_FORMAT_BIN);
}
