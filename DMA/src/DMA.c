/*
===============================================================================
 Name        : DMA.c
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

// TODO: insert other include files here

// TODO: insert other definitions and declarations here
uint8_t data[200];
int main(void) {
	//Setup of the timer which generates the DMA request signal


	//Now lets configure the DMA
	//1. Power up the GPDMA (LPC17xx manual table 46, pg 64)
	LPC_SC->PCONP |= 1 << 29;
	/*There is no need to setup the clock for DMA as we were doing for other peripherals.
	 *
	 * The system clock will be directly given to the DMA.
	*/

	//2. Enable the GPDMA (LPC17xx manual table 557, pg 599)
	LPC_GPDMA->DMACConfig |= 1 << 0;

	//3. Set the Match Compare 0 (MAT1.0 signal) as the DMA request signal

    LPC_GPDMA->DMACSync &= ~(1 << 10); // use MAT1.0 for Sync (LPC17xx manual table 558, pg 599)

    // The above line is actually not needed as DMACSync register will be zero on reset.

    LPC_SC->DMAREQSEL |= 1 << 2; // Timer1 Match Compare 0 as DMA request (LPC17xx manual table 559, pg 600)

    //4. Clear the Interrupt Terminal Count Request and Interrupt Error Status register.
    //(Writing 1 to the clear registers will clear the entry in the main register.
    //i.e writing 0xff to DMACIntErrClr register will clear the DMACIntErrStat register)

    LPC_GPDMA->DMACIntErrClr |= 0xff; // (LPC17xx manual table 549, pg 596)

    LPC_GPDMA->DMACIntTCClear |= 0xff; // (LPC17xx manual table 547, pg 595)

    //5. Set the source and destination addresses (LPC17xx manual table 560 and 561, pg 601)

    LPC_GPDMACH0->DMACCDestAddr = (uint32_t) &(LPC_GPIO0->FIOPIN2); // LED is connected to P1.29

    LPC_GPDMACH0->DMACCSrcAddr = (uint32_t) &data[0]; // data[] is the array where I have stored alternating 1's and 0's

    //6. Clear the Linked List as we are not using it. (LPC17xx manual table 562, pg 602)

    LPC_GPDMACH0->DMACCLLI = 0;

    //7. Set the burst transfer size, source burst and destination burst size,
    //source and destination width, source increment, destination increment (LPC17xx manual table 563, pg 603)

    //In our case, let the transfer size be 200 bytes,
    //source burst and destination burst sizes are 1,
    //source and destination width are 8-bits
    //and we need to do source increment and there is no need for destination increment.

    LPC_GPDMACH0->DMACCControl = 10 | ( 1 << 26 );

    //8. Set the type of transfer as Memory to Peripheral and the destination request line
    //as MAT1.0 (LPC17xx manual table 564, pg 605)

    LPC_GPDMACH0->DMACCConfig = ( 10 << 6 ) | ( 1 << 11)| ( 1 << 15); // 10 corresponds to MAT1.0 and it is selected as the destination
    													  //request peripheral(LPC17xx manual table 543, pg 592)

    //9. Enable the channel (LPC17xx manual table 564, pg 605)



    //You can setup an interrupt for DMA transfer completion so that after the DMA
    //transfer is complete and interrupt request is generated and in the interrupt service routine
    //you need to disable the channel by writing

    //LPC_GPDMACH0->DMACCConfig = 0; // stop ch0 dma

    // TODO: insert code here
    //NVIC_EnableIRQ(TIMER1_IRQn);
    //NVIC_SetPriority(DMA_IRQn, 20);
    //NVIC_EnableIRQ(DMA_IRQn);
    LPC_GPDMACH0->DMACCConfig |= 1; //enable ch0
    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
    }
    return 0 ;
}


void TIMER1_IRQHandler(void)
{
	LPC_TIM1->IR=(0b111111);
}
