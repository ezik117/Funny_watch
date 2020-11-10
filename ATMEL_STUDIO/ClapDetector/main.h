/*
 * main.h
 *
 * Created: 08.11.2020 21:25:24
 *  Author: user
 */ 


#ifndef MAIN_H_
#define MAIN_H_

// ----------------------------------------------------------------------------
#define F_CPU 9600000UL // SYSCLK=9.6MHz, HSI=9.6MHz, CKDIV8=0, Attiny13A

// ----------------------------------------------------------------------------
#include <avr/io.h>
#include <avr/portpins.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// ----------------------------------------------------------------------------
#define TRUE 1
#define FALSE 0
#define bool uint8_t
#define SET_BIT(REG, BIT) ((REG) |= (1<<BIT))
#define CLEAR_BIT(REG, BIT) ((REG) &= ~(1<<BIT))
#define READ_BIT(REG, BIT) ((REG) & (1<<BIT))
#define TOGGLE_BIT(REG, BIT) ((REG) ^= (1<<BIT))
#define MODIFY_REG(REG, CLEARMASK, SETMASK) (WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK))))

// ----------------------------------------------------------------------------
/* ���� ������ ������� */
typedef enum TimerTypes
{
	NORMAL, // �������������� �������
	GROW,   // ��������� �������
	FADE	// ��������� �������
} TimerTypes;

/* ������� ����� ������� */
volatile TimerTypes TimerMode;

// ----------------------------------------------------------------------------
typedef enum SignalTypes
{
	SIGNAL_DETECTED,  // ��������� ������ (������� �� H � L)
	SILENCE_DETECTED, // ���������� ������ (������� �� L � H)
	NONE,             // ������ ���������
	SHUTDOWN          // ������������� ��������� ��������
} SignalTypes;

/* ������� ��������� ��������� ����� */
volatile SignalTypes SignalMode;

// ----------------------------------------------------------------------------
/* ��������� �������� */
volatile bool LoadState = 0; // 0-���������, 1-��������

/* ������� �������� ��������� DutyCycle ��� ������ PWM */
volatile uint8_t PwmDivider;
const uint8_t PwmDividerTop = 255;

/* ���������� ��� �������������� ������� */
volatile uint16_t ticks = 0;    // ������������ ������� � ����
const uint16_t ticksMax = 9999; // ����������� ���������� ����� �������� �������� ����/������

volatile int PrgStep = 0; // ������� ��������� ���������� ��������� (����� ����*2)
const uint8_t PrgMaxSteps = 6; // ���������� ����� � ���������




/* ��������� */
const uint16_t Prg[] = {
	0, 0xFFFF, // ������ ����� �����
	40, 120,      // ������
	500, 1000,     // ������ ������
	40, 120,      // ������
	100, 500,     // �������� ������
40, 120}; // ��������� (1 ������, �����, 2 ������� ������)


/* ��������� ������� */
void ConfigureTimer(TimerTypes TimerType);
void ResetProgram();
void DbgSend(uint16_t data, uint8_t length);
volatile uint16_t dbgData[7] = {0,0,0,0,0,0,0};
volatile uint16_t dbgFlag = 0;
void DbgStartSequence();
void DbgSendStat();
void delay5(uint8_t fives);

volatile bool resetCounter = FALSE;



#endif /* MAIN_H_ */