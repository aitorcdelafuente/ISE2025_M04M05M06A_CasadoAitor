#include "Thread.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread;                        // thread id

osTimerId_t tid_timDist;
osStatus_t status;
uint32_t exec = 2U;

uint8_t distance = 0;
extern uint8_t cm; //From Callback
void Thread (void *argument);                   // thread function
void Init_HCSR04 (void);
static void TimDist_Callback (void);

uint8_t timer_2s = 0;
 
int Init_Thread (void) {
 
  tid_Thread = osThreadNew(Thread, NULL, NULL);
  if (tid_Thread == NULL) {
    return(-1);
  }
  
  tid_timDist = osTimerNew((osTimerFunc_t)&TimDist_Callback, osTimerPeriodic, &exec, NULL);
  if (tid_timDist != NULL) {
    if(status != osOK)
      return -1;
  }
 
  return(0);
}
 
void Thread (void *argument) {
 
  osTimerStart(tid_timDist, 2000U);
  TriggerPulse_Init();
//  EchoPulse_Init ();
  
  while (1) {
    // Insert thread code here...
    
    //Every two seconds
    osThreadFlagsWait(TIMER_EVENT_FLAG, osFlagsWaitAny, osWaitForever);
    
    //Set 10 us TRIGGER
    Init_HCSR04 ();
    
    //Measure done
//    osThreadFlagsWait(WATER_LEVEL, osFlagsWaitAny, osWaitForever);
//    distance = cm;
    
    osThreadYield();                            // suspend thread
  }
}

void Init_HCSR04 (void){
  HAL_TIM_Base_Start_IT(&htim7);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
}

void TimDist_Callback (void){
  osThreadFlagsSet(tid_Thread, TIMER_EVENT_FLAG);
  timer_2s += 1;
}
