/*
  Autor: Aitor Casado de la Fuente
  Grupo: M04M05M06A
*/

/* ****************************************************************************************
   * Sensor functionality: this sensor work as a proximity sensor, with a supply          *
   * of 3V3 to 5V. It has two pins: TRIGGER & ECHO. The 'TRIG' pin is the responsable     *
   * for ending the ultrasonic burst. This pin should be set to HIGH at least for 10 us.  *
   * After that, the sensor will send out an eight cycle sonic burst at 40 kHz.           *
   * As it names sais, will eject a trigger wavellength. The 'ECHO' pin, after the sonic  *
   * burst has been sent, this pin will go HIGH. Its main uses is for data, taking data   *
   * measurments. It will stay high until an ultrasonic burst us detected back, at        *
   * which point it will go LOW.                                                          *
   *                                                                                      *
   * Use in the project: we will could calculate the distance between the object          *
   * and the sensor measuring the time the 'ECHO' pin is on HIGH level. The time that     *
   * the pin os at HIGH level is the time the burst is travelling.                        *
   **************************************************************************************** */
   
  
#include "hcsr04.h"

TIM_HandleTypeDef htim4;

/* ****************************************************************************************
   * TRIGGER: we will use the PB2 pin to generate a 10 us HIGH level pulse using an IT    *
   * from TIM7, configurated with to generate a 10 us pulse.                              *
   **************************************************************************************** */
void TriggerPulse_Init (void){
  
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* Trigger Pulse Initialization */
  /* This pin will go HIGH level for at least 10 us */
  
  /* PB2 Configuration */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
  
  printf("Inicializacion TRIGGER");
}

/* ****************************************************************************************
   * ECHO: this pin will be used as an Input Capture TIMER in order to could receive the  *
   * bounced pulse. We are using PB6 pin configurated in AF to generate a TIMER (TIM4)    *
   * working as an Input Capture Timer. The counter of the timer is set to 1 us with a    *
   * prescaler of 83 (84) and with a period of 65535. This is the maximum value that this *
   * timer can reach because it is a 16 bit timer. This gives a measuring window util     *
   * 65.535 us, enough to measure objects from 11 metres from the sensor. The IC timer    *
   * is configured to capture both edges of the wave (up & down) using the maximum        *
   * digital filter we can afford.
   **************************************************************************************** */
void EchoPulse_Init (void){
  
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /*Echo Pulse Initialization */
  /*This pin will receive the bounced wavelength */
  
  /* PB6 Configuration as AF */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
  
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  printf("Inicializacion ECHO");
}

void IC_TIM4_Initialization (void){
  
  TIM_IC_InitTypeDef sConfigIC;
  
  /* TIM4 Configuration as IC */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 83;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  
  HAL_TIM_IC_Init(&htim4);
  
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0xF;
  HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1);
  
  HAL_TIM_IC_Start(&htim4, TIM_CHANNEL_1);
  HAL_NVIC_EnableIRQ(TIM4_IRQn);
  
}

void Measure_Moment (void){
  HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
}