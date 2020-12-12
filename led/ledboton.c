/*
===============================================================================
 Name        : ledcontimer.c
 Author      : Fede
 Version     :
 Copyright   : $(copyright)
 Description :
 Escribir un código que encienda y apague un led en función del estado lógico de un pin de la placa LPCXpresso.
 Si un pin está en estado alto el led parpadeará
 Si el led está en estado bajo el led no parpaderá
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

// Declaramos punteros que apunten a las direcciones de memoria de los registros asociados al GPIO
#define addrFIO0DIR 0x2009c000;
#define addrFIO0SET 0x2009c018;
#define addrFIO2DIR 0x2009c040;
#define addrFIO2SET 0x2009c058;
#define addrFIO3DIR 0x2009c060;
#define addrFIO3SET 0x2009c078;

unsigned int volatile *const FIO0DIR = (unsigned int*)addrFIO0DIR;
unsigned int volatile *const FIO0SET = (unsigned int*)addrFIO0SET;
unsigned int volatile *const FIO2DIR = (unsigned int*)addrFIO2DIR;
unsigned int volatile *const FIO2SET = (unsigned int*)addrFIO2SET;
unsigned int volatile *const FIO3DIR = (unsigned int*)addrFIO3DIR;//verde y blue
unsigned int volatile *const FIO3SET = (unsigned int*)addrFIO3SET;

//Declaramos Punteros solo que sin la necesidad de usar #define
unsigned int volatile *const FIO0CLR = (unsigned int*)0x2009c01c;
unsigned int volatile *const FIO0PIN = (unsigned int*)0x2009c014;
unsigned int volatile *const FIO2CLR = (unsigned int*)0x2009c05c;
unsigned int volatile *const FIO2PIN = (unsigned int*)0x2009c054;
unsigned int volatile *const FIO3CLR = (unsigned int*)0x2009c07c;
unsigned int volatile *const FIO3PIN = (unsigned int*)0x2009c074;

// TODO: insert other definitions and declarations here

//funciones
void demora(int);
/*
void botonsi()
{
	*FIO0CLR=(1<<22);//rojo
	demora(3000000);
	*FIO0SET=(1<<22);
}
void botonno()
{
	*FIO3CLR=(1<<25);//verde
	demora(3000000);
	*FIO3SET=(1<<25);
}*/
//funciones

int main(void)
{
// TODO: insert code here

// Force the counter to be placed into memory
	volatile static int i = 0 ;
// Enter an infinite loop, just incrementing a counter
	*FIO0DIR|=(1<<22);//pongo en 1 (salida el pin 22 del puerto 0)
	//*FIO2DIR|=0x3800;//pongo en 1 (salida el pin 22 del puerto 0)

	*FIO2DIR&=~(1<<10);//pongo en 0 (entrada el pin 10 del puerto 0) sw2

	//*FIO3DIR|=(1<<25);//pongo en 1 (salida el pin 25 del puerto 3)
	*FIO0SET=(1<<22);
	//*FIO0SET=(1<<22);//en 0 el led
	//*FIO3SET=(1<<25);//en 0 el led
	LPC_GPIO2->FIODIR|=0x1FF;//SALIDA DE P2.0 A P2.8
	while(1)
	{
		if(!(*FIO2PIN&(1<<10)))//si se pulsa el boton entra (esta en 0 logico)
		{
			//LPC_GPIO2->FIOSET=0x1FF;//SALIDA DE P2.0 A P2.8
			*FIO0CLR=(1<<22);//rojo
			//demora(3000000);
			//*FIO2SET=0x3800;
		}else
		{
			//LPC_GPIO2->FIOCLR=0x1FF;//SALIDA DE P2.0 A P2.8
			*FIO0SET=(1<<22);
			//demora(3000000);
			//*FIO3SET=(1<<25);
			//*FIO2CLR=0x3800;
		}

	}
	return 0 ;
}

void demora(int x)
{
	for(int i=x;i>0;i--)
	{

	}
}
