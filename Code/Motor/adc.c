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
	
	// Initialize ports as analog
	// 		PA1: Temp sensor
	//		PA2: Potentiometer
	GPIOA->CRL &= ~GPIO_CRL_MODE1 & ~GPIO_CRL_MODE2;
	GPIOA->CRL &= ~GPIO_CRL_CNF1 & ~GPIO_CRL_CNF2;
	
	// Turn on and calibrate ADC
	ADC1->CR2 |= ADC_CR2_CAL | ADC_CR2_ADON;
	
	// Select sample rate
	ADC1->SMPR2 |= ADC_SMPR2_SMP1 | ADC_SMPR2_SMP2;
	
	// Select sequence that channels will be read and converted
	//		Sequence 1: Channel 1
	//		Sequence 2: Channel 2
	//ADC1->SQR3 |= (ADC_SQR3_SQ1 & 0x1) | (ADC_SQR3_SQ2 & 0x2);
}

uint16_t adc_read(uint8_t channel) {
	// Select channel to read
	// 		May be useful if I pass in a channel to write to SQR3 SQ1
	//		May not be useful if I predetermined sequence in init
	ADC1->SQR3 = (ADC_SQR3_SQ1 & channel);
	
	// Start conversion sequence
	ADC1->CR2 |= ADC_START;
	
	// Wait for sequence to complete
	while (!(ADC1->SR & ADC_SR_EOC)) {
	}
	
	// Read and return data
	return (ADC1->DR & ADC_DR_DATA);
}
