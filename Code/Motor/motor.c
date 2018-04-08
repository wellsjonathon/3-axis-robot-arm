/*
 *	Author: Jonathon Wells
 *	Description: Library to initialize and control motors
 *
 *	Class: ENEL387
 *	Date: April 2018
 *
 */
 
#include "stm32f10x.h"
#include "m3_lib.h"
#include "motor.h"

void motors_init(void) {
  // Ensure Port B clock is on
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  // Initialize PB10-15 push-pull outputs
	GPIOB->CRH |= GPIO_CRH_MODE10 | GPIO_CRH_MODE11 
							| GPIO_CRH_MODE12 | GPIO_CRH_MODE13 
							| GPIO_CRH_MODE14 | GPIO_CRH_MODE15;
	GPIOB->CRH &= ~GPIO_CRH_CNF10 & ~GPIO_CRH_CNF11
							& ~GPIO_CRH_CNF12 & ~GPIO_CRH_CNF13
							&~GPIO_CRH_CNF14 & ~GPIO_CRH_CNF15;
}

void motors_a1_move(uint32_t dir, uint16_t steps) {
	// Set direction
	GPIOB->BSRR = dir;
	// Move
	for (int i = 0; i < steps; i++) {
		// Total delay: 60000 -> 100 steps/second
		GPIOB->BSRR = A1_STEP_ON;
		delay(30000);// Slow it down to account for overcoming inertia of the arm
		GPIOB->BSRR = A1_STEP_OFF;
		delay(30000); 
	}
}

void motors_a2_move(uint32_t dir, uint16_t steps) {
	// Set direction
	GPIOB->BSRR = dir;
	// Move
	for (int i = 0; i < steps; i++) {
		// Total delay: 60000 -> 100 steps/second
		GPIOB->BSRR = A2_STEP_ON;
		delay(30000); 
		GPIOB->BSRR = A2_STEP_OFF;
		delay(30000); 
	}
}
