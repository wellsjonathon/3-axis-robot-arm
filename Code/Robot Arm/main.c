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

// GLOBAL CONSTANTS
// ----------------
#define MOTOR_SPEED_LEVELS 3;
#define MOTOR_ON 1;
#define MOTOR_OFF 0;
#define MOTOR_EXTEND 1;
#define MOTOR_RETRACT 0;
#define MOTOR_CLOCKWISE 1;
#define MOTOR_COUNTERCLOCKWISE 0;
// ----------------

// GLOBAL VARIABLES
// ----------------
volatile uint16_t ADC_DATA[2];
uint8_t MOTOR_SPEED = 1;
_Bool MOTOR_B_STATE = MOTOR_OFF;
_Bool MOTOR_B_DIR = MOTOR_COUNTERCLOCKWISE;
_Bool MOTOR_A1_STATE = MOTOR_OFF;
_Bool MOTOR_A1_DIR = MOTOR_RETRACT;
_Bool MOTOR_A2_STATE = MOTOR_OFF;
_Bool MOTOR_A2_DIR = MOTOR_RETRACT;
// ----------------

void initialize() {
	// Initialize clocks
	clocks_init();
	
	// Initialize timers
	
	
	// Initialize interrupts
	
	
	// Initialize inputs
	adc_init(); // Joystick
	dma_init(ADC_DATA);
	input_init(); // Pushbuttons, dipswitches, limit switches
	
	// Initialize outputs
	led_init();
	lcd_init();
	motors_init();
	motors_all_hold();
	
	// LED Startup sequence to signal end of initialization
	led_startup();
}

uint8_t handle_mode(uint8_t prev_mode) {
	// Temp error code, extract out to err_codes.h
	uint8_t err_switches[] = "E:INVALID MODE";
	uint8_t select_mode[] = "SELECT MODE";
	uint8_t mode_manual[] = "MODE:MANUAL";
	uint8_t mode_memorize[] = "MODE:MEMORIZE";
	uint8_t mode_programmed[] = "MODE:PROGRAMMED";
	uint8_t *ptr_err = err_switches;
	uint8_t *ptr_select = select_mode;
	uint8_t *ptr_manual = mode_manual;
	uint8_t *ptr_memorize = mode_memorize;
	uint8_t *ptr_programmed = mode_programmed;
	
	// Get switches state
	uint8_t new_mode = switches_read();
	// If new mode is different, return new mode, else return old
	if (new_mode != prev_mode) {
		commandToLCD(LCD_CLR);
		commandToLCD(LCD_LN1);
		switch (new_mode & 0x0F){
			case 0x0F:
				GPIOA->BSRR |= LED1_OFF | LED2_OFF | LED3_OFF | LED4_OFF;
				stringToLCD(ptr_select);
				break;
			case 0x0E:
				GPIOA->BSRR |= LED1_ON | LED2_OFF | LED3_OFF | LED4_OFF;
				stringToLCD(ptr_manual);
				break;
			case 0x0D:
				GPIOA->BSRR |= LED1_OFF | LED2_ON | LED3_OFF | LED4_OFF;
				stringToLCD(ptr_memorize);
				break;
			case 0x0B:
				GPIOA->BSRR |= LED1_OFF | LED2_OFF | LED3_ON | LED4_OFF;
				stringToLCD(ptr_programmed);
				break;
			default:
				GPIOA->BSRR |= LED1_OFF | LED2_OFF | LED3_OFF | LED4_OFF;
				stringToLCD(ptr_err);
				printHexToLCD(new_mode, LCD_LN2);
				break;
		}
	}
	return new_mode;
}

int main(void) {
	
	int i = 0;
	uint8_t mode = 0;
	uint16_t ch1 = 0;
	uint16_t ch2 = 0;
	uint16_t prev_ch[2];
	uint32_t printable_adc = 0x00000000;
	uint32_t temp_adc = 0x00000000;
	
	initialize();
	
	while (1) {
		
		//mode = handle_mode(mode);
		
		printHexToLCD(ADC_DATA[0], LCD_LN1);
		printHexToLCD(ADC_DATA[1], LCD_LN2);
			
		for (int i = 1; i >= 0; --i) {
			if (prev_ch[i] != ADC_DATA[i]) {
				temp_adc |= ADC_DATA[i];
				temp_adc = temp_adc << i*16;
				prev_ch[i] = ADC_DATA[i];
			}
		}
		
		if (ADC_DATA[1] == 0x000) {
			motors_a1_move(A1_RETRACT, 6);
			//while (ADC_DATA[1] < 0x1) {
				
			//}
		}
		else if (ADC_DATA[1] == 0xFFF) {
			motors_a1_move(A1_EXTEND, 6);
		}
		
		if (ADC_DATA[0] == 0x000) {
			motors_b_move(B_COUNTERCLOCKWISE, 6);
		}
		else if (ADC_DATA[0] == 0xFFF) {
			motors_b_move(B_CLOCKWISE, 6);
		}
		
		if (temp_adc != printable_adc) {
			printable_adc = temp_adc;
			//printHexToLCD(printable_adc, LCD_LN2);
		}
		/*
		if (prev_ch1 == 0 || ch1 < prev_ch1 - 0x9 || ch1 > prev_ch1 + 0x9) {
			printHexToLCD(ch1, LCD_LN1);
			prev_ch1 = ch1;
		}
		if (prev_ch2 == 0 || ch2 < prev_ch2 - 0x9 || ch2 > prev_ch2 + 0x9) {
			printHexToLCD(ch2, LCD_LN2);
			prev_ch2 = ch2;
		}
		*/
	}	
}

void SysTick_Handler(void) {
	GPIOC->ODR ^= GPIO_ODR_ODR8; // Toggle blue light on STM32
}

void EXTI0_IRQHandler(void) {
	EXTI->PR |= EXTI_PR_PR0; // Reset pending register
	GPIOC->ODR ^= GPIO_ODR_ODR9; // Toggle green light on STM32
}

void DMA1_Channel1_IRQHandler(void) {
	if (DMA1->ISR & DMA_ISR_TEIF1) {
		GPIOA->BSRR = LED3_ON | LED4_OFF;
	}
	else {
		GPIOA->BSRR = LED4_ON | LED3_OFF; 
	}
	printHexToLCD(0xFFFFFFFF, LCD_LN2);
}
