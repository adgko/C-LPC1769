/*
===============================================================================
 Name        : ledconDifFrec.c
 Author      : Fede
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

#define addrFIO0DIR 0x2009c000;
#define addrFIO0SET 0x2009c018;
//Registros del puerto 2
#define addrFIO2DIR 0x2009c040;
#define addrFIO2SET 0x2009c058;
//#define addrFIO0BUT ;

unsigned int volatile *const FIO0DIR = (unsigned int*)addrFIO0DIR;
unsigned int volatile *const FIO0SET = (unsigned int*)addrFIO0SET;
unsigned int volatile *const FIO2DIR = (unsigned int*)addrFIO2DIR;
unsigned int volatile *const FIO2SET = (unsigned int*)addrFIO2SET;
//Declaramos Punteros solo que sin la necesidad de usar #define
unsigned int volatile *const FIO0CLR = (unsigned int*) 0x2009c01c;
unsigned int volatile *const FIO0PIN = (unsigned int*) 0x2009c014;
unsigned int volatile *const FIO2CLR = (unsigned int*)0x2009c05c;
unsigned int volatile *const FIO2PIN = (unsigned int*)0x2009c054;

void demora(int);

int main(void)
{

    // TODO: insert code here

	// Force the counter to be placed into memory
		volatile static int i = 0;
	// Enter an infinite loop, just incrementing a counter
		*FIO0DIR|=(1<<22);//pongo en 1 salida el pin 22 del puerto 0 LED ROJO
		*FIO2DIR&=~(1<<10);//pongo en 0 (entrada el pin 10 del puerto 0) sw2

		while(1)
		{
			if((1<<10)==*FIO2PIN&(1<<10))//SI EL PIN ESTA EN 1 LOGICO?
			{
				//if(*FIO0PIN&(1<<22))
				//{
					*FIO0SET=(1<<22);
				}
				else
				{
					*FIO0CLR=(1<<22);

				}
				//demora(2000000);
			//}
			/*else
			{
				if(*FIO0PIN&(1<<22))
				{
					*FIO0CLR=(1<<22);
				}
				else
				{
					*FIO0SET=(1<<22);
				}
				demora(4000000);
			}*/
		}
		return 0;
}
void demora(int x)
{
	for (int i = x; i > 0; i--)
	{
	}
}
