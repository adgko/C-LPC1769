/*
===============================================================================
Name : accesoRegistros.c
Author : $(author)
Version :
Copyright : $(copyright)
Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

// Declaramos punteros que apunten a las direcciones de memoria de los registros asociados al GPIO
#define addrFIO0DIR 0x2009c000;
#define addrFIO0SET 0x2009c018;

unsigned int volatile *const FIO0DIR = (unsigned int*)addrFIO0DIR;
unsigned int volatile *const FIO0SET = (unsigned int*)addrFIO0SET;

//Declaramos Punteros solo que sin la necesidad de usar #define
unsigned int volatile *const FIO0CLR = (unsigned int*)0x2009c01c;
unsigned int volatile *const FIO0PIN = (unsigned int*)0x2009c014;
// TODO: insert other definitions and declarations here

//funciones
void demora(int);
//funciones

int main(void) {

// TODO: insert code here

// Force the counter to be placed into memory
volatile static int i = 0 ;
// Enter an infinite loop, just incrementing a counter
*FIO0DIR|=(1<<22);
//*FIO0DIR&=~(1<<22);
while(1)
{
if(*FIO0PIN&(1<<22))
{
	LPC_GPIO2->FIOCLR=0x1FF;//SALIDA DE P2.0 A P2.8
}
else
{
	LPC_GPIO2->FIOSET=0x1FF;//SALIDA DE P2.0 A P2.8
}
demora(3000000);
}
return 0 ;
}
void demora(int x)
{
for(int i=x;i>0;i--)
{

}
}
