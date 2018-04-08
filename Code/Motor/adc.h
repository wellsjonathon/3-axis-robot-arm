/*
 *	Author: Jonathon Wells
 *	Description: Library to initialize and work with ADC
 *
 *	Class: ENEL387
 *	Date: Feb. 7, 2018
 *
 */
 
#include "stm32f10x.h"
 
#define ADC_START 0x1
 
// Analog channels
//		Channel 1: Temp sensor
//		Channel 2: Potentiometer
#define ANALOG_CHANNEL_1 0x1
#define ANALOG_CHANNEL_2 0x2
 
void adc_init(void);
uint16_t adc_read(uint8_t channel);
