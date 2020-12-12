/*
===============================================================================
 Name        : T5-ADC.c
 Author      : FedeBosack
 Version     :
 Copyright   : $(copyright)
 Description : Escriba un programa que realice conversiones a intervalos constantes de 10 ms
 y que para tensiones por debajo de Vdd/2 se encienda un led y para tensiones por encima de
 este valor se encienda otro led. Cada vez que se enciende un led se apaga el otro.
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

//CONF. DE PUERTOS 0 Y 3 - GPIO
unsigned int volatile *const FIO0DIR   = (unsigned int*)0x2009C000;
unsigned int volatile *const FIO0SET   = (unsigned int*)0x2009C018;
unsigned int volatile *const FIO0CLR   = (unsigned int*)0x2009C01C;
unsigned int volatile *const FIO0PIN   = (unsigned int*)0x2009C014;
unsigned int volatile *const FIO0MASK  = (unsigned int*)0x2009C010;
unsigned int volatile *const FIO3DIR   = (unsigned int*)0x2009C060;
unsigned int volatile *const FIO3SET   = (unsigned int*)0x2009C078;
unsigned int volatile *const FIO3CLR   = (unsigned int*)0x2009C07C;

//CONFIGURO PUNTEROS PARA INTERRUPCIONES
	// Puntero INTERRUPCIONES ISER0
unsigned int volatile *const ISER0 = (unsigned int*)0xE000E100;
	// Puntero para encender perifericos PCONP
unsigned int volatile *const PCONP =(unsigned int*)0x400FC0C4;
	//PCLKSEL0 elije el periferico al que le asigna el clock
unsigned int volatile *const PCLKSEL0 = (unsigned int*)0x400FC1A8;

//CONFIGURACION DEL ADC:
unsigned int volatile *const AD0CR  	= (unsigned int*)0x40034000;
unsigned int volatile *const PINSEL1   	= (unsigned int*)0x4002C004;
unsigned int volatile *const PINMODE1  	= (unsigned int*)0x4002C044;
unsigned int volatile *const ADINTEN0	= (unsigned int*)0x4003400C;
unsigned int volatile *const AD0GDR		= (unsigned int*)0x40034004;

//Punteros para el st
unsigned int volatile *const STCTRL  = (unsigned int*)0xE000E010;
unsigned int volatile *const STRELOAD = (unsigned int*)0xE000E014;
unsigned int volatile *const STCURR = (unsigned int*)0xE000E018;

//CONFIGURACION DEL TIMER:
unsigned int volatile *const T0PR = (unsigned int*)0x4000400C; //Preescaler register, cuando el preescaler counter alcanza el valor almacenado aca, se incrementa el TC (TimerCounter)
unsigned int volatile *const T0MCR = (unsigned int*)0x40004014;
unsigned int volatile *const T0CR  = (unsigned int*)0x40004004;
unsigned int volatile *const T0IR  = (unsigned int*)0x40004000;// bit de interrupcion (flag)
unsigned int volatile *const T0MR0 = (unsigned int*)0x40004018;
unsigned int volatile *const T0TCR = (unsigned int*)0x40004004;//registro del TCR, sirve para controlar las funciones del timer counter

//FUNCIONES
void confADC();
void confDAC();
void confTMR0();
//void confsystick();
void sendDAC(int valor);
int a;
int main(void) {

    // TODO: insert code here

    // Force the counter to be placed into memory
    volatile static int i = 0 ;

    *FIO0DIR|=(1<<22);
    *FIO3DIR|=(1<<25);
    *FIO0SET|=(1<<22);
    *FIO3SET|=(1<<25);

    //*FIO0DIR|=(1<<27);
    *FIO0DIR|=(1<<28);
    //*FIO0SET|=(1<<27);
    *FIO0CLR|=(1<<28);

    //confDAC();
    confTMR0();
    confADC();



    while(1)
    {
    }
    return 0 ;
}
void confTMR0()
{
	*PCONP|=(1<<1);//enciendo PCONP para timer0
	// Habilito el clock para el timer0
	*PCLKSEL0&=~(1<<2); //para timer 0 el bit 2 y 3 en 00
	*PCLKSEL0&=~(1<<3);
	//Configuracion del Timer0
	*T0PR = 0; //Al poner el Preescaler igual a 0, el timer counter se incrementara con cada pulso de clock
	*T0MR0 = 250000;//10ms)
	//Configuracion del MCR
	*T0MCR|=(1<<0); // Se genera una interrupcion cuando el MR0 se matchea con el TC (timer counter)
	*T0MCR|=(1<<1);//RESETEA CUANDO MATCHEA
	// INICIO EL TIMER COUNTER DEL TIMER 0
	*T0TCR|=(1<<0);
	//habilito interrupcion por timer0
	*ISER0|=(1<<1);
}
void confDAC()
{
	LPC_PINCON->PINSEL1&=~(1<<20);//p0.26
	LPC_PINCON->PINSEL1|=(1<<21);
	LPC_PINCON->PINMODE1 &= ~(1UL << 20); // pin 0.26 en pullup (?)
	LPC_PINCON->PINMODE1 &= ~(1UL << 21);
	LPC_SC->PCLKSEL0 &= ~(1UL << 22);
	LPC_SC->PCLKSEL0 &= ~(1UL << 23); // PCLK en 100MHz/4 = 25MHz
}
void sendDAC(int valor)
{
	LPC_DAC->DACR=(valor&0x3FF)<<6;
	LPC_DAC->DACR = (valor & 0x3FF/*and 10 bits*/) << 6;
}
void confADC() {

	*PCONP |= (1<<12);//habilito periferico ADC
	*AD0CR = 0;
	*AD0CR |= (1<<21);
	*AD0CR |= (1<<0); //PARA HABILITAR EL CANAL 0
	//*AD0CR |= (1<<2); //PARA HABILITAR EL CANAL 1
	*AD0CR &=~ (1<<16); //MODO BURST desACTIVADO
	*AD0CR &= ~(1UL << 24); // Start en 000 (para funcionamiento burst)
	*AD0CR &= ~(1UL << 25);
	*AD0CR &= ~(1UL << 26);
	//CONFIGURACION DEL CLOCK DEL ADC (por defecto 00 -> CCLK/4 --> CCLK/8 (11))
	*PCLKSEL0 |= (1<<25) | (1<<24); 	//QUEDA UNA VEL DE 12.5K
	//TAMBIEN PODEMOS DIRECTAMENTE MODIFICAR EL CLK DEL AD0CR PERO ES MAS FACIL ESTE

	*PINSEL1 |= (1<<14) | (0<<15); 		//P0.23 COMO ADC

	*PINMODE1 |= (1<<15) | (0<<14); 	//DESACTIVO RESIST PULL-UP Y PULL-DOWN

	//*PINSEL1 |= (1<<18);
	//*PINSEL1 &=~ (1<<19); 		//P0.26 COMO ADC
	//*PINMODE1 |= (1<<19);
	//*PINMODE1 &=~(0<<18); 	//DESACTIVO RESIST PULL-UP Y PULL-DOWN

	*ADINTEN0 |= (1<<0);
	*ISER0 |= (1<<22); //HABILITO INTERRUPCIONES
}

void TIMER0_IRQHandler(void)
{
	*T0IR|=(1<<0);
	*AD0CR|=(1<<24);//empiezo adc

}


void ADC_IRQHandler (void)
{

	//if(*AD0GDR&(1<<15))
			/*{		//COMPARANDO 1 CON BIT 15 DEL AG0GDR (MSB RESULT)
				*FIO0SET|=(1<<28);

			}
			else{
				*FIO0CLR|=(1<<28);

			}*/
	//CON LED RGB
	if(*AD0GDR&(1<<15))
		{		//COMPARANDO 1 CON BIT 15 DEL AG0GDR (MSB RESULT)
			*FIO0CLR|=(1<<22);
			*FIO3SET|=(1<<25);
			*FIO0SET|=(1<<28);
			//*FIO0CLR|=(1<<27);
		}
		else{
			*FIO0CLR|=(1<<28);
			*FIO0SET|=(1<<22);
			*FIO3CLR|=(1<<25);
			//*FIO0SET|=(1<<27);
		}
	a=(LPC_ADC->ADGDR & 0xFFFF) >> 4;
	//sendDAC(a);

}

