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
void clocks_init(void) {
	
	uint32_t temp = 0x00;
	
	// Set output MCO as PLL/2
	// Set PLL input as PLLMUL X3, PREDIV1 as PLL input
	RCC->CFGR = 0x07050002;
	
	// Turn on PLL, HSE, HSI
	RCC->CR = 0x01010081;
	
	// Wait for PLL to stabilize by checking if PLLRDY is set
	while (temp != 0x02000000) {
		// Check to see if the PLL lock bit (PLLRDY) is set
		temp = RCC->CR & 0x02000000;
	}
	
	// Enable peripheral clocks
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN 
								| RCC_APB2ENR_IOPBEN 
								| RCC_APB2ENR_IOPCEN 
								| RCC_APB2ENR_IOPDEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN 
								| RCC_APB1ENR_TIM3EN
								| RCC_APB1ENR_TIM4EN;
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
}

void led_init(void) {
	// Enable periheral clocks if they aren't already
	// Development board LEDs, left to right, Port A9, A10, A11, A12
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	
	// Set config and mode bits for Port A9-A12 as push-pull outputs
	GPIOA->CRH |= GPIO_CRH_MODE12 | GPIO_CRH_MODE11 | GPIO_CRH_MODE10 | GPIO_CRH_MODE9;
	GPIOA->CRH &= ~GPIO_CRH_CNF12 & ~GPIO_CRH_CNF11 & ~GPIO_CRH_CNF10 & ~GPIO_CRH_CNF9;
	
	// Initialize onboard LEDs
	GPIOC->CRH |= GPIO_CRH_MODE9 | GPIO_CRH_MODE8;
	GPIOC->CRH &= ~GPIO_CRH_CNF9 & ~GPIO_CRH_CNF8;
}

void input_init(void) {
	// Enable peripheral clocks if they aren't already
	// SW1.X = PA6/7, PC10/11		MODE: 00 CNF: 01
	// SWX = PA5, PB8/9, PC12		MODE: 00 CNF: 01
	// LS = PB6/7, PC13 (?)			MODE: 00 CNF: 10
	// JOY_PB = PD2							MODE: 00 CNF: 10
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN 
								| RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPDEN;
	
	// Set mode bits
	GPIOA->CRL &= ~GPIO_CRL_MODE5 & ~GPIO_CRL_MODE6 
							& ~GPIO_CRL_MODE7;
	GPIOB->CRL &= ~GPIO_CRL_MODE6 & ~GPIO_CRL_MODE7;
	GPIOB->CRH &= ~GPIO_CRH_MODE8 & ~GPIO_CRH_MODE9;
	GPIOC->CRH &= ~GPIO_CRH_MODE10 & ~GPIO_CRH_MODE11
							& ~GPIO_CRH_MODE12 & ~GPIO_CRH_MODE13;
	GPIOD->CRL &= ~GPIO_CRL_MODE2;
	
	// Set config bits
	GPIOA->CRL |= GPIO_CRL_CNF5_0 | GPIO_CRL_CNF6_0 
							| GPIO_CRL_CNF7_0;
	GPIOA->CRL &= ~GPIO_CRL_CNF5_1 & ~GPIO_CRL_CNF6_1
							& ~GPIO_CRL_CNF7_1;
	GPIOB->CRL |= GPIO_CRL_CNF6_1 | GPIO_CRL_CNF7_1;
	GPIOB->CRH |= GPIO_CRH_CNF8_0 | GPIO_CRH_CNF9_0;
	GPIOB->CRL &= ~GPIO_CRL_CNF6_0 & ~GPIO_CRL_CNF7_0;
	GPIOB->CRH &= ~GPIO_CRH_CNF8_1 & ~GPIO_CRH_CNF9_1;
	GPIOC->CRH |= GPIO_CRH_CNF10_0 | GPIO_CRH_CNF11_0
							| GPIO_CRH_CNF12_0 | GPIO_CRH_CNF13_1;
	GPIOC->CRH &= ~GPIO_CRH_CNF10_1 & ~GPIO_CRH_CNF11_1
							& ~GPIO_CRH_CNF12_1 & ~GPIO_CRH_CNF13_0;
	GPIOD->CRL |= GPIO_CRL_CNF2_1;
	GPIOD->CRL &= ~GPIO_CRL_CNF2_0;
	GPIOD->ODR &= ~GPIO_ODR_ODR2;
	
	GPIOA->BSRR |= LED1_OFF | LED2_OFF | LED3_OFF | LED4_OFF;
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

void led_startup(void) {
	// Startup light sequence
	// Total time: 7350000 = 1.225 seconds
	GPIOA->BSRR |= LED1_ON;
	delay(450000);
	GPIOA->BSRR |= LED2_ON;
	delay(450000);
	GPIOA->BSRR |= LED3_ON | LED1_OFF;
	delay(450000);
	GPIOA->BSRR |= LED4_ON | LED2_OFF;
	delay(450000);
	GPIOA->BSRR |= LED3_OFF;
	delay(750000); // Pause on LED4
	GPIOA->BSRR |= LED3_ON;
	delay(450000);
	GPIOA->BSRR |= LED4_OFF | LED2_ON;
	delay(450000);
	GPIOA->BSRR |= LED3_OFF | LED1_ON;
	delay(450000);
	GPIOA->BSRR |= LED2_OFF;
	delay(750000); // Pause on LED1
	GPIOA->BSRR |= LED1_OFF;
	delay(450000);
	GPIOA->BSRR |= LED1_ON | LED2_ON | LED3_ON | LED4_ON;
	delay(750000);
	GPIOA->BSRR |= LED1_OFF | LED2_OFF | LED3_OFF | LED4_OFF;
	delay(750000);
	GPIOA->BSRR |= LED1_ON | LED2_ON | LED3_ON | LED4_ON;
	delay(750000);
	GPIOA->BSRR |= LED1_OFF | LED2_OFF | LED3_OFF | LED4_OFF;
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

uint8_t switches_read(void) {
	// Development board dip switches:
	//		SW1.1: Port A6
	//		SW1.2: Port A7
	//		SW1.3: Port C10
	//		SW1.4: Port C11
	// Return:
	// 		Dip switches 4->1 [3:0] (0x00004321)
	
	uint16_t temp = 0x0000;
	
	// Read in dip switches
	temp = (((GPIOC->IDR & GPIO_IDR_IDR11) >> 8) | 	// SW1.4
					((GPIOC->IDR & GPIO_IDR_IDR10) >> 8) | 	// SW1.3
					((GPIOA->IDR & GPIO_IDR_IDR7) >> 6)	 | 	// SW1.2
					((GPIOA->IDR & GPIO_IDR_IDR6) >> 6));		// SW1.1
	
	return temp & 0x0F;
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