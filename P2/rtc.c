/*
  Autor: Aitor Casado de la Fuente
  Grupo: M04M05M06A
*/

#include "rtc.h"

RTC_HandleTypeDef rtchandler;
RTC_TimeTypeDef rtcTimeConfig;
RTC_DateTypeDef rtcDateConfig;

uint8_t errorDia = 0;
uint8_t errorHora = 0;
uint8_t errorLSE = 0;
uint8_t errorPeriferico = 0;

uint8_t hora = 0x14;
uint8_t min = 0x24;
uint8_t seg = 0x32;

uint8_t dia = 0x03;
uint8_t mes = RTC_MONTH_MARCH;
uint8_t anio = 0x25;
uint8_t weekDay = RTC_WEEKDAY_MONDAY;

/**
  * @brief  Inicialización del RTC
  * @param  None
  * @retval None
  */
void RTC_Init (void){
  
  /* LSE Initialization */
  init_LSE_Clock ();
  
  __HAL_RCC_RTC_ENABLE ();
  
  /*##-1- Configure the RTC peripheral #######################################*/
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
  
    /*##-2- Check if Data stored in BackUp register1: No Need to reconfigure RTC#*/
  /* Read the Back Up Register 1 Data */
  if (HAL_RTCEx_BKUPRead(&rtchandler, RTC_BKP_DR1) != 0x32F2)
  {
    /* Configure RTC Calendar */
    RTC_Time_Config();
    RTC_Date_Config();
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
void RTC_Time_Config (void){
  
  /*##-1- Configure the Time #################################################*/
  /* Set Time: 20:24:58 */
  rtcTimeConfig.Hours = hora;
  rtcTimeConfig.Minutes = min;
  rtcTimeConfig.Seconds = seg;
  rtcTimeConfig.TimeFormat = RTC_HOURFORMAT_24;
  rtcTimeConfig.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  rtcTimeConfig.StoreOperation = RTC_STOREOPERATION_RESET;
  
  if (HAL_RTC_SetTime(&rtchandler, &rtcTimeConfig, RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    errorHora += 1;
  }
  
  //Guarda en el registro back-up 1 el dato en hexadecimal. Dicho valor puede ir del 0 al 19
//  HAL_RTCEx_BKUPWrite(&rtchandler, RTC_BKP_DR1, 0x32F2); 
}

/**
  * @brief  Función que configura la fecha
  * @param  Dia, Mes, Año, dia, mes y años a configurar
  * @retval None
  */
void RTC_Date_Config (void){
  
  /*##-2- Configure the Date #################################################*/
  /* Set Date: Sunday March 02nd 2025 */
  rtcDateConfig.Date = dia;
  rtcDateConfig.Month = mes;
  rtcDateConfig.Year = anio; //Sumar 2000 cuando le llamemos
  rtcDateConfig.WeekDay = weekDay;
  
  if(HAL_RTC_SetDate(&rtchandler,&rtcDateConfig,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
   // Error_Handler();
    errorDia += 1;
  }
  
  HAL_RTCEx_BKUPWrite(&rtchandler, RTC_BKP_DR1, 0x32F2);
}

void RTC_Show(uint8_t *showtime, uint8_t *showdate){
  
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&rtchandler, &rtcTimeConfig, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&rtchandler, &rtcDateConfig, RTC_FORMAT_BIN);
  
  /* Display time Format : hh:mm:ss */
  sprintf((char *)showtime, "%2d:%2d:%2d", rtcTimeConfig.Hours, rtcTimeConfig.Minutes, rtcTimeConfig.Seconds);
  /* Display date Format : mm-dd-yy */
  sprintf((char *)showdate, "%2d-%2d-%2d", rtcDateConfig.Date,rtcDateConfig.Month, 2000 + rtcDateConfig.Year);
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
