#ifndef DELAY_H
#define DELAY_H

#include "cc2538-reg.h"

#ifdef __cplusplus
 extern "C" {
#endif

/*******REGISTER ADDRESSES******/
#define STCTRL      (*( ( volatile unsigned long *) 0xE000E010 ))
#define STRELOAD    (*( ( volatile unsigned long *) 0xE000E014 ))
#define STCURR      (*( ( volatile unsigned long *) 0xE000E018 ))  

/*******STCTRL bits*******/
#define SBIT_ENABLE     0
#define SBIT_TICKINT    1
#define SBIT_CLKSOURCE  2

// http://www.ti.com/lit/ug/swru319c/swru319c.pdf - Clock operating at 32MHz
#define CLOCK_FREQ 32000000


void delay_us(uint8_t us);
void delay_ms(uint8_t ms);
void delay_s(uint8_t s);


#ifdef __cplusplus
} // end extern "C"
#endif
#endif // DELAY_H
