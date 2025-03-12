#ifndef __LEDS_H
#define __LEDS_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

extern void LED_Initialize (void);
extern void ledsON (uint8_t leds);

#endif /* __LEDS_H */
