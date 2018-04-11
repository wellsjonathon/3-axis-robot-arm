//
//	Author: Jonathon Wells
//	Date: Mar 3, 2018
//

#include "m3_lib.h"
#include "interrupts.h"
#include "adc.h"
#include "lcd.h"
#include "motor.h"
#include "stm32f10x.h"
#include "stdio.h"

// GLOBAL CONSTANTS
// ----------------
#define MOTOR_ON 1
#define MOTOR_OFF 0
#define MOTOR_EXTEND 1
#define MOTOR_RETRACT 0
#define MOTOR_CLOCKWISE 1
#define MOTOR_COUNTERCLOCKWISE 0
#define JOY_LOW 0x100
#define JOY_HIGH 0xF00
// ----------------

// GLOBAL VARIABLES
// ----------------
volatile uint16_t ADC_DATA[2];
int MOTOR_SPEED;
_Bool MOTOR_B_STATE = MOTOR_OFF;
_Bool MOTOR_B_DIR = MOTOR_COUNTERCLOCKWISE;
_Bool MOTOR_A1_STATE = MOTOR_OFF;
_Bool MOTOR_A1_DIR = MOTOR_RETRACT;
_Bool MOTOR_A2_STATE = MOTOR_OFF;
_Bool MOTOR_A2_DIR = MOTOR_RETRACT;
uint16_t PREV_DMA_CH[2];
uint32_t TEMP_ADC = 0x00000000;
// ----------------

void initialize() {
	// Initialize clocks
	clocks_init();
	
	// Initialize interrupts
	interrupt_init();
	
	// Initialize inputs
	adc_init(); // Joystick
	dma_init(ADC_DATA);
	input_init(); // Pushbuttons, dipswitches, limit switches
	
	// Initialize outputs
	led_init();
	lcd_init();
	motors_init();
	motors_all_hold();
	
	// Fetch motor speed
	MOTOR_SPEED = motors_get_speed();
	
	// LED Startup sequence to signal end of initialization
	timers_disable();
	led_startup();
	timers_enable();
}

uint8_t handle_mode(uint8_t prev_mode) {
	// Temp error code, extract out to err_codes.h
	uint8_t err_switches[] = "E:INVALID MODE";
	uint8_t select_mode[] = "SELECT MODE";
	uint8_t mode_manual[] = "MODE:MANUAL";
	uint8_t mode_memorize[] = "MODE:MEMORIZE";
	uint8_t mode_programmed[] = "MODE:PROGRAMMED";
	uint8_t speed[] = "SPEED: ";
	uint8_t *ptr_err = err_switches;
	uint8_t *ptr_select = select_mode;
	uint8_t *ptr_manual = mode_manual;
	uint8_t *ptr_memorize = mode_memorize;
	uint8_t *ptr_programmed = mode_programmed;
	uint8_t *ptr_speed = speed;
	
	// Get switches state
	uint8_t new_mode = switches_read();
	// If new mode is different, return new mode, else return old
	if (new_mode != prev_mode) {
		commandToLCD(LCD_CLR);
		commandToLCD(LCD_LN1);
		switch (new_mode & 0x0F){
			case 0x0F:
				GPIOA->BSRR |= LED1_OFF | LED2_OFF | LED3_OFF | LED4_OFF;
				stringToLCD(ptr_select, LCD_LN1);
				timers_disable();
				break;
			case 0x0E:
				GPIOA->BSRR |= LED1_ON | LED2_OFF | LED3_OFF | LED4_OFF;
				stringToLCD(ptr_manual, LCD_LN1);
				stringToLCD(speed, LCD_LN2);
				dataToLCD(MOTOR_SPEED + '0');
				timers_enable();
				break;
			case 0x0D:
				GPIOA->BSRR |= LED1_OFF | LED2_ON | LED3_OFF | LED4_OFF;
				stringToLCD(ptr_memorize, LCD_LN1);
				timers_disable();
				break;
			case 0x0B:
				GPIOA->BSRR |= LED1_OFF | LED2_OFF | LED3_ON | LED4_OFF;
				stringToLCD(ptr_programmed, LCD_LN1);
				timers_disable();
				break;
			default:
				GPIOA->BSRR |= LED1_OFF | LED2_OFF | LED3_OFF | LED4_OFF;
				stringToLCD(ptr_err, LCD_LN1);
				printHexToLCD(new_mode, LCD_LN2);
				timers_disable();
				break;
		}
	}
	if ((GPIOD->IDR & GPIO_IDR_IDR2) == 0) {
		MOTOR_SPEED = motors_change_speed();		
		stringToLCD(speed, LCD_LN2);
		dataToLCD(MOTOR_SPEED + '0');
	}
	return new_mode;
}

int main(void) {
	
	int i = 0;
	uint8_t mode = 0;
	
	initialize();
	
	while (1) {
		
		mode = handle_mode(mode);
		
		// Simply for debugging purposes, print joystick data
		// JOY_X on LCD_LN1 -- JOY_Y on LCD_LN2
		//printHexToLCD(ADC_DATA[0], LCD_LN1);
		//printHexToLCD(ADC_DATA[1], LCD_LN2);
		//printHexToLCD(TIM2->ARR, LCD_LN1);
		//printHexToLCD(MOTOR_A1_STATE, LCD_LN2);
	}	
}

// ------------------
// INTERRUPT HANDLERS
// ------------------

void DMA1_Channel1_IRQHandler(void) {
	if (DMA1->ISR & DMA_ISR_TEIF1) {
		//printHexToLCD(ERR_DMA_TRANSFER_ERR, LCD_LN2);
	}
	else {
		// Determine JOY_X values
		if (ADC_DATA[1] < JOY_LOW) {
				MOTOR_B_STATE = MOTOR_ON;
				MOTOR_B_DIR = MOTOR_COUNTERCLOCKWISE;
		} else if (ADC_DATA[1] > JOY_HIGH) {
				MOTOR_B_STATE = MOTOR_ON;
				MOTOR_B_DIR = MOTOR_CLOCKWISE;
		} else {
				MOTOR_B_STATE = MOTOR_OFF;
		}
		// Determine JOY_Y values
		if (ADC_DATA[0] > JOY_HIGH) {
				MOTOR_A1_STATE = MOTOR_ON;
				MOTOR_A1_DIR = MOTOR_RETRACT;
		} else if (ADC_DATA[0] < JOY_LOW) {
				MOTOR_A1_STATE = MOTOR_ON;
				MOTOR_A1_DIR = MOTOR_EXTEND;
		} else {
				MOTOR_A1_STATE = MOTOR_OFF;
		}
	}
	DMA1->IFCR = DMA_IFCR_CTCIF1;
}

void TIM2_IRQHandler(void) {
	// Clear update interrupt flag
	TIM2->SR &= ~TIM_SR_UIF;
	GPIOA->BSRR |= LED1_OFF | LED2_OFF | LED3_OFF | LED4_OFF;
	// Move BASE
	if (MOTOR_B_STATE == MOTOR_ON) {
		motors_b_move(MOTOR_B_DIR ? B_CLOCKWISE : B_COUNTERCLOCKWISE, 1);
		GPIOA->BSRR |= LED1_ON;
	}
	// Move ARM1
	if (MOTOR_A1_STATE == MOTOR_ON) {
		motors_a1_move(MOTOR_A1_DIR ? A1_EXTEND : A1_RETRACT, 1);
		GPIOA->BSRR |= LED2_ON;
	}
	// Move ARM2
	if (MOTOR_A2_STATE == MOTOR_ON) {
		motors_a2_move(MOTOR_A2_DIR ? A2_EXTEND : A2_RETRACT, 1);
		GPIOA->BSRR |= LED3_ON;
	}
}

/*
void EXTI2_IRQHandler(void) {
	if (EXTI->PR & EXTI_PR_PR2) {
		// Make sure this only happens in manual? Or should it happen in other modes?
		MOTOR_SPEED = motors_change_speed();
		GPIOA->BSRR = LED3_ON;
		printHexToLCD(MOTOR_SPEED, LCD_LN2);
		EXTI->PR |= EXTI_PR_PR2;
	}
}
*/
