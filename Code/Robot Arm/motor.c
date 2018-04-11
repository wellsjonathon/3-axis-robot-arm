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

// GLOBAL VARIABLES
// ----------------
float MOTOR_B_STEPS_TAKEN;
float MOTOR_A1_STEPS_TAKEN;
float MOTOR_A2_STEPS_TAKEN;
int MOTOR_SPEED_CURRENT = 1;
// ----------------

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
	// Initialize motor timer
	timers_init();
	// Set steps taken to 0 initially
	MOTOR_B_STEPS_TAKEN = 0;
	MOTOR_A1_STEPS_TAKEN = 0;
	MOTOR_A2_STEPS_TAKEN = 0;
	
	motors_all_hold();
}

void timers_init(void) {
	// Ensure TIM2 clock is on
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	// SysClk = 24MHz
	// PSC = 2399, ARR = 100 ==> 100Hz or 100 steps/second
	// PSC = 2399, ARR = 50 ==> 200Hz or 200 steps/second
	TIM2->CR1 &= ~TIM_CR1_CEN;
	TIM2->EGR = TIM_EGR_UG;
	TIM2->PSC = 2399;
	TIM2->ARR = MOTOR_SPEED_1;
	TIM2->DIER = TIM_DIER_UIE;
	TIM2->CR1 = TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_SetPriority(TIM2_IRQn, 0);
}

void timers_enable(void) {
	TIM2->CR1 |= TIM_CR1_CEN;
}

void timers_disable(void) {
	TIM2->CR1 &= ~TIM_CR1_CEN;
}

uint8_t motors_change_speed(void) {
	MOTOR_SPEED_CURRENT++;
	if (MOTOR_SPEED_CURRENT > MOTOR_SPEED_LEVELS) {
		MOTOR_SPEED_CURRENT = 1;
	}
	switch (MOTOR_SPEED_CURRENT) {
		case 1:
			TIM2->ARR = MOTOR_SPEED_1;
			break;
		case 2:
			TIM2->ARR = MOTOR_SPEED_2;
			break;
		case 3:
			TIM2->ARR = MOTOR_SPEED_3;
			break;
		default:
			TIM2->ARR = MOTOR_SPEED_1;
			break;
	}
	TIM2->EGR |= TIM_EGR_UG;
	return MOTOR_SPEED_CURRENT;
}

uint8_t motors_get_speed(void) {
	return MOTOR_SPEED_CURRENT;
}

void motors_all_hold(void) {
	GPIOB->BSRR = B_STEP_OFF;
	GPIOB->BSRR = A1_STEP_OFF;
	GPIOB->BSRR = A2_STEP_OFF;
}

void motors_b_move(uint32_t dir, float steps) {
	// Set direction
	GPIOB->BSRR = dir;
	// Move
	for (int i = 0; i < steps; i++) {
		// Total delay: 60000 -> 100 steps/second
		GPIOB->BSRR = B_STEP_ON;
		delay(1000);// Slow it down to account for overcoming inertia of the arm
		GPIOB->BSRR = B_STEP_OFF;
		MOTOR_B_STEPS_TAKEN++;
		//delay(30000); 
	}
}

void motors_a1_move(uint32_t dir, float steps) {
	// Set direction
	GPIOB->BSRR = dir;
	// Move
	for (int i = 0; i < steps; i++) {
		// Total delay: 60000 -> 100 steps/second
		GPIOB->BSRR = A1_STEP_ON;
		delay(1000);// Slow it down to account for overcoming inertia of the arm
		GPIOB->BSRR = A1_STEP_OFF;
		MOTOR_A1_STEPS_TAKEN++;
		//delay(30000); 
	}
}

void motors_a2_move(uint32_t dir, float steps) {
	// Set direction
	GPIOB->BSRR = dir;
	// Move
	for (int i = 0; i < steps; i++) {
		// Total delay: 60000 -> 100 steps/second
		GPIOB->BSRR = A2_STEP_ON;
		delay(1000); 
		GPIOB->BSRR = A2_STEP_OFF;
		//delay(30000); 
	}
}
