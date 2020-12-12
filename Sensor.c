/*
===============================================================================
 Name        : senso_de_color.c
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
#include <stdio.h>
#include <stdlib.h>

int rojo=0;
int verde=0;
int azul=0;
int clear=0;

int estado1=0;
int estado2=0;
int cont=0;

char cadena[20]={'Hola'};
char espacio[1]={'\n'};
char colorR[20]={'Rojo'};

void color(void);
int pulso(void);
void uartConfig(void);
void enviar(char c[]);
void delay(int x);

int main(void) {

	//Configuracion de pines
	LPC_GPIO0->FIODIR |= (1<<6)|(1<<7)|(1<<8)|(1<<9);				//p0[9:6] como salida
	LPC_GPIO0->FIODIR &= ~(1<<0);									//p0[0] como entrada

	LPC_GPIO0->FIOSET = (1<<7);				    //s1=p0[7]=1 frecuencia al 2%

	uartConfig();


    while(1) {

    	color();

    	if((rojo>=72) && (rojo<=86)){
    		if((verde>=240) && (verde<=290)){
    			if((azul>=166) && (azul<=205)){
    				enviar('R'); enviar('O'); enviar('J'); enviar('O'); enviar(' '); }}}

    	else if(rojo>=50 && rojo<=70){
    		if(verde>=60 && verde<=180){
    			if(azul>=150 && azul<=188){
    				enviar('N'); enviar('A'); enviar('R'); enviar('A'); enviar('N'); enviar('J'); enviar('A'); enviar(' '); }}}

    	else if(rojo>=225 && rojo<=240){
    		if(verde>=140 && verde<=158){
    			if(azul>=170 && azul<=188){
    				enviar('V'); enviar('E'); enviar('R'); enviar('D'); enviar('E'); enviar(' ');}}}

    	else if(rojo>=160 && rojo<=180){
    		if(verde>=75 && verde<=100){
    			if(azul>=43 && azul<=65){
    				enviar('C'); enviar('I'); enviar('A'); enviar('N'); enviar(' ');}}}

    	else if(rojo>=252 && rojo<=274){
    		if(verde>=215 && verde<=240){
    			if(azul>=80 && azul<=105){
    				enviar('A'); enviar('Z'); enviar('U'); enviar('L'); enviar(' '); }}}

    	else if(rojo>=75 && rojo<=120){
    		if(verde>=240 && verde<=325){
    			if(azul>=100 && azul<=200){
    				enviar('R'); enviar('O'); enviar('S'); enviar('A'); enviar(' '); }}}

    	else enviar('-'); enviar(' ');

    	delay(100000);
    	delay(100000);
    	delay(100000);

/*    	enviar(' ');
    	enviar('R');
    	for(int i=0;i<20;i++)
        	enviar(cadena[i]);
    	itoa(rojo, cadena, 10);
	for(int i=0;i<20;i++)
    		enviar(cadena[i]);

		enviar(' ');
		enviar('V');
    	itoa(verde, cadena, 10);
	for(int i=0;i<20;i++)
    		enviar(cadena[i]);

		enviar(' ');
		enviar('A');
    	itoa(azul, cadena, 10);
	for(int i=0;i<20;i++)
    		enviar(cadena[i]);

		enviar(' ');
		enviar('C');
    	itoa(clear, cadena, 10);
	for(int i=0;i<20;i++)
    		enviar(cadena[i]);

		enviar('|');
		enviar('|');
		enviar('|');

	color();

	*/


    }
    return 0 ;
}

void color(void){

	LPC_GPIO0->FIOCLR = (1<<8);			//S2=0
	LPC_GPIO0->FIOCLR = (1<<9);			//S3=0 - Rojo
	rojo = pulso();

	LPC_GPIO0->FIOSET = (1<<8);			//S2=1, S3=0 - Clear
	clear = pulso();

	LPC_GPIO0->FIOCLR = (1<<8);			//S2=0
	LPC_GPIO0->FIOSET = (1<<9);			//S3=1 - Azul
	azul = pulso();

	LPC_GPIO0->FIOSET = (1<<8);			//S2=1, S3=1 - Verde
	verde = pulso();

	delay(10000);

}

int pulso(void){

	int fin=0;
	cont=0;

	estado1 = (LPC_GPIO0->FIOPIN) & (1<<0);			//toma el estado actual del pulso ( out=p0[0] )

	while(fin==0){
		estado2 = (LPC_GPIO0->FIOPIN) & (0x01);			//toma el estado actual del pulso ( out=p0[0] )

		while(!estado1 == estado2){								    //cuando cambie de estado comienzo a contar
			cont++;
			estado2 = (LPC_GPIO0->FIOPIN) & (0x01);			//toma el estado actual del pulso ( out=p0[0] )
			fin=1;												//cuando vuelva a cambiar se detiene
		}

}

	return cont;
}


void enviar(char c[]){

	//espero a que THR este vacio (mientras se este transmitiendo me quedo en el bucle)
	while ( ((LPC_UART0->LSR) & (1<<5)) == 0 ){	}			//LSR - THREmpty=1 cuando THR este vacio y THREmpty=0 cuando THR contiene datos

	LPC_UART0->THR = c; 									//envio dato

}


void uartConfig(void){

	//configuro funcion de pines
	LPC_PINCON->PINSEL0 |= (1<<4)|(1<<6);				//TX0-p0[2] y RX0-p0[3]

	//Habilito periferico UART0
	LPC_SC->PCONP |= (1<<3);

	// PCLKSEL0 = CLCK/4  por default 25MHz

	//Config BaudeRate.
	LPC_UART0->LCR |= (1<<7);					//DLAB=1 para acceder a DLL y DLM
	LPC_UART0->DLL = 163;						//DLL=163
	LPC_UART0->DLM = 0;						    //DLM=0
	LPC_UART0->LCR &=~ (1<<7);					//DLAB=0

	//Config LCR (control de trama)
	LPC_UART0->LCR |= (1<<0)|(1<<1);			//tamaño palabra: 8 bits de transmision
												//1 bit de stop, sin bit de paridad

}


void delay(int x){

	for(int i=0;i<x;i++){};
}
