/*
  Autor: Aitor Casado de la Fuente
  Grupo: M04M05M06A
*/

#include "rtc.h"

RTC_HandleTypeDef rtchandler;
RTC_TimeTypeDef rtcTimeConfig;
RTC_DateTypeDef rtcDateConfig;

/**
  * @brief  Inicializaci�n del RTC
  * @param  None
  * @retval None
  */
void RTC_Init (void){
  /*##-1- Configure the RTC peripheral #######################################*/
  /* Configure RTC prescaler and RTC data registers */
  rtchandler.Instance = RTC; //Asigna la instancia del RTC a la estructura rtchandler, del tipo RTC_HandleTypeDef
  rtchandler.Init.HourFormat = RTC_HOURFORMAT_24; //Configura el modo 24 horas
  rtchandler.Init.AsynchPrediv = RTC_ASYNCH_PREDIV; //Configuraci�n de prescaler as�ncrono
  rtchandler.Init.SynchPrediv = RTC_SYNCH_PREDIV; //Configuraci�n de prescaler s�ncrono
  rtchandler.Init.OutPut = RTC_OUTPUT_DISABLE; //Desactiva a opci�n de generar la se�al de reloj del rtc
  rtchandler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH; //Configura polaridad de salida alta -> Sin efecto si Output est� desahabilitado
  rtchandler.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN; //Configura el tipo de salida Open Drain -> Necesitar�a resistencia Pull-Up externa si hubiese salida
  __HAL_RTC_RESET_HANDLE_STATE(&rtchandler); //Resetea manejador RTC para que no haya estados previos almacenados en la estructura
  
  if (HAL_RTC_Init(&rtchandler) != HAL_OK)
  {
    /* Initialization Error */
  }
}

/*La raz�n por la cual se asignan dos prescalers (as�ncrono y s�ncrono) es porque al estar usando el LSE, ya que
  en el lbro indica que si queremos exactitud usemos ese. El LSE tiene un relon t�pico de 32768 Hz, valor que
  es demasiado r�pido para un RTC, por ende se usan dos prescalers.                                               
                                                                                                                 
    - Prescaler as�ncrono: divide la frecuencia de la primera etapa, permitiendo ahorrar energ�a
    - Prescaler s�ncrono: este controla la resoluci�n del temporizador
                                                                                                                 
  A estos se les asignan dos constantes (as�ncrono 0x7F y s�ncrono 0x00FF). Estos valores corresponden a
  127 y 255, exactamente para generar 1 tick por segundo: 32768/[(127+1)*(255+1)]= 1. Recordatorio de SBM
  que en el prescaler se pone un valor menos siempre.*/

/**
  * @brief  Funci�n que configura la hora
  * @param  Hora, minutos, segundos, dia, mes y a�os a configurar
  * @retval None
  */
void RTC_Time_Config (uint8_t hor, uint8_t min, uint8_t seg){
  
  /*##-1- Configure the Time #################################################*/
  /* Set Time: 20:24:58 */
  rtcTimeConfig.Hours = hor;
  rtcTimeConfig.Minutes = min;
  rtcTimeConfig.Seconds = seg;
  rtcTimeConfig.TimeFormat = RTC_HOURFORMAT_24;
  rtcTimeConfig.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  rtcTimeConfig.StoreOperation = RTC_STOREOPERATION_RESET;
  
  if (HAL_RTC_SetTime(&rtchandler, &rtcTimeConfig, RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
   // Error_Handler();
  }
  
  //Guarda en el registro back-up 1 el dato en hexadecimal. Dicho valor puede ir del 0 al 19
  HAL_RTCEx_BKUPWrite(&rtchandler, RTC_BKP_DR1, 0x32F2); 
}

/**
  * @brief  Funci�n que configura la fecha
  * @param  Dia, Mes, A�o, dia, mes y a�os a configurar
  * @retval None
  */
void RTC_Date_Config (uint8_t dia, uint8_t mes, uint8_t anio){
  
  /*##-2- Configure the Date #################################################*/
  /* Set Date: Sunday March 02nd 2025 */
  rtcDateConfig.Date = dia;
  rtcDateConfig.Month = mes;
  rtcDateConfig.Year = anio; //Sumar 2000 cuando le llamemos
  
  if(HAL_RTC_SetDate(&rtchandler,&rtcDateConfig,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
   // Error_Handler();
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
  
  /* Display user text lines */
  escrituraLCD_V2(1, (char*)showtime);
  escrituraLCD_V2(2, (char*)showdate);
  
  /*Update LCD Info*/
  LCD_Update ();
}