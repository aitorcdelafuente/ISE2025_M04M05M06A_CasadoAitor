#ifndef __LCD_H
#define __LCD_H


#include "Driver_SPI.h"
#include "stdio.h"
#include "string.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

#define S_TRANS_DONE_SPI  0x01

extern void LCD_Update (void);
extern void LCD_Clean (void);
extern void LCD_Initialize (void);
extern void escrituraLCD (uint8_t linea, const char frase[20]);
extern void escrituraLCD_V2 (uint8_t linea, char *texto);
extern void limpiar_L1 (void);
extern void limpiar_L2 (void);

#endif /* __LCD_H */
