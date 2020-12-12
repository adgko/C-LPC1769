/*
===============================================================================
 Name        : T4-displays.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : - 2 Display conectados a la placa LPC,
- Los display cuentan de 00 a 99
- En cada interrupción excterna incremento en 1 la cuenta mostrada.
- En cada interrupción de GPIO decremento la cuenta
===============================================================================
*/
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

//Declaramos Punteros PUERTO 0
unsigned int volatile *const FIO0DIR = (unsigned int*)0x2009C000;
unsigned int volatile *const FIO0SET = (unsigned int*)0x2009C018;
unsigned int volatile *const FIO0CLR = (unsigned int*)0x2009c01c;
unsigned int volatile *const FIO0PIN = (unsigned int*)0x2009c014;
// Declaramos Punteros PUERTO 2
unsigned int volatile *const FIO2DIR = (unsigned int*)0x2009C040;
unsigned int volatile *const FIO2SET = (unsigned int*)0x2009C058;
unsigned int volatile *const FIO2PIN = (unsigned int*)0x2009c054;
unsigned int volatile *const FIO2CLR = (unsigned int*)0x2009c05c;

//Declaramos Punteros PUERTO 3
unsigned int volatile *const FIO3DIR = (unsigned int*)0x2009C060;
unsigned int volatile *const FIO3CLR = (unsigned int*)0x2009c07c;
unsigned int volatile *const FIO3SET = (unsigned int*)0x2009C078;

// Declaramos puntero INTERRUPCIONES ISER0
unsigned int volatile *const ISER0 = (unsigned int*)0xE000E100;

// Declaramos puntero para encender perifericos PCONP
unsigned int volatile *const PCONP =(unsigned int*)0x400FC0C4;

//Declaramos puntero para habilitar interrupciones por GPIO
unsigned int volatile *const IO0IntEnR =(unsigned int*)0x40028090;

//Declaramos puntero para habilitar interrupciones por GPIO
unsigned int volatile *const IO0IntClr =(unsigned int*)0x4002808C;

//Declaramos punteros para seleccionar funcion del pin P2.10
unsigned int volatile *const PINSEL4 =(unsigned int*)0x4002C010;
unsigned int volatile *const PINMODE4 =(unsigned int*)0x4002C050;

//Declaramos puntero a Modo Interrupciones
unsigned int volatile *const EXTMODE =(unsigned int*)0x400FC148;

//Declaramos puntero a polaridad de interrupciones
unsigned int volatile *const EXTPOLAR =(unsigned int*)0x400FC14C;

//Declaramos puntero a bandera de interrupciones
unsigned int volatile *const EXTINT =(unsigned int*)0x400FC140;

//FUNCIONES
void demora(int x);
void unidades();
void decenas();
void confEINT0();
void confEINT3();
int valoru;
int valord;
/////////////
int cuenta;
int main(void)
{
	confEINT0();
	confEINT3();
	//Configuro las salidas del RGB
	*FIO3DIR|=(1<<26); //LED Azul como salida
	*FIO3DIR|=(1<<25); //LED Verde como salida
	*FIO0DIR|=(1<<22); //LED rojo RGB salida

	//apago los leds
	*FIO3SET|=(1<<26); //LED Azul como salida
	*FIO3SET|=(1<<25); //LED Verde como salida
	*FIO0SET|=(1<<22); //LED rojo RGB salida

	*FIO2DIR=0x00;
	*FIO2DIR|=0x3BC0;//pongo puerto 2 pines 6,7,8,9,11,12,13 en salida
	*FIO0DIR|=(1<<27);
	*FIO0DIR|=(1<<28);

	//probando
	*FIO2DIR&=~(1<<10);

	valoru=0;
	valord=0;
	cuenta=0;
	unidades();
	decenas();
	//*FIO0SET=(1<<27);//prendo display unidades
	//*FIO0CLR=(1<<28);//apago display decenas
	//*FIO2SET=0x23C0;
	while(1)
	{
	*FIO0SET=(1<<22);//apago led rojo
	*FIO3SET=(1<<25);//apago led verde
	*FIO0SET=(1<<27);//prendo display unidades
	//*FIO0CLR=(1<<28);
	*FIO2SET=valoru;
	demora(100000);//modificar esto
	*FIO0CLR=(1<<27);//apago display unidades
	*FIO2CLR=0x3BC0;//limpio
	*FIO0SET=(1<<28);//prendodisplay decenas
	*FIO2SET=valord;//paso valor decenas
	demora(100000);//modificar esto
	*FIO0CLR=(1<<28);//apago display decenas
	*FIO2CLR=0x3BC0;//limpio
	}
	return 0 ;
}
void confEINT3()
{
	//Habilito las interrupciones para EINT3
	*ISER0|=(1<<21);
	//Habilitamos interrupcion por GPIO (P0.21)
	*IO0IntEnR|=(1<<21);
}
void confEINT0()
{
	//1°Configuramos el pin P2.10 para EINT0 (01)
	*PINSEL4|=(1<<20);
	*PINSEL4&=~(1<<21);
	//2°Configuramos para pull-up P2.10 (00)
	*PINMODE4&=~(1<<20);
	*PINMODE4&=~(1<<21);
	//3°Configuro para que sea EINT0 sea sensitivo por flanco
	*EXTMODE|=(1<<0);
	//4°Configuro para que sea EINT0 se active por flanco de subida
	*EXTPOLAR|=(1<<0);
	//por las dudas bajo bandera ¿hace falta?
	*EXTINT&=~(1<<0);
	//5°Habilito las interrupciones para EINT0
	*ISER0|=(1<<18);
}
void demora(int x)
{
	for(int i=x;i>0;i--)
	{
	}
}
void unidades()
{
	int valoruaux=0;
	if(cuenta==100)//si la cuenta llego a 100 reinicio
	{
		cuenta =0;
	}else if(cuenta<0)//si la cuenta fue negativa voy a 99
	{
		cuenta=99;
	}
	valoruaux=cuenta;
	while(valoruaux>9)
	{
		valoruaux=valoruaux-10;
	}
	valoru=valoruaux;
	switch(valoru)
	{
		case 0:
			valoru=0x1BC0;//valor en hexa que muestra el 0
			break;
		case 1:
			valoru=0x180;
			break;
		case 2:
			valoru=0x2AC0;
			break;
		case 3:
			valoru=0x23C0;
			break;
		case 4:
			valoru=0x3180;
			break;
		case 5:
			valoru=0x3340;
			break;
		case 6:
			valoru=0x3B40;
			break;
		case 7:
			valoru=0x1C0;
			break;
		case 8:
			valoru=0x3BC0;
			break;
		case 9:
			valoru=0x31C0;
			break;
		default:
			valoru=0;
	}
}
void decenas()
{
	int valordaux=0;
	int aux;
		if(cuenta==100)//si la cuenta llego a 100 reinicio
		{
			cuenta =0;
		}else if(cuenta<0)//si la cuenta fue negativa voy a 99
		{
			cuenta=99;
		}
		aux=cuenta;
		while(aux>9)
		{
			valordaux++;
			aux=aux-10;
		}
		valord=valordaux;
		switch(valord)
		{
			case 0:
				valord=0x1BC0; //valor en hexa que muestra el 0
				break;
			case 1:
				valord=0x180;
				break;
			case 2:
				valord=0x2AC0;
				break;
			case 3:
				valord=0x23C0;
				break;
			case 4:
				valord=0x3180;
				break;
			case 5:
				valord=0x3340;
				break;
			case 6:
				valord=0x3B40;
				break;
			case 7:
				valord=0x1C0;
				break;
			case 8:
				valord=0x3BC0;
				break;
			case 9:
				valord=0x31C0;
				break;
			default:
				valord=0;
		}

}

void EINT0_IRQHandler (void)
{
	*FIO3CLR=(1<<25);//PRENDO LED ROJO
	demora(1000000);
	cuenta++;//aumento uno el contador
	unidades();
	decenas();
	*EXTINT|=(1<<0);
	//*FIO0SET=(1<<22);//PRENDO LED ROJO
}

void EINT3_IRQHandler (void)
{
	*FIO0CLR=(1<<22);//PRENDO LED ROJO
	demora(1000000);
	cuenta--;//decremento en uno el contador
	unidades();
	decenas();
	*IO0IntClr|=(1<<21);
	//*FIO0SET=(1<<22);//APAGO LED ROJO
}
