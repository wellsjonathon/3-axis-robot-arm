/*
 *	Author: Jonathon Wells
 *	Description: Library to initialize and control motors
 *
 *	Class: ENEL387
 *	Date: April 2018
 *
 */
 
#include "stm32f10x.h"

// Motors:
// 	Base:
//		Step: PB10
//		Dir: PB11
//	A1:
//		Step: PB12
//		Dir: PB13
//	A2:
//		Step: PB14
//		Dir: PB15

// Bit masks for motors
// Base -- BSRR:
#define B_BLOCKWISE           ((uint32_t)0x00000800)
#define B_COUNTERCLOCKWISE    ((uint32_t)0x08000000
#define B_STEP_ON             ((uint32_t)0x00000400)
#define B_STEP_OFF            ((uint32_t)0x04000000)
// A1 -- BSRR:
#define A1_EXTEND             ((uint32_t)0x00002000)
#define A1_RETRACT            ((uint32_t)0x20000000)
#define A1_STEP_ON            ((uint32_t)0x00001000)
#define A1_STEP_OFF           ((uint32_t)0x10000000)
// A2 -- BSRR:
#define A2_EXTEND             ((uint32_t)0x00008000)
#define A2_RETRACT            ((uint32_t)0x80000000)
#define A2_STEP_ON            ((uint32_t)0x00004000)
#define A2_STEP_OFF           ((uint32_t)0x40000000)

void motors_init(void);

void motors_b_clockwise(void);
void motors_b_counterclockwise(void);
void motors_b_move(uint32_t dir, uint16_t steps);

void motors_a1_extend(void);
void motors_a1_retract(void);
void motors_a1_move(uint32_t dir, uint16_t steps);

void motors_a2_extend(void);
void motors_a2_retract(void);
void motors_a2_move(uint32_t dir, uint16_t steps);
