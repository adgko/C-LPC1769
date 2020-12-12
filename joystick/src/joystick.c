/*
===============================================================================
 Name        : joystick.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif


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
//CONFIGURACION DEL TIMER:
unsigned int volatile *const T0PR = (unsigned int*)0x4000400C; //Preescaler register, cuando el preescaler counter alcanza el valor almacenado aca, se incrementa el TC (TimerCounter)
unsigned int volatile *const T0MCR = (unsigned int*)0x40004014;
unsigned int volatile *const T0CR  = (unsigned int*)0x40004004;
unsigned int volatile *const T0IR  = (unsigned int*)0x40004000;// bit de interrupcion (flag)
unsigned int volatile *const T0MR0 = (unsigned int*)0x40004018;
unsigned int volatile *const T0TCR = (unsigned int*)0x40004004;//registro del TCR, sirve para controlar las funciones del timer counter
// TODO: insert other include files here
#include <cr_section_macros.h>
// TODO: insert other definitions and declarations here
int x,y;
int canal;
int flagmux;
int promxi=0;
int promyi=0;
int promx[10];
int promy[10];
int flagx=0;
int flagy=0;
void confADC();
void confGPIO();
void confTMR0();
void verificarposicion(int x2,int y2);
int main(void) {

    // TODO: insert code here

    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    flagmux=0;
    x=0;
    y=0;
    // Enter an infinite loop, just incrementing a counter
    confGPIO();
    confADC();
    confTMR0();

    while(1) {
        i++ ;
    }
    return 0 ;
}
void TIMER0_IRQHandler(void)
{
	if(canal==0)
	{
		*AD0CR |= (1<<0);
		*AD0CR &=~ (1<<1);
		canal=1;

	}else
	{
		*AD0CR |= (1<<1);
		*AD0CR &=~ (1<<0);
		canal=0;
		flagmux=1;
	}//*/
		*AD0CR|=(1<<24);//empiezo adc
		*T0IR|=(1<<0);


}
void confTMR0()
{
	*PCONP|=(1<<1);//enciendo PCONP para timer0
	// Habilito el clock para el timer0
	*PCLKSEL0&=~(1<<2); //para timer 0 el bit 2 y 3 en 00
	*PCLKSEL0&=~(1<<3);
	//Configuracion del Timer0
	*T0PR = 0; //Al poner el Preescaler igual a 0, el timer counter se incrementara con cada pulso de clock
	*T0MR0 = 2500000;//10ms)
	//Configuracion del MCR
	*T0MCR|=(1<<0); // Se genera una interrupcion cuando el MR0 se matchea con el TC (timer counter)
	*T0MCR|=(1<<1);//RESETEA CUANDO MATCHEA
	// INICIO EL TIMER COUNTER DEL TIMER 0
	*T0TCR|=(1<<0);
	//habilito interrupcion por timer0
	*ISER0|=(1<<1);

}
void confADC()
{

	*PCONP |= (1<<12);//habilito periferico ADC
	*AD0CR |= (1<<0);
	canal=0;//me dice por q canal va a converir
	//*AD0CR &=~ (1<<1);
	*AD0CR |=(1<<21);//PDN

	*AD0CR |= (1<<0); //PARA HABILITAR EL CANAL 0
	*AD0CR |= (1<<1); //PARA HABILITAR EL CANAL 1
	*AD0CR &=~ (1<<16); //MODO BURST ACTIVADO
	*AD0CR &= ~(1UL << 24); // Start en 000 (para funcionamiento burst)
	*AD0CR &= ~(1UL << 25);
	*AD0CR &= ~(1UL << 26);
	//CONFIGURACION DEL CLOCK DEL ADC (por defecto 00 -> CCLK/4 --> CCLK/8 (11))
	*PCLKSEL0 |= (1<<25);
	*PCLKSEL0 |= (1<<24); 	//QUEDA UNA VEL DE 12.5K
	//TAMBIEN PODEMOS DIRECTAMENTE MODIFICAR EL CLK DEL AD0CR PERO ES MAS FACIL ESTE

	*PINSEL1 |= (1<<14)| (0<<15); 		//P0.23 COMO ADC
	*PINMODE1 |= (1<<15) | (0<<14); 	//DESACTIVO RESIST PULL-UP Y PULL-DOWN
    //canal 1
	*PINSEL1 |= (1<<16) | (0<<17); 		//P0.24 COMO ADC
	*PINMODE1 |= (1<<17) | (0<<16); 	//DESACTIVO RESIST PULL-UP Y PULL-DOWN
//pruebo con canal 3


	*ADINTEN0 |= (1<<0);
	*ADINTEN0 |= (1<<1);
	*ISER0 |= (1<<22); //HABILITO INTERRUPCIONES
	//LPC_ADC->ADCR|=(1<<24);//EMPIEZA A CONVERTIR

}

void ADC_IRQHandler(void){

	if(LPC_ADC->ADINTEN & (1<<0)){
		//Valor de canal 0
		//x=(int)((LPC_ADC->ADDR0>>4)&0xFFF); //Toma el valor del ADC
		//*AD0CR &=~ (1<<1);
		//*AD0CR |= (1<<0);

		promx[promxi]=(int)((LPC_ADC->ADDR0>>4)&0xFFF);
				x=x+promx[promxi];
				promxi++;

				if(promxi==10)
				{
					promxi=0;
					x=x/10;
					flagx=1;
				}//*/

	}
	if(LPC_ADC->ADINTEN &(1<<1))
	{
		//Valor de canal 1
		//y=(int)((LPC_ADC->ADDR1>>4)&0xFFF); //Toma el valor del ADC
		//*AD0CR &=~ (1<<0);
		//*AD0CR |= (1<<1);
		promy[promyi]=(int)((LPC_ADC->ADDR1>>4)&0xFFF);
				y=y+promy[promyi];
				promyi++;
				if(promyi==10)
				{
					y=y/10;
					promyi=0;
					flagy=1;
				}//*/
	}

	if(flagx==1 && flagy==1)
	{//flagmux==1 &&
		//flagmux=0;
		verificarposicion(x,y);
		flagx=0;
		flagy=0;

	}//*/
	/*if(flagx==1 && flagy==1)
		{
			//verificarposicion(x,y);
			//println("x=",x);
			//println("y=",y);
			flagx=0;
			flagy=0;
			x=0;
			y=0;
		}//*/
//
}

void verificarposicion(int x2,int y2)
{

	if(x2>=4090)//x=1 derecha
	{
		LPC_GPIO0->FIOSET=(1<<6);//P0.6 derecha
		LPC_GPIO0->FIOCLR=(1<<7);//P0.7 izquierda
		LPC_GPIO0->FIOCLR=(1<<27);//P0.27 arriba
		LPC_GPIO0->FIOCLR=(1<<9);//P0.9 abajo
		LPC_GPIO0->FIOCLR=(1<<18);//P0.18 centro

		//delay(5);
	}else
	if(x2<100)//x=0 izquierda
		{
		LPC_GPIO0->FIOSET=(1<<7);//P0.7 izquierda
		LPC_GPIO0->FIOCLR=(1<<6);//P0.6 derecha
		LPC_GPIO0->FIOCLR=(1<<27);//P0.8 arriba
		LPC_GPIO0->FIOCLR=(1<<9);//P0.9 abajo
		LPC_GPIO0->FIOCLR=(1<<18);//P0.18 centro

		//delay(100);
		}else
	if(y2>=4094)//y=1 abajo
			{
		LPC_GPIO0->FIOSET=(1<<9);//P0.9 abajo
		LPC_GPIO0->FIOCLR=(1<<6);//P0.6 derecha
		LPC_GPIO0->FIOCLR=(1<<7);//P0.7 izquierda
		LPC_GPIO0->FIOCLR=(1<<27);//P0.8 arriba
		LPC_GPIO0->FIOCLR=(1<<18);//P0.18 centro

		//delay(100);

			}else
	if(y2<100)//y=0 arriba
			{
			LPC_GPIO0->FIOSET=(1<<27);//P0.27 arriba
			LPC_GPIO0->FIOCLR=(1<<6);//P0.6 derecha
			LPC_GPIO0->FIOCLR=(1<<7);//P0.7 izquierda
			LPC_GPIO0->FIOCLR=(1<<9);//P0.9 abajo
			LPC_GPIO0->FIOCLR=(1<<18);//P0.18 centro

			//delay(5);
			}else
			{
		//delay(100);
		LPC_GPIO0->FIOCLR=(1<<6);//P0.6 derecha
		LPC_GPIO0->FIOCLR=(1<<7);//P0.7 izquierda
		LPC_GPIO0->FIOCLR=(1<<27);//P0.27 arriba
		LPC_GPIO0->FIOCLR=(1<<9);//P0.9 abajo
			}
		//LPC_GPIO0->FIOSET=(1<<18);//P0.18 centro*/
		//delay(10);
		x=0;
		y=0;
}
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
void delay (unsigned int value)
{
	unsigned int i,j;
	for(i=0; i<value; i++)
	{
   	for (j=0; j<3000; j++);
	}
}
