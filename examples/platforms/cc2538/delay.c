#include "delay.h"

void delay_us(uint8_t us){
	/* Set RELOAD value */
	STRELOAD = (32-1)*us; 
    /* Enable the Systick, and select CPU Clock Source */
    STCTRL = (1<<SBIT_ENABLE) | (1<<SBIT_CLKSOURCE);
    /*Read CURRENT VALUE until its 0*/
    while(STCURR);
}

void delay_ms(uint8_t ms){
	STRELOAD = (32000-1)*ms;
    /* Enable the Systick, and select CPU Clock Source */
    STCTRL = (1<<SBIT_ENABLE) | (1<<SBIT_CLKSOURCE);
    while(STCURR);
}


void delay_s(uint8_t s){
	STRELOAD = (32000000 - 1)*s;
    /* Enable the Systick, and select CPU Clock Source */
    STCTRL = (1<<SBIT_ENABLE) | (1<<SBIT_CLKSOURCE);
    while(STCURR);
}
