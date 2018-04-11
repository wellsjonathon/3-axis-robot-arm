/*
 *	Author: Jonathon Wells
 *	Description: Library to initialize and operate on LCD display
 *
 *	Class: ENEL387
 *	Date: Jan. 31, 2018
 *
 */

#include "stm32f10x.h"
#include "lcd.h"
#include "m3_lib.h"

void lcd_init(void) {
	
	// Set config and mode bits as push-pull outputs
	GPIOB->CRL |= GPIO_CRL_MODE5 | GPIO_CRL_MODE1 | GPIO_CRL_MODE0;
	GPIOB->CRL &= ~GPIO_CRL_CNF5 & ~GPIO_CRL_CNF1 & ~GPIO_CRL_CNF0;
	
	// LCD_DATA: PC0-7
	GPIOC->CRL |= GPIO_CRL_MODE;
	GPIOC->CRL &= ~GPIO_CRL_CNF;
	
	delay(240000);
	commandToLCD(LCD_8B2L);
	commandToLCD(LCD_8B2L);
	commandToLCD(LCD_8B2L);
	commandToLCD(LCD_8B2L);
	//commandToLCD(0x08);
	commandToLCD(LCD_CLR);
	commandToLCD(LCD_DCB);
	commandToLCD(LCD_MCR);
	//commandToLCD(LCD_MCR);
	commandToLCD(LCD_CB);
}

void commandToLCD(uint8_t data) {
	GPIOB->BSRR = LCD_CM_ENA;
	
	GPIOC->ODR &= 0xFF00;
	GPIOC->ODR |= data;
	
	delay(8000);
	
	GPIOB->BSRR = LCD_CM_DIS;
	delay(80000);
}

void dataToLCD(uint8_t data) {
	GPIOB->BSRR = LCD_DM_ENA;
	
	GPIOC->ODR &= 0xFF00;
	GPIOC->ODR |= data;
	
	delay(8000);
	
	GPIOB->BSRR = LCD_DM_DIS;
	delay(80000);
}

void stringToLCD(uint8_t *ptr, uint8_t line) {
	commandToLCD(line);
	while (*ptr) {
		dataToLCD(*ptr);
		ptr++;
	}
}

uint8_t hexToAscii(uint8_t hex) {
	if (hex <= 0x09) {
		return hex + 0x30;
	}
	else {
		return hex + 0x37;
	}
}

uint32_t printHexToLCD(uint32_t hex, uint8_t line) {
	uint32_t temp;
	int count = 28;
	int i;
	uint8_t beforeHex[] = "0x";
	uint8_t *beforeHexPtr = beforeHex;
	
	//commandToLCD(line);
	stringToLCD(beforeHexPtr, line);
	
	for (i = 8; i > 0; i--) {
		temp = hex >> count;
		dataToLCD(hexToAscii(temp & 0xF));
		count -= 4;
	}
}
