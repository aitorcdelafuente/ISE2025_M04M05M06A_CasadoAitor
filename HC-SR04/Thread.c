#include "Thread.h"

/*----------------------------------------------------------------------------
 *  OPERATING CONCEPT:                                                        
                                                                               
 *  1. It is necessary to make a 10 us pulse (at least) to the pin TRG in
    order to start the measurment.
    2. Follow-up, the sensor will send out an eight cycle sonic burst at 
    40 kHz, setting the ECHO pin at HIGH level.
    3. This pin (ECHO) will stand out at HIGH level until we receive the
    'echo' pin bounce from the object.
    4. Finally, in order to stablish a correct distance, we only have to
    measure the duration at HIGH level of the pin ECHO, using the formule
    cm = us/58 = us*0.01724.
    
    *WARNING NOTES*:
 *---------------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_HCSR04': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread;                        // thread id

osTimerId_t tid_timDist;
osStatus_t status;
uint32_t exec = 2U;

float distance = 0.0;
extern float cm; //From Callback
void Thread (void *argument);                   // thread function
void Init_HCSR04 (void);
void DWT_Delay_Init(void);
void DWT_Delay_us(uint32_t us);
static void TimDist_Callback (void);

uint8_t timer_2s = 0;
 
int Init_Thread (void) {
 
  tid_Thread = osThreadNew(Thread, NULL, NULL);
  if (tid_Thread == NULL) {
    return(-1);
  }
  
  /*We will use his timer to do a measure every 10 seconds.*/
  tid_timDist = osTimerNew((osTimerFunc_t)&TimDist_Callback, osTimerPeriodic, &exec, NULL);
  if (tid_timDist != NULL) {
    if(status != osOK)
      return -1;
  }
 
  return(0);
}
 
void Thread (void *argument) {
 
  osTimerStart(tid_timDist, 10000U);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
  TriggerPulse_Init();
  EchoPulse_Init ();
  IC_TIM4_Initialization ();
  uint32_t flagWTR = 0x00000000;
  uint32_t flagTIM = 0x00000000;
  printf("Se crea el hilo");
  
  while (1) {
    // Insert thread code here...
    
    //Every ten seconds does a measure
    flagTIM = osThreadFlagsWait(TIMER_EVENT_FLAG, osFlagsWaitAny, osWaitForever);
    
    if(flagTIM == 0x02U){
      flagTIM = 0x0U;
      //Set 10 us TRIGGER
      Init_HCSR04 ();
      Measure_Moment ();
    }
    
    //Measure done
    flagWTR = osThreadFlagsWait(WATER_LEVEL, osFlagsWaitAny, osWaitForever);

    if(flagWTR == 0x01U){
      flagWTR = 0x0U;
      distance = cm;
    }
    
    
    //osThreadYield();                            // suspend thread
  }
}

void TimDist_Callback (void){
  osThreadFlagsSet(tid_Thread, TIMER_EVENT_FLAG);
  timer_2s += 1;
  printf("Timer 10 segundos");
}

/*Here we will generate the Trigger pulse every time
  we want to do a measure. This pulse has to be, at least,
  10 us long. Less than that time we won't get a
  response from the sensor.*/
void Init_HCSR04 (void){
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
  DWT_Delay_us (2); //Wait for 2 us
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
  DWT_Delay_us (10); //Wait for 10 us
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
}

/*This both functions are unique from this module. They are designed to
  generate/implement high-precision delays using the Data Watchpoint
  and Trace (DWT), specifically for microsecond delays. We cannot use 
  osDelay() due to it receives arguments in ms, and it is impossible
  to introduce decimal numbers as arguments.*/

/*Initializes the DWT cycle counter to measure delays based on CPU cycles.*/
void DWT_Delay_Init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; //Enable DWT access setting bit TRCENA in register DEMCR
    DWT->CYCCNT = 0;  // Resets the cycle counter (CYCCNT) of the DWT to zero
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // Enable the cycle counter (CYCCNT) to start counting
}

/*It generates a delay in microseconds (µs) with high precision.*/
void DWT_Delay_us(uint32_t us) {
    uint32_t start = DWT->CYCCNT; //Saves the current value of the cycle counter (CYCCNT) as the starting point.
    uint32_t ticks = (HAL_RCC_GetHCLKFreq() / 1000000) * us; // Calculates the number of CPU cycles required for the desired delay
    while ((DWT->CYCCNT - start) < ticks); // Wait in a loop until the cycle counter (CYCCNT) has incremented the calculated amount (ticks).
}


