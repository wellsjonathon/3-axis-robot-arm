//
//	Author: Jonathon Wells
//	Date: Jan 29, 2018
//

#include "m3_lib.h"
#include "interrupts.h"
#include "stm32f10x.h"
#include "motor.h"

/*
 * Description: Helper function used to print readSwitches() result to the LCD
 *
 */

int main(void) {
	
	int i = 0;
	
	initClocks();
	initLEDs();
	motors_init();
	
	while(1) {
		
		motors_a1_move(A1_EXTEND, 100);
		delay(12000000);
		motors_a1_move(A1_RETRACT, 100);
		delay(12000000);

		// if (i == 200) {
		// 	GPIOA->ODR ^= GPIO_ODR_ODR9;
		// 	i = 0;
		// 	delay(1500000);
		// }
		// else {
		// 	GPIOA->ODR |= GPIO_ODR_ODR10;
		// 	i++;
		// 	delay(50000);
		// 	//delay(10000); // 0.1 second
		// 	GPIOA->ODR &= ~GPIO_ODR_ODR10;
		// }
	}
	
}

void SysTick_Handler(void) {
	GPIOC->ODR ^= GPIO_ODR_ODR8; // Toggle blue light on STM32
}

void EXTI0_IRQHandler(void) {
	EXTI->PR |= EXTI_PR_PR0; // Reset pending register
	GPIOC->ODR ^= GPIO_ODR_ODR9; // Toggle green light on STM32
}
