#include "sntp.h"

struct tm t_StructSNTP;
netStatus SNTP_OK;
uint8_t errorSNTP = 0;
uint8_t errorSNTP_2 = 0;
uint8_t errorHora_sntp = 0;
uint8_t errorDia_sntp = 0;

extern RTC_HandleTypeDef rtchandler;
extern RTC_TimeTypeDef rtcTimeConfig;
extern RTC_DateTypeDef rtcDateConfig;

static void netSNTP_Callback (uint32_t seconds, uint32_t seconds_fracation);

void SNTP_Init (void){
  SNTP_OK = netSNTPc_GetTime (NULL, netSNTP_Callback); //Primer parámetro NULL ya que ya hemos configurado la IP
  
  if(SNTP_OK != netOK){
    errorSNTP += 1; //Depuration
    while(1);
  }
}

static void netSNTP_Callback (uint32_t seconds, uint32_t seconds_fracation){
  
  if(seconds == 0){
    errorSNTP_2 += 1;
    
  }else{
    //time_t segundosEpoch = (time_t)seconds; //Cambiamos formato variable que almacena los seg. desde 1970 a tipo time_t
                                            //para pasarlo por parámetro a la función localtime
    //En horario de verano, habrá que sumar dos horas
    t_StructSNTP = *localtime(&seconds); //Segundos desde 1 enero de 1970. Se castea a la estructura de tipo tm y lo almacena en la estructura
    
    /* Configure Date */
    rtcDateConfig.Year = t_StructSNTP.tm_year - 100;
    rtcDateConfig.Month = t_StructSNTP.tm_mon + 1;
    rtcDateConfig.Date = t_StructSNTP.tm_mday;
    rtcDateConfig.WeekDay = t_StructSNTP.tm_wday;
    
    if(HAL_RTC_SetDate(&rtchandler, &rtcDateConfig, RTC_FORMAT_BIN) != HAL_OK)
    {
      /* Initialization Error */
     // Error_Handler();
      errorDia_sntp += 1;
      while(1);
    }
    
        /* Configure Time */
    rtcTimeConfig.Hours = t_StructSNTP.tm_hour + 2;
    rtcTimeConfig.Minutes = t_StructSNTP.tm_min;
    rtcTimeConfig.Seconds = t_StructSNTP.tm_sec;
    rtcTimeConfig.TimeFormat = RTC_HOURFORMAT12_PM;
    rtcTimeConfig.DayLightSaving = t_StructSNTP.tm_isdst;
    rtcTimeConfig.StoreOperation = RTC_STOREOPERATION_RESET;
    
    if (HAL_RTC_SetTime(&rtchandler, &rtcTimeConfig, RTC_FORMAT_BIN) != HAL_OK)
    {
    /* Initialization Error */
      errorHora_sntp += 1;
    }
    
    HAL_RTCEx_BKUPWrite(&rtchandler, RTC_BKP_DR1, 0x32F2);
    
    RTC_SetAlarm ();
  }
}

void init_User (void){

  GPIO_InitTypeDef GPIO_InitStruct;

  /*Enable clock to GPIO-C*/
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  
}
