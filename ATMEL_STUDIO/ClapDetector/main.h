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
/* Типы режима таймера */
typedef enum TimerTypes
{
	NORMAL, // детектирование сигнала
	GROW,   // увеличить яркость
	FADE	// уменьшить яркость
} TimerTypes;

/* Текущий режим таймера */
volatile TimerTypes TimerMode;

// ----------------------------------------------------------------------------
typedef enum SignalTypes
{
	SIGNAL_DETECTED,  // обнаружен сигнал (переход от H к L)
	SILENCE_DETECTED, // обнаружена тишина (переход от L к H)
	NONE,             // сигнал обработан
	SHUTDOWN          // приостановить обработку сигналов
} SignalTypes;

/* текущее состояние детектора звука */
volatile SignalTypes SignalMode;

// ----------------------------------------------------------------------------
/* Состояние нагрузки */
volatile bool LoadState = 0; // 0-выключена, 1-включена

/* Счетчик плавного изменения DutyCycle для режима PWM */
volatile uint8_t PwmDivider;
const uint8_t PwmDividerTop = 255;

/* переменные для детектирования сигнала */
volatile uint16_t ticks = 0;    // длительность сигнала в мсек
const uint16_t ticksMax = 9999; // максимально допустимое время ожидания перепада звук/тишина

volatile int PrgStep = 0; // текущее состояние выполнения программы (номер шага*2)
const uint8_t PrgMaxSteps = 6; // количество шагов в программе




/* Программы */
const uint16_t Prg[] = {
	0, 0xFFFF, // тишина любой длины
	40, 120,      // хлопок
	500, 1000,     // долгая тишина
	40, 120,      // хлопок
	100, 500,     // короткая тишина
40, 120}; // программа (1 хлопок, пауза, 2 быстрых хлопка)


/* Прототипы функций */
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