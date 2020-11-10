/*
 * main.h
 *
 * Created: 09.11.2020 9:49:03
 *  Author: ermolaea
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 9600000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define FALSE 0
#define TRUE 1
#define SET_BIT(REG, BIT) ((REG) |= (1<<BIT))
#define CLEAR_BIT(REG, BIT) ((REG) &= ~(1<<BIT))
#define bool uint8_t

volatile uint16_t ticks;
volatile uint16_t countedTicks;
volatile bool calibrationComplete;

volatile uint8_t reportTick;

void delay5(uint8_t cycles);
void dbgSend(uint16_t data, uint8_t bits);

#endif /* MAIN_H_ */