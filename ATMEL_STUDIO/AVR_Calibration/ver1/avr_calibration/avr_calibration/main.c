/*
 * avr_calibration.c
 *
 * Created: 09.11.2020 9:46:49
 * Author : ermolaea
 */ 

#include "main.h"



int main(void)
{
	sei();
	
	OSCCAL = 88;
	
	SET_BIT(DDRB, 4); // PB2 - out, PB3 - in

	ticks = countedTicks = reportTick = 0;
	calibrationComplete = FALSE;
	
	SET_BIT(TIMSK0, OCIE0A); //включить прерывание по совпадению OCRА
	OCR0A = 149; // 149 = 1мс (1кГц)
	// (1) ШИМ на пине OC0A отключен
	// (2) ШИМ на пине OC0B отключен
	// (3) CTC режим. Счет идет от 0 до значения в регистре OCR0A
	// (4) Force Output Compare отключен
	// (5) Запустить таймер. Источник частоты SysCLK/64
	TCCR0A = (0<<COM0A1 | 0<<COM0A0) |      // (1)
			(0<<COM0B1 | 0<<COM0B0) |       // (2)
			(1<<WGM01 | 0<<WGM00);          // (3)
	TCCR0B = (0<<FOC0A | 0<<FOC0B) |        // (4)
			(0<<WGM02) |                    // (3)
			(0<<CS02 | 1<<CS01 | 1<<CS00);  // (5)
		
	SET_BIT(PCMSK, PCINT3); // включить прерывания от порта PB3
	SET_BIT(GIMSK, PCIE); // включить прерывания GPIO
	
	dbgSend(OSCCAL, 16);

    while (!calibrationComplete)
	{
		if (reportTick >= 5)
		{
			cli();
			dbgSend(countedTicks, 16);
			dbgSend(OSCCAL, 16);
			reportTick = 0;
			sei();			
		}
	}
	
	dbgSend(countedTicks, 16);
	dbgSend(OSCCAL, 16);

	while(1);
}


// ------------------------------------------------------------------------------------------------
ISR (PCINT0_vect)
{
	if (calibrationComplete) return;
	
	if ((PINB & 8) == 8)
	{
		ticks = 0;
		
		//SET_BIT(PORTB, 4);
	}
	else
	{
		//CLEAR_BIT(PORTB, 4);
		
		reportTick++;
		countedTicks = ticks;
		//if (countedTicks < 999)
		//{
			//if (OSCCAL < 0x7F) OSCCAL++;
		//}
		//else if (countedTicks > 1001)
		//{
			//if (OSCCAL > 2) OSCCAL--;
		//}
		//else
		//{
			//calibrationComplete = TRUE;
		//}
	}
}


// ------------------------------------------------------------------------------------------------
ISR (TIM0_COMPA_vect)
{
	ticks++;
}


// ------------------------------------------------------------------------------------------------
void delay5(uint8_t cycles)
{
	while(cycles--) 
	{
		_delay_ms(5);
	}
}


// ------------------------------------------------------------------------------------------------
#define DBGPIN 4
void dbgSend(uint16_t data, uint8_t bits)
{
	SET_BIT(PORTB, DBGPIN);
	delay5(150/5);
	CLEAR_BIT(PORTB, DBGPIN);
	delay5(10/5);
	
	while(bits--)
	{
		SET_BIT(PORTB, DBGPIN);
		if ((data & 1) == 1)
			delay5(50/5);
		else
			delay5(25/5);
			
		CLEAR_BIT(PORTB, DBGPIN);
		data >>= 1;
		delay5(10/5);
	}
	
	SET_BIT(PORTB, DBGPIN);
	delay5(100/5);
	CLEAR_BIT(PORTB, DBGPIN);
	delay5(10/5);
}