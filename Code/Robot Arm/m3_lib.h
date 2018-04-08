/*
 *	Author: Jonathon Wells
 *	Description: Library to initialize Master Clock and Port Clock, and to setup Ports
 *
 *	Class: ENEL387
 *	Date: Jan. 24, 2018
 *
 */
 
#include "stm32f10x.h"
 
// Bit masks for LEDs on ENEL387 development board
// LED1 = A9
// LED2 = A10
// LED3 = A11
// LED4 = A12
#define LED1_OFF 0x00000200
#define LED1_ON 0x02000000
#define LED2_OFF 0x00000400
#define LED2_ON 0x04000000
#define LED3_OFF 0x00000800
#define LED3_ON 0x08000000
#define LED4_OFF 0x00001000
#define LED4_ON 0x10000000
 
void clocks_init(void);
void led_init(void);
void input_init(void);
void delay(uint32_t count);
void led_startup(void);
uint8_t switches_read(void);
uint8_t readButtons(void);
