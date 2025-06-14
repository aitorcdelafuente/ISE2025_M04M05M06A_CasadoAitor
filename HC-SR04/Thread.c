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
extern TIM_HandleTypeDef htim4;

/* Cambiar dependiendo de la altura del depósito */
#define ALTURA_DEPO_CM 11.8f 

float quantity = 0.0;
void Thread (void *argument);                   // thread function
void Init_HCSR04 (void);
void delay(uint32_t n_microsegundos);
float getMeasure (void);

TIM_HandleTypeDef tim7;
 
int Init_Thread (void) {
 
  tid_Thread = osThreadNew(Thread, NULL, NULL);
  if (tid_Thread == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread (void *argument) {
  
  GPIO_HCSR04 ();
  IC_TIM4_Initialization ();
  initMBED_leds ();
  
  while (1) {
    // Insert thread code here...
    quantity = getMeasure();
    if (quantity >= 0.0f){
      ledsON(quantity);
      printf("Nivel de agua en el tanque: %.1f%%\n", quantity);
    }else{
      printf("Error en la medida\n");
    }
    
    osDelay(10000);
    
    //osThreadYield();                            // suspend thread
  }
}

/*Here we will generate the Trigger pulse every time
  we want to do a measure. This pulse has to be, at least,
  10 us long. Less than that time we won't get a
  response from the sensor.*/
void Init_HCSR04(void) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);  // TRIG LOW
    delay(2);  // Espera breve antes de enviar pulso
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);    // TRIG HIGH
    delay(10);  // Pulso de 10 µs
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);  // TRIG LOW
}

/*This both functions are unique from this module. They are designed to
  generate/implement high-precision delays using the Data Watchpoint
  and Trace (DWT), specifically for microsecond delays. We cannot use 
  osDelay() due to it receives arguments in ms, and it is impossible
  to introduce decimal numbers as arguments.*/

/*Initializes the DWT cycle counter to measure delays based on CPU cycles.*/
void delay(uint32_t n_microsegundos) {
    __HAL_RCC_TIM7_CLK_ENABLE();

    tim7.Instance = TIM7;
    tim7.Init.Prescaler = 83;                 // 84 MHz / (83+1) = 1 MHz ? 1 µs por cuenta
    tim7.Init.Period = n_microsegundos - 1;
    tim7.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim7.Init.CounterMode = TIM_COUNTERMODE_UP;

    HAL_TIM_Base_Init(&tim7);
    HAL_TIM_Base_Start(&tim7);

    while (__HAL_TIM_GET_COUNTER(&tim7) < (n_microsegundos - 1));

    HAL_TIM_Base_Stop(&tim7);
    HAL_TIM_Base_DeInit(&tim7);
}

float getMeasure(void) {
    uint32_t start = 0, end = 0, timeout = 30000;
    uint32_t counter = 0;
    float width = 0.0f, distance = 0.0f, waterLevel = 0.0f;

    __HAL_TIM_SET_COUNTER(&htim4, 0);
    HAL_TIM_Base_Start(&htim4);

    Init_HCSR04();  // Generar pulso

    // Esperar flanco de subida en ECHO
    while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == GPIO_PIN_RESET) {
        counter = __HAL_TIM_GET_COUNTER(&htim4);
        if (counter > timeout) {
            HAL_TIM_Base_Stop(&htim4);
            return -1.0f;  // Timeout esperando HIGH
        }
    }

    start = __HAL_TIM_GET_COUNTER(&htim4);

    // Esperar flanco de bajada
    while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == GPIO_PIN_SET) {
        counter = __HAL_TIM_GET_COUNTER(&htim4);
        if (counter - start > timeout) {
            HAL_TIM_Base_Stop(&htim4);
            return -1.0f;  // Timeout esperando LOW
        }
    }

    end = __HAL_TIM_GET_COUNTER(&htim4);
    HAL_TIM_Base_Stop(&htim4);

    // Calcular duración
    width = (end >= start) ? (end - start) : (0xFFFF - start + end);
    distance = width / 58.0f;
    waterLevel = ALTURA_DEPO_CM - distance;

    if (waterLevel < 0.0f) return 0.0f;
    else if (waterLevel > ALTURA_DEPO_CM) return 100.0f;
    else return (waterLevel / ALTURA_DEPO_CM) * 100.0f;
}
