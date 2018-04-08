/*
 *	Author: Jonathon Wells
 *	Description: Library to initialize timing mechanisms
 *							 Master clock, Port clock
 *
 *	Class: ENEL387
 *	Date: Jan. 24, 2018
 *
 */

#include "stm32f10x.h"
#include "m3_lib.h"


/*
 *	Name: void clockInit(void)
 *	Purpose: Set System Clock to 24MHz
 *	Mostly taken from lab1_lib.c -> clockInit(void)
 */
void initClocks(void) {
	
	uint32_t temp = 0x00;
	
	// Set output MCO as PLL/2
	// Set PLL input as PLLMUL X3, PREDIV1 as PLL input
	RCC->CFGR = 0x0705002;
	
	// Turn on PLL, HSE, HSI
	RCC->CR = 0x01010081;
	
	// Wait for PLL to stabilize by checking if PLLRDY is set
	while (temp != 0x02000000) {
		// Check to see if the PLL lock bit (PLLRDY) is set
		temp = RCC->CR & 0x02000000;
	}
	
	// Enable peripheral clocks, read in APB2ENR and then OR with bits that you want set
	// This makes sure you don't wipe out what was already set
	// ENEL387 development board uses Ports A, B, C
	// Pertinent options:
	// 		Port A: RCC_APB2ENR_IOPAEN
	// 		Port B: RCC_APB2ENR_IOPBEN
	// 		Port C: RCC_APB2ENR_IOPCEN
	// 		Port D: RCC_APB2ENR_IOPDEN
	// 		Port E: RCC_APB2ENR_IOPEEN
	// 		Port F: RCC_APB2ENR_IOPFEN
	// 		Port G: RCC_APB2ENR_IOPGEN
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;
}

void initLEDs(void) {
	// Enable periheral clocks if they aren't already
	// Development board LEDs, left to right, Port A9, A10, A11, A12
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	
	// Set config and mode bits for Port A9-A12 as push-pull outputs
	GPIOA->CRH |= GPIO_CRH_MODE12 | GPIO_CRH_MODE11 | GPIO_CRH_MODE10 | GPIO_CRH_MODE9;
	GPIOA->CRH &= ~GPIO_CRH_CNF12 & ~GPIO_CRH_CNF11 & ~GPIO_CRH_CNF10 & ~GPIO_CRH_CNF9;
	
	// Initialize onboard LEDs
	GPIOC->CRH |= GPIO_CRH_MODE9 | GPIO_CRH_MODE8 ;
	GPIOC->CRH &= ~GPIO_CRH_CNF9 & ~GPIO_CRH_CNF8 ;
}

/*
 *	Name: void delay(uint32_t count)
 *	Parameters: 32 bit delay value, (a value of 6000
 *							gives approximately 1ms delay)
 *	Purpose: Wait a set amount of time
 */
void delay(uint32_t count) {
	int i = 0;
	for (i = 0; i < count; ++i) {}
}

void initSwitches(void) {
	// Enable periheral clocks if they aren't already
	// Development board switches:
	//		SW1.1: Port A6
	//		SW1.2: Port A7
	//		SW1.3: Port C10
	//		SW1.4: Port C11
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;
}

uint8_t readSwitches(void) {
	// Development board dip switches:
	//		SW1.1: Port A6
	//		SW1.2: Port A7
	//		SW1.3: Port C10
	//		SW1.4: Port C11
	// Return:
	// 		Dip switches 4->1 [3:0] (0x00004321)
	
	uint16_t temp = 0x00;
	
	// Read in dip switches
	temp = ((GPIOC->IDR & GPIO_IDR_IDR11) >> 8) | 	// SW1.4
					((GPIOC->IDR & GPIO_IDR_IDR10) >> 8) | 	// SW1.3
					((GPIOA->IDR & GPIO_IDR_IDR7) >> 6) | 	// SW1.2
					(GPIOA->IDR & GPIO_IDR_IDR6) >> 6;			// SW1.1
	
	return (uint8_t)temp;
}

uint8_t readButtons(void) {
	// Development board push buttons:
	//		PB2: Port B8
	//		PB3: Port B9
	//		PB4: Port C12
	//		PB5: Port A5
	// Return:
	// 		Push buttons 5->2 [3:0]
	
	uint8_t temp = 0x00;
	
	// Read in push buttons
	temp |= (GPIOA->IDR & GPIO_IDR_IDR5) << 1; 	// PB5
	temp |= (GPIOC->IDR & GPIO_IDR_IDR12) << 1; // PB4
	temp |= (GPIOB->IDR & GPIO_IDR_IDR9) << 1; 	// PB3
	temp |= (GPIOB->IDR & GPIO_IDR_IDR8); 			// PB2
	
	return temp;
}

void testLEDs(void) {
	// Turn on all LEDs
	if ((GPIOB->IDR & GPIO_IDR_IDR8) == 0) {
	//if ((GPIOA->IDR & GPIO_IDR_IDR0) == 0) {
		GPIOA->BSRR = LED1_ON | LED2_OFF | LED3_ON | LED4_OFF;
	}
	else {
		GPIOA->BSRR = LED1_OFF | LED2_ON | LED3_OFF | LED4_ON;
	}
	//GPIOA->BSRR = LED1_ON | LED2_ON | LED3_ON | LED4_ON;
}
