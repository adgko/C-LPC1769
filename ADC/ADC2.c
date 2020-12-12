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

typedef int boolean;
#define true 1
#define false 0

//funciones
void setVelocidad(int);
void PLL0_Feed();
void configTMR0();
void configTMR1();
void configNVIC();
void configEINT();
void configPort2();
void escribirDisplay(int);
void refrescoDisplay();
void configGPIOINT();
void configADC();
void configDAC();

//variables
int timerInt = 0;
int displayNum = 0;
int flagAntiRebote = 0;
int contAntiRebote = 0;
int displayValues[2] = {0,0};
int displayTable[10] = {63,6,91,79,102,109,125,7,127,111};
int DAC_Value = 0;
int ADC_Values[5] = {0,0,0,0,0};
int ADC_Values_Pos = 0;

int main(void) {
	LPC_GPIO0->FIODIR |= (1 << 22); // port0pin22 en modo salida
	LPC_GPIO0->FIODIR |= (1 << 4); // port0pin4 en modo salida

	configDAC();
	configTMR0();
	configTMR1();
	configEINT();
	configPort2();
	configGPIOINT();
	configADC();
	configNVIC();

	for (int i = 0; i < 10; i++) {
		escribirDisplay(displayTable[i]);
	}

	while (1) {
	}
	return 0;
}

void configDAC() {
	LPC_PINCON->PINSEL1 &= ~(1UL << 20); // pin 0.26 - funcion '10' = AOUT
	LPC_PINCON->PINSEL1 |= (1UL << 21);
	LPC_PINCON->PINMODE1 &= ~(1UL << 20); // pin 0.26 en pullup (?)
	LPC_PINCON->PINMODE1 &= ~(1UL << 21);
	LPC_SC->PCLKSEL0 &= ~(1UL << 22);
	LPC_SC->PCLKSEL0 &= ~(1UL << 23); // PCLK en 100MHz/4 = 25MHz
}

void configADC() {
	// configuracion basica
	LPC_SC->PCONP |= (1UL << 12); // encender el modulo ADC (power y clock)
	LPC_ADC->ADCR = 0;
	LPC_ADC->ADCR |= (1UL << 21); // encender el modulo ADC
	LPC_ADC->ADCR &= ~(1UL << 24); // Start en 000 (para funcionamiento burst)
	LPC_ADC->ADCR &= ~(1UL << 25);
	LPC_ADC->ADCR &= ~(1UL << 26);
	LPC_SC->PCLKSEL0 |= (1UL << 24);
	LPC_SC->PCLKSEL0 |= (1UL << 25); // PCLK en 100MHz/8 = 12.5MHz
	LPC_PINCON->PINSEL1 |= (1UL << 14);
	LPC_PINCON->PINSEL1 &= ~(1UL << 15); // pin0.23 en modo AD0.0
	LPC_PINCON->PINMODE1 &= ~(1UL << 14);
	LPC_PINCON->PINMODE1 &= ~(1UL << 15); // pin0.23 con pull-up (?)
	//
	LPC_ADC->ADINTEN |= (1UL << 8);
	LPC_ADC->ADINTEN &= ~(1UL << 0);
	LPC_ADC->ADCR |= (1UL << 0); // seleccion de canal 0 para realizar la converison
	LPC_ADC->ADCR &= ~(1UL << 16); // modo burst desact

}

void ADC_IRQHandler(void) {
	// para desactivar la bandera DONE hay que leer el ADDR
	int ADC_Valor = (LPC_ADC->ADGDR & 0xFFFF) >> 4;
	ADC_Values[ADC_Values_Pos] = ADC_Valor;
	ADC_Values_Pos++;
	if (ADC_Values_Pos == 5) {
		ADC_Values_Pos = 0;
	}
	if (ADC_Valor >= 2048) {
		LPC_GPIO0->FIOCLR = (1 << 4);
	} else {
		LPC_GPIO0->FIOSET = (1 << 4);
	}
}

void escribirDisplay(int valor) {
	LPC_GPIO2->FIOMASK = ~0x7F;
	LPC_GPIO2->FIOPIN = valor;
	LPC_GPIO2->FIOMASK = 0;
}

void refrescoDisplay() {
	if (LPC_GPIO2->FIOPIN & (1 << 7)) {
		LPC_GPIO2->FIOCLR = (1 << 7);
		LPC_GPIO2->FIOSET = (1 << 8);
	} else {
		LPC_GPIO2->FIOCLR = (1 << 8);
		LPC_GPIO2->FIOSET = (1 << 7);
	}
}

void configPort2() {
	for (int i = 0; i < 9; i++) { // 7 segmentos + 2 de seleccion de display
		LPC_GPIO2->FIODIR |= (1UL << i);
	}
	LPC_GPIO2->FIOCLR |= (1UL << 7);
	LPC_GPIO2->FIOSET |= (1UL << 8);

	for (int i = 0; i < 7; i++) {
		LPC_GPIO2->FIOSET |= (1UL << i);
	}

	for (int i = 0; i < 7; i++) {
		LPC_GPIO2->FIOCLR |= (1UL << i);
	}
}

void configTMR1() {
	LPC_TIM1->CTCR = 0; // TMR0 en modo timer y no contador
	LPC_TIM1->MR0 = 25000; // Match register 0 (realiza accion en este valor)
	LPC_TIM1->MCR |= (1UL << 0); // Interrupt on MR0
	LPC_TIM1->MCR |= (1UL << 1); // NO Reset on MR0
	//INTERRUPCION EN NVIC
	LPC_TIM1->TCR |= (1UL << 0); // Counter Enable
	LPC_TIM1->TCR |= (1UL << 1); // Counter reset
	LPC_TIM1->TCR &= ~(1UL << 1); // Counter reset

	LPC_TIM1->IR &= (1UL << 0); // bajamos la bandera de interrupcion
}

void configTMR0() {
	LPC_TIM0->CTCR = 0; // TMR0 en modo timer y no contador
	LPC_TIM0->MR0 = 250000; // Match register 0 (realiza accion en este valor)
	LPC_TIM0->MR1 = 500000; // Match register 1
	LPC_TIM0->PR = 0; // Prescaler, cuenta hasta esto para incrementar TMR0 contador
	LPC_TIM0->MCR |= (1UL << 0); // Interrupt on MR0
	LPC_TIM0->MCR &= ~(1UL << 1); // NO Reset on MR0
	LPC_TIM0->MCR |= (1UL << 3); // Interrupt on MR1
	LPC_TIM0->MCR |= (1UL << 4); // Reset on MR1
	//INTERRUPCION EN NVIC
	LPC_TIM0->TCR |= (1UL << 0); // Counter Enable
	LPC_TIM0->TCR |= (1UL << 1); // Counter reset
	LPC_TIM0->TCR &= ~(1UL << 1); // Counter reset
//	*T0TC = 0;
	LPC_TIM0->IR &= (1UL << 0); // bajamos la bandera de interrupcion
	LPC_TIM0->IR &= (1UL << 1); // bajamos la bandera de interrupcion
}

void configEINT() {
	LPC_PINCON->PINSEL4 |= (1UL << 20);
	LPC_PINCON->PINSEL4 &= ~(1UL << 21);
	LPC_PINCON->PINMODE4 &= ~(1UL << 20);
	LPC_PINCON->PINMODE4 &= ~(1UL << 21);
	LPC_SC->EXTMODE |= (1UL << 0);
	LPC_SC->EXTPOLAR &= ~(1UL << 0);
	LPC_SC->EXTINT &= ~(1UL << 0);
}

void configNVIC() {
	NVIC_EnableIRQ(1); // TMR0
	NVIC_EnableIRQ(2); // TMR1
	NVIC_EnableIRQ(18); // EINT0
	NVIC_EnableIRQ(21); // INT por EINT3 (compartido con GPIOINT)
	NVIC_EnableIRQ(22); // habilitar interrupcion por ADC
}

void configGPIOINT() {
	LPC_GPIO2->FIODIR &= ~(1UL << 11); // pin 2.11 entrada
	LPC_PINCON->PINSEL4 &= ~(1UL << 22); // GPIO
	LPC_PINCON->PINSEL4 &= ~(1UL << 23);
	LPC_PINCON->PINMODE4 &= ~(1UL << 22); // pull up
	LPC_PINCON->PINMODE4 &= ~(1UL << 23);
	LPC_GPIOINT->IO2IntEnF |= (1UL << 11); // INT en pin 2.11 falling
	LPC_GPIOINT->IO2IntEnR |= (1UL << 11); // INT en pin 2.11 rising
}

void EINT0_IRQHandler(void) {
	LPC_SC->EXTINT = (1UL << 0); // Se setea en 0 seteando un 1 - Se baja la bandera

	displayValues[0]++;
	if (displayValues[0] == 10) {
		displayValues[0] = 0;
		displayValues[1]++;
		if (displayValues[1] == 10) {
			displayValues[1] = 0;
		}
	}

	flagAntiRebote = 1;

	if (LPC_GPIO0->FIOPIN & (1 << 22)) {
		LPC_GPIO0->FIOCLR = (1 << 22);
	} else {
		LPC_GPIO0->FIOSET = (1 << 22);
	}
}

void EINT3_IRQHandler(void) {
	if (LPC_GPIOINT->IO2IntStatF & (1 << 11)) { // INT en pin 2.11?
		LPC_GPIOINT->IO2IntClr |= (1UL << 11); // limpiar bandera

		flagAntiRebote = 1;

		displayValues[0]--;
		if (displayValues[0] == -1) {
			displayValues[0] = 9;
			displayValues[1]--;
			if (displayValues[1] == -1) {
				displayValues[1] = 9;
			}
		}
	} else if (LPC_GPIOINT->IO2IntStatR & (1 << 11)) {
		LPC_GPIOINT->IO2IntClr |= (1UL << 11);
		flagAntiRebote = 1;
	}
}

void TIMER0_IRQHandler(void) {
	if (LPC_TIM0->IR & (1 << 0)) {
		LPC_TIM0->IR &= (1UL << 0); // bajamos la bandera de interrupcion
		LPC_GPIO2->FIOCLR = (1 << 7);
		LPC_GPIO2->FIOSET = (1 << 8);
		escribirDisplay(displayTable[displayValues[1]]);
	} else if (LPC_TIM0->IR & (1 << 1)) {
		LPC_TIM0->IR &= (1UL << 1); // bajamos la bandera de interrupcion
		LPC_GPIO2->FIOCLR = (1 << 8);
		LPC_GPIO2->FIOSET = (1 << 7);
		escribirDisplay(displayTable[displayValues[0]]);
	}
	if (flagAntiRebote == 1) {
		NVIC_DisableIRQ(18); // EINT0
		NVIC_DisableIRQ(21); // INT por EINT3 (compartido con GPIOINT)

		LPC_SC->EXTINT = (1UL << 0); // Se setea en 0 seteando un 1 - Se baja la bandera
		LPC_GPIOINT->IO2IntClr = (1UL << 11); // limpiar bandera

		contAntiRebote++;
		if (contAntiRebote == 30) {
			contAntiRebote = 0;
			flagAntiRebote = 0;
			NVIC_EnableIRQ(18); // EINT0
			NVIC_EnableIRQ(21); // INT por EINT3 (compartido con GPIOINT)
			NVIC_ClearPendingIRQ(18); // limpio pending EINT0
			NVIC_ClearPendingIRQ(21); // limpio pending INT por EINT3 (compartido con GPIOINT)
		}
	}
	refrescoDisplay();
}

void TIMER1_IRQHandler(void) {
	LPC_TIM1->IR &= (1UL << 0); // bajamos la bandera de interrupcion
	LPC_ADC->ADCR |= (1UL << 24); //start ADC

	DAC_Value = 0;
	for (int i = 0; i < 5; i++) {
		DAC_Value += ADC_Values[i];
	}
	DAC_Value /= 5*4;

	LPC_DAC->DACR = (DAC_Value & 0x3FF/*and 10 bits*/) << 6; // escribir el valor al DAC - lo convierte y saca por salida solo
}
