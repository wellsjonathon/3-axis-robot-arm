/*
 *	Author: Jonathon Wells
 *	Description: Library to initialize and operate on 44780 LCD display
 *
 *	Class: ENEL387
 *	Date: Jan. 31, 2018
 *
 */
 
#include "stm32f10x.h"
 
// LCD Pins:
//		RS:			PB0
//		R/W:		PB5
//		ENA:		PB1
//		DB0-7:	PC0-7
 
// Commands for Hitachi 44780 compatible LCD controlles
#define LCD_8B2L	0x38 // Enable 8-bit data, 2 display lines
#define LCD_DCB 	0x0F // Enable Display, Cursor, Blink
#define LCD_MCR 	0x06 // Set Move Cursor Right
#define LCD_CLR 	0x01 // Home and clear LCD
#define LCD_LN1 	0x80 // Set DDRAM to start of line 1
#define LCD_LN2 	0xC0 // Set DDRAM to start of line 2
#define LCD_CB		0x0C

// Control signal manipulation for LCDs on 352/384/287 board
// 		RS: 	PB0
//		ENA:	PB1
// 		R/W:	PB5
#define LCD_CM_ENA 0x00210002 // RS low, Enable high
#define LCD_CM_DIS 0x00230000 // RS low, Enable low
#define LCD_DM_ENA 0x00200003 // RS high, Enable high
#define LCD_DM_DIS 0x00220001 // RS high, Enable low
 
void lcd_init(void);
void dataToLCD(uint8_t data);
void commandToLCD(uint8_t data);
void stringToLCD(uint8_t *ptr, uint8_t line);
uint8_t hexToAscii(uint8_t hex);
uint32_t printHexToLCD(uint32_t hex, uint8_t line);
