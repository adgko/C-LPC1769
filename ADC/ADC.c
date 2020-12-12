/*
===============================================================================
 Name        : ADC.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

// Declaramos puntero INTERRUPCIONES ISER0
unsigned int volatile *const ISER0 = (unsigned int*)0xE000E100;

// Declaramos puntero para encender perifericos PCONP
unsigned int volatile *const PCONP =(unsigned int*)0x400FC0C4;
// Declaramos puntero para AD0CR
unsigned int volatile *const AD0CR =(unsigned int*)0x40034000;
//PCLKSEL0 elije el periferico al que le asigna el clock
unsigned int volatile *const PCLKSEL0 = (unsigned int*)0x400FC1A8;

int main(void)
{

	 	 //habilito interrupcion para ADC
	    *ISER0|=(1<<22);

	    //enciendo PCONP para ADC
	    *PCONP|=(1<<12);
	    *AD0CR|=(1<<21);

	    // Habilito el clock para el ADC
	    *PCLKSEL0|=(1<<24); //
	    *PCLKSEL0|=(1<<25);
/* otra configuracion
 *
 * *AD0CR&=~(1<<21);
 * *PCONP|=(1<<12);
 * *AD0CR|=(1<<21);
 * *PINSEL1|=(1<<14);//CANAL 0 P0.14
 * *PCLKSEL0|=(1<<24);
 *						//*PCLKSEL0|=(1<<25);->NO LO USA
 *
 *
 *
 *
 *
 *
 */


    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
    }
    return 0 ;
}
