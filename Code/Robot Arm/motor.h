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
#define B_CLOCKWISE           ((uint32_t)0x00000800)
#define B_COUNTERCLOCKWISE    ((uint32_t)0x08000000)
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

// ----------------
// GLOBAL CONSTANTS
// ----------------
#define MOTOR_SPEED_LEVELS 3
#define MOTOR_SPEED_1 150
#define MOTOR_SPEED_2 100
#define MOTOR_SPEED_3 50
#define A1_HEIGHT 11 // 10.5cm from ground to axle, round up
#define A1_LENGTH 17 // 17cm joint to joint
#define A2_LENGTH 27 // 27cm joint to joint
// ----------------

void motors_init(void);
void timers_init(void);
void timers_enable(void);
void timers_disable(void);

uint8_t motors_change_speed(void);
void motors_all_hold(void);
uint8_t motors_get_speed(void);

void motors_b_clockwise(void);
void motors_b_counterclockwise(void);
void motors_b_move(uint32_t dir, float steps);

void motors_a1_extend(void);
void motors_a1_retract(void);
void motors_a1_move(uint32_t dir, float steps);

void motors_a2_extend(void);
void motors_a2_retract(void);
void motors_a2_move(uint32_t dir, float steps);
