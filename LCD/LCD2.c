/*
===============================================================================
 Name        : LCD.c
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
//punteros systick
unsigned int volatile *const STCTRL  = (unsigned int*)0xE000E010;
unsigned int volatile *const STRELOAD = (unsigned int*)0xE000E014;
unsigned int volatile *const STCURR = (unsigned int*)0xE000E018;

void confGPIO();
void confLCD();
void delay (unsigned int value);
void escribirdato(char val);
void escribircomando(char val);
void enable();
void confsystick();

char Cronometro[10] = {0x43,0x72,0x6F,0x6E,0x6F,0x6D,0x65,0x74,0x72,0x6F};
char Probando[8] = {0x50,0x72,0x6F,0x62,0x61,0x6E,0x64,0x6F};//Probando
char numeros[10] ={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};//numero 0 1 2 3 4 5 6 7 8 9
int cuenta;
int numero;
int main(void)
{

    // TODO: insert code here

    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    cuenta=0;
    numero=1;
    confGPIO();
    confLCD();
    LPC_GPIO2->FIOPIN&=~0x1FF;
    //LPC_GPIO0->FIOSET=(1<<28);//ENABLE
   /*for(int i=0;i<8;i++)
   {
        		escribirdato(Probando[i]);
   }*/
   /*delay(10000);
   escribircomando(0x01);//limpio pantalla

   escribircomando(0x01);//limpio pantalla
   escribircomando(0x02);//chau cursor
   //confsystick();*/
    /*for(int i=0;i<10;i++)
     {
         		escribirdato(Cronometro[i]);
     }*/
    LPC_GPIO2->FIOPIN|=(1<<8);//RS=1
    	LPC_GPIO2->FIOPIN=00000011;//escribe valor
    	enable();

    while(1)
    {

    }
    return 0 ;
}

void unoaldiez()
{
	for(int i=1;i<10;i++)
	{
	  escribirdato(numeros[i]);
	}
}
void confGPIO()
{
	LPC_GPIO2->FIODIR|=0x1FF;//SALIDA DE P2.0 A P2.8
	LPC_GPIO0->FIODIR|=(1<<28);//P0.28
	LPC_GPIO2->FIOCLR=0x1FF;//EMPIEZA TODO APAGADO
}


void confLCD()
{
	char lcd[5] = {0x38,0x0F,0x06,0x01,0x80};//MODO 8 BITS-CURSOR PARPADEANDO-CURSOR A LA DERECHA-PANTALLA CLARA-FORZAR EN PRIMERA LINEA
	for(int i=0;i<5;i++)
	{
		escribircomando(lcd[i]);
	}

}
void escribirdato(char val)
{
	LPC_GPIO2->FIOPIN|=(1<<8);//RS=1
	LPC_GPIO2->FIOPIN=val;//escribe valor
	enable();

}
void enable()
{
	LPC_GPIO0->FIOPIN|=(1<<28);//ENABLE
	delay(50);
	LPC_GPIO0->FIOPIN&=~(1<<28);//DISABLE
	delay(100);
}
void escribircomando(char val)
{
	LPC_GPIO2->FIOPIN&=~(1<<8);//RS=0
	LPC_GPIO2->FIOPIN=val;//escribe valor
	enable();
}

void delay (unsigned int value)
{
	unsigned int i,j;
	for(i=0; i<value; i++)
	{
   	for (j=0; j<3000; j++);
	}
}
void confsystick()
{
	*STCTRL&=~(1<<0);//desabilito sistick counter enable
	*STCTRL&=~(1<<1);//desabilito sistick interrupt
	*STRELOAD=0xF423F;//reload con 999999 para 10 ms

	*STCTRL|=(1<<0);//habilito sistick counter enable
	*STCTRL|=(1<<1);//habilito sistick interrupt
	*STCTRL|=(1<<2);//cpu clock
}

void SysTick_Handler (void)
{
	if(cuenta==100)
	{
	if(numero==10)
		{
			numero=1;
		}
	escribircomando(0x01);//limpio pantalla
	escribircomando(0x0C);//chau cursor
	escribirdato(numeros[numero]);

	 cuenta=0;
	 numero++;
	}
	else
	{
		cuenta++;
	}
	*STCTRL&=~(1<<16);//bajo flag
}
