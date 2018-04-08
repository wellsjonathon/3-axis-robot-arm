/*
 *	Author: Jonathon Wells
 *	Description: Library to initialize and work with ADC
 *
 *	Class: ENEL387
 *	Date: Feb. 7, 2018
 *
 */

#include "stm32f10x.h"
#include "m3_lib.h"
#include "adc.h"

void adc_init(void) {
	// Enable clock
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	
	// THIS SHOULD BE DONE IN INPUT INIT
	// Initialize ports as analog
	// 		PA1: JOY_X
	//		PA2: JOY_Y
	GPIOA->CRL &= ~GPIO_CRL_MODE1 & ~GPIO_CRL_CNF1;
	GPIOA->CRL &= ~GPIO_CRL_MODE2 & ~GPIO_CRL_CNF2;
	
	// Calibrate ADC
	//ADC1->CR2 |= ADC_CR2_CAL;
	// Wait for calibration to finish
	//while (ADC1->CR2 & ADC_CR2_CAL) {
	//}
	
	// Turn on and initialize ADC
	// Enable continuous conversion, DMA, and scan mode
	ADC1->CR2 |= ADC_CR2_ADON 
						 | ADC_CR2_CONT 
						 | ADC_CR2_DMA;
	ADC1->CR1 |= ADC_CR1_SCAN;
	
	// Select sample rate
	ADC1->SMPR2 |= ADC_SMPR2_SMP1 
							 | ADC_SMPR2_SMP2;
	
	// Select sequence that channels will be read and converted
	//		Sequence 1: Channel 1 - JOY_X
	//		Sequence 2: Channel 2 - JOY_Y
	ADC1->SQR1 |= ADC_SQR1_L_0; // Set to two sequences
	ADC1->SQR3 |= ADC_SQR3_SQ1_0 
							| ADC_SQR3_SQ2_1;
}

void dma_init(volatile uint16_t *destination) {
	// Enable clock
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	
	// Set peripheral (source) address
	DMA1_Channel1->CPAR = (uint32_t)(&ADC1->DR);
	// Set memory (destination) address to passed in array
	DMA1_Channel1->CMAR = (uint32_t)(destination);
	// Set number of transfers - JOY_X and JOY_Y
	DMA1_Channel1->CNDTR = 2;
	
	// Initiliaze channel 1
	// Circular mode, memory increment, 16-bit memory and peripheral size
	DMA1_Channel1->CCR |= DMA_CCR1_CIRC 
											| DMA_CCR1_MINC
											| DMA_CCR1_MSIZE_0 
											| DMA_CCR1_PSIZE_0;
	
	// Enable channel 1
	DMA1_Channel1->CCR |= DMA_CCR1_EN;
	
	// Turn on ADC
	ADC1->CR2 |= ADC_CR2_ADON;
}

uint16_t adc_read(uint8_t channel) {
	// Select channel to read
	// 		May be useful if I pass in a channel to write to SQR3 SQ1
	//		May not be useful if I predetermined sequence in init
	//ADC1->SQR3 = (ADC_SQR3_SQ1 & channel);
	
	// 0x01110222
	//		- 1 = JOY_X
	//		- 2 = JOY_Y
	
	// Start conversion sequence
	ADC1->CR2 |= ADC_START;
	
	// Wait for sequence to complete
	while (!(ADC1->SR & ADC_SR_EOC)) {
	}
	
	// Read and return data
	return (ADC1->DR & ADC_DR_DATA);
}

uint32_t adc_joystick(void) {
	uint32_t result = 0;
	for (int i = 0; i < 2; ++i) {
		if (result != 0) {
			result = result << 16;
		}
		result |= adc_read(0);
	}
	return result;
}
