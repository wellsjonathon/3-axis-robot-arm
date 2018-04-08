/*
 *	Author: Jonathon Wells
 *	Description: Library to initialize interrupts
 *
 *	Class: ENEL387
 *	Date: Feb. 19, 2018
 *
 */
 
#include "interrupts.h"
#include "stm32f10x.h"

void systick_init(void) {
	SysTick->CTRL = 0x00; // Turn SysTick off
	SysTick->VAL = 0x00; // Reset Value counter
	SysTick->LOAD = 0x7A1200; // Initialize with time between interrupts (24MHz)
	                         // 16E3600 = 24,000,000
	SysTick->CTRL = 0x07; // Re-enable SysTick and allow interrupt generation
}

void extio_init(void) {
	// Enable AFIO clock if it hasn't been already
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	// Select Port A as EXTI 0 source
	AFIO->EXTICR[0] &= AFIO_EXTICR1_EXTI0;
	// Unmask PA0 as interrupt source
	EXTI->IMR |= EXTI_IMR_MR0;
	// Select falling edge as trigger
	EXTI->FTSR |= EXTI_FTSR_TR0;
	// Unmask EXTI 0 as interrupt source in NVIC
	NVIC->ISER[0] |= NVIC_ISER_SETENA_6;
}
