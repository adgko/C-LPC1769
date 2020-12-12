/*
===============================================================================
 Name        : LCD.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : P.0.28->ENABLE
 	 	 	   P.2.8->RS
 	 	 	   P.2.7->D7
 	 	 	   .
 	 	 	   P.2.0->D0
 Vss a ground , Vdd a 5v ,Rw con resistencia 4.7k a ground , A a 5v, K con R220 a ground
 IMPORTANTE----->> CONECTAR GROUND DE LPC CON GROUND DE CARGADOR
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif

#include <cr_section_macros.h>
//punteros systick
unsigned int volatile *const STCTRL  = (unsigned int*)0xE000E010;
unsigned int volatile *const STRELOAD = (unsigned int*)0xE000E014;
unsigned int volatile *const STCURR = (unsigned int*)0xE000E018;
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

void confGPIO();
void confLCD();
void delay (unsigned int value);
void escribirdato(char val);
void escribircomando(char val);
void enable();
void confsystick();
void escribircadena(char *string);
void confADC();
void verificarposicion(int x,int y);
int cuenta;
int numero;
int x,y;
int promxi=0;
int promyi=0;
int promx[10];
int promy[10];
int flagx=0;
int flagy=0;

int main(void)
{

	//int hola=0x31;//0
    // TODO: insert code here

    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    cuenta=0;
    numero=1;
    confGPIO();
    confLCD();

    LPC_GPIO2->FIOCLR=0x1FF;

   escribircomando(0x01);//limpio pantalla
   escribircomando(0x02);//chau cursor
   escribircadena("TP FINAL");
   delay(10000);
   escribircomando(0x01);//limpio pantalla
   escribircomando(0x02);//chau cursor
   LPC_GPIO0->FIODIR|=(1<<22);
   LPC_GPIO3->FIODIR|=(1<<25);
   LPC_GPIO0->FIOCLR=(1<<22);

   LPC_GPIO0->FIODIR&=~(1<<2);//->Y
   LPC_GPIO0->FIODIR&=~(1<<3); //->X
   confADC();
   //confsystick();

    while(1)
    {

    	escribircomando(0x01);//limpio pantalla
    	escribircomando(0x02);//chau cursor
    	escribircadena("while");
    }
    return 0 ;
}

/*void unoaldiez()
{
	for(int i=1;i<10;i++)
	{
	  escribirdato(numeros[i]);
	}
}*/
void confGPIO()
{
	LPC_GPIO2->FIODIR|=0x1FF;//SALIDA DE P2.0 A P2.8
	LPC_GPIO0->FIODIR|=(1<<28);//P0.28
	LPC_GPIO2->FIOPIN&=~0x1FF;//EMPIEZA TODO APAGADO

	//para leds
	LPC_GPIO0->FIODIR|=(1<<6);//P0.6 derecha
	LPC_GPIO0->FIODIR|=(1<<7);//P0.7 izquierda
	LPC_GPIO0->FIODIR|=(1<<27);//P0.8 arriba
	LPC_GPIO0->FIODIR|=(1<<9);//P0.9 abajo
	LPC_GPIO0->FIODIR|=(1<<18);//P0.18 centro

	LPC_GPIO0->FIOCLR=(1<<6);//P0.6 derecha
	LPC_GPIO0->FIOCLR=(1<<7);//P0.7 izquierda
	LPC_GPIO0->FIOCLR=(1<<27);//P0.8 arriba
	LPC_GPIO0->FIOCLR=(1<<9);//P0.9 abajo
	LPC_GPIO0->FIOCLR=(1<<18);//P0.18 centro
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
	LPC_GPIO2->FIOSET=val;//escribe valor
	enable();
	LPC_GPIO2->FIOCLR=0x1FF;//LIMPIO

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
	LPC_GPIO2->FIOCLR=0x1FF;
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
//LPC_ADC->ADCR|=(1<<24);//EMPIEZA A CONVERTIR
	/*if(cuenta==100)
	{
		cuenta=0;

	if(numero==10)
		{
			numero=1;
		}
	/char numero1[1];
	itoa(numero,numero1,10);
	escribircomando(0x01);//limpio pantalla
	escribircomando(0x0C);//chau cursor
	escribirdato(*numero1);


	 //numero++;
	}
	else
	{
		cuenta++;
	}*/
	*STCTRL&=~(1<<16);//bajo flag
}

//SON LAS QUE ME DIERON PROBLEMAS


void escribircadena(char *string)
{
	int c=0;
	while (string[c]!='\0')
	{
		escribirdato(string[c]);
		c++;
	}
}

void confADC()
{

	*PCONP |= (1<<12);//habilito periferico ADC
	*AD0CR = 0;
	*AD0CR |= (1<<21);

	*AD0CR |= (1<<0); //PARA HABILITAR EL CANAL 0
	*AD0CR |= (1<<1); //PARA HABILITAR EL CANAL 1
	*AD0CR |= (1<<16); //MODO BURST ACTIVADO
	*AD0CR &= ~(1UL << 24); // Start en 000 (para funcionamiento burst)
	*AD0CR &= ~(1UL << 25);
	*AD0CR &= ~(1UL << 26);
	//CONFIGURACION DEL CLOCK DEL ADC (por defecto 00 -> CCLK/4 --> CCLK/8 (11))
	*PCLKSEL0 |= (1<<25) | (1<<24); 	//QUEDA UNA VEL DE 12.5K
	//TAMBIEN PODEMOS DIRECTAMENTE MODIFICAR EL CLK DEL AD0CR PERO ES MAS FACIL ESTE

	*PINSEL1 |= (1<<14)| (0<<15); 		//P0.23 COMO ADC
	*PINMODE1 |= (1<<15) | (0<<14); 	//DESACTIVO RESIST PULL-UP Y PULL-DOWN

	*PINSEL1 |= (1<<16) | (0<<17); 		//P0.24 COMO ADC
	*PINMODE1 |= (1<<17) | (0<<16); 	//DESACTIVO RESIST PULL-UP Y PULL-DOWN



	*ADINTEN0 |= (1<<0);
	*ADINTEN0 |= (1<<1);
	*ISER0 |= (1<<22); //HABILITO INTERRUPCIONES
	LPC_ADC->ADCR|=(1<<24);//EMPIEZA A CONVERTIR

}
/*
void confADC()
{
	// configuracion basica
	LPC_SC->PCONP |= (1<<12); // encender el modulo ADC (power y clock)
	LPC_ADC->ADCR = 0;
	LPC_ADC->ADCR |= (1<<21); // encender el modulo ADC

	LPC_ADC->ADCR |= (1<<0); // seleccion de canal 0 para realizar la converison
	//LPC_ADC->ADCR |= (1<<1); // seleccion de canal 1 para realizar la converison
	LPC_ADC->ADCR |= (1<<3); // seleccion de canal 2 para realizar la converison

	LPC_ADC->ADCR &=~(1<<16);//desHABILITO BURST
	LPC_ADC->ADCR &= ~(1UL << 24); // Start en 000 (para funcionamiento burst)
	LPC_ADC->ADCR &= ~(1UL << 25);
	LPC_ADC->ADCR &= ~(1UL << 26);
	LPC_SC->PCLKSEL0 |= (1UL << 24);
	LPC_SC->PCLKSEL0 |= (1UL << 25); // PCLK en 100MHz/8 = 12.5MHz

	LPC_PINCON->PINSEL1 |= (1UL << 14);
	LPC_PINCON->PINSEL1 &= ~(1UL << 15); // pin0.23 en modo AD0.0
	LPC_PINCON->PINMODE1 &= ~(1UL << 14);
	LPC_PINCON->PINMODE1 &= ~(1UL << 15); // pin0.23 con pull-up (?)

	LPC_PINCON->PINSEL1 |= (1UL << 20);
	LPC_PINCON->PINSEL1 &= ~(1UL << 21); // pin0.26 en modo AD0.3
	LPC_PINCON->PINMODE1 &= ~(1UL << 20);
	LPC_PINCON->PINMODE1 &= ~(1UL << 21); // pin0.26 con pull-up (?)
	//
	LPC_ADC->ADINTEN &=~ (1UL << 8);
	//LPC_ADC->ADINTEN |=(1<<0);//HABILITO INTERRPCIONES PARA LOS 3 CANALES
	//LPC_ADC->ADINTEN |=(1<<1);
	LPC_ADC->ADINTEN |=(1<<3);
	//LPC_ADC->ADCR |= (1UL << 0); // seleccion de canal 0 para realizar la converison
	//LPC_ADC->ADCR &= ~(1UL << 16); // modo burst desact
	NVIC_EnableIRQ(ADC_IRQn);//-> ver eso
}*/
void ADC_IRQHandler (void)
{
	//x=0;
	//y=0;
	//escribircomando(0x01);//limpio pantalla
	//escribircomando(0x02);//chau cursor
	//escribircadena("....");
	//LPC_GPIO0->FIOSET=(1<<22);
	if(LPC_ADC->ADSTAT&(1<<0))//canal 0?
	{
		x=(LPC_ADC->ADGDR& 0xFFF0) >> 4;
		/*promx[promxi]=(LPC_ADC->ADGDR& 0xFFF0) >> 4;
		x=x+promx[promxi];
		promxi++;

		if(promxi==10)
		{
			promxi=0;
			x=x/10;
			flagx=1;
		}
		//char equis[12];
		//itoa(equis,x,16);
		//escribircomando(0x01);//limpio pantalla
		//escribircomando(0x0C);//chau cursor
		//escribircadena("X=");
		//escribircadena(equis);
		//escribirdato(x);*/
		//LPC_GPIO0->FIOCLR=(1<<22);
		//delay(10);
		//LPC_GPIO0->FIOSET=(1<<22);
	}
	if(LPC_ADC->ADSTAT&(1<<1))//canal 1
	{
		y=(LPC_ADC->ADGDR & 0xFFF0) >> 4;
		/*promy[promyi]=(LPC_ADC->ADGDR & 0xFFF0) >> 4;
		y=y+promy[promyi];
		promyi++;
		if(promyi==10)
		{
			y=y/10;
			promyi=0;
			flagy=1;
		}
		//escribircomando(0x01);//limpio pantalla
		//escribircomando(0x0C);//chau cursor
		//escribircadena("Y=");
		//escribirdato(y);

		/*LPC_GPIO3->FIOCLR=(1<<25);
		delay(10);
		LPC_GPIO3->FIOSET=(1<<25);*/
	}
	/*if(flagx==1 && flagy==1)
	{
		verificarposicion(x,y);
		//println("x=",x);
		//println("y=",y);
		flagx=0;
		flagy=0;
		x=0;
		y=0;
	}*/
	verificarposicion(x,y);
	//LPC_GPIO0->FIOSET=(1<<22);
}

void verificarposicion(int x,int y)
{

	if(x>3000)//x=1 derecha
	{
		LPC_GPIO0->FIOSET=(1<<6);//P0.6 derecha
		LPC_GPIO0->FIOCLR=(1<<7);//P0.7 izquierda
		LPC_GPIO0->FIOCLR=(1<<27);//P0.8 arriba
		LPC_GPIO0->FIOCLR=(1<<9);//P0.9 abajo
		LPC_GPIO0->FIOCLR=(1<<18);//P0.18 centro
		//delay(10);
	}else
	if(x<500)//x=0 izquierda
		{
		LPC_GPIO0->FIOCLR=(1<<6);//P0.6 derecha
		LPC_GPIO0->FIOSET=(1<<7);//P0.7 izquierda
		LPC_GPIO0->FIOCLR=(1<<27);//P0.8 arriba
		LPC_GPIO0->FIOCLR=(1<<9);//P0.9 abajo
		LPC_GPIO0->FIOCLR=(1<<18);//P0.18 centro
		//delay(10);
		}else
	if(y>2500 && x>2500 )//y=1 abajo
			{
		LPC_GPIO0->FIOCLR=(1<<6);//P0.6 derecha
		LPC_GPIO0->FIOCLR=(1<<7);//P0.7 izquierda
		LPC_GPIO0->FIOCLR=(1<<27);//P0.8 arriba
		LPC_GPIO0->FIOSET=(1<<9);//P0.9 abajo
		LPC_GPIO0->FIOCLR=(1<<18);//P0.18 centro
		//delay(10);

			}else
	if(y<1000 && x<2000)//y=0 arriba
			{
			LPC_GPIO0->FIOCLR=(1<<6);//P0.6 derecha
			LPC_GPIO0->FIOCLR=(1<<7);//P0.7 izquierda
			LPC_GPIO0->FIOSET=(1<<27);//P0.27 arriba
			LPC_GPIO0->FIOCLR=(1<<9);//P0.9 abajo
			LPC_GPIO0->FIOCLR=(1<<18);//P0.18 centro
			//delay(10);
			}
		//delay(100);
		LPC_GPIO0->FIOCLR=(1<<6);//P0.6 derecha
		LPC_GPIO0->FIOCLR=(1<<7);//P0.7 izquierda
		LPC_GPIO0->FIOSET=(1<<8);//P0.8 arriba
		LPC_GPIO0->FIOCLR=(1<<9);//P0.9 abajo
		LPC_GPIO0->FIOSET=(1<<18);//P0.18 centro*/
		//delay(10);
		//x=0;
		//y=0;
}
