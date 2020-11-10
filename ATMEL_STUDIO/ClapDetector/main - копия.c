/*
 * ClapDetector.c
 *
 * Created: 17.10.2020 23:38:17
 * Author : user
 */ 

#include "main.h"


int main(void)
{
	sei();
	
	// настройка GPIO
	CLEAR_BIT(DDRB, DD3); // PB3 - вход
	SET_BIT(DDRB, DD4); // PB4 - выход отладчика
	
	//ConfigureTimer(GROW);
	ConfigureTimer(NORMAL);
	
	
	
    while (1)
	{
		if (dbgFlag)
		{
			SignalMode = SHUTDOWN;
			DbgSend(dbgFlag, 16);
			dbgFlag = 0;
			SignalMode = NONE;
		}
	};
	

}

// ----------------------------------------------------------------------------
/* Конфигурирование и запуск таймера */
void ConfigureTimer(TimerTypes TimerType)
{
	SignalMode = SHUTDOWN; // временно отключили измерение сигналов
	
	switch (TimerType)
	{
		/* Конфигурирование таймера в режиме счетчика, с периодом ~1миллисекунда */
		case NORMAL:
			// (1) ШИМ на пине OC0A отключен
			// (2) ШИМ на пине OC0B отключен
			// (3) CTC режим. Счет идет от 0 до значения в регистре OCR0A
			// (4) Force Output Compare отключен
			// (5) Запустить таймер. Источник частоты SysCLK/64
	
			TCCR0B = 0; // отключить таймер
			TCCR0A = 0;
			TimerMode = TimerType;
			ResetProgram();
			SET_BIT(TIMSK0, OCIE0A); //включить прерывание по совпадению OCRА
			OCR0A = 149; // 149 = 1мс (1кГц)
			OCR0B = 0;
			PwmDivider = 0;
			TCCR0A = (0<<COM0A1 | 0<<COM0A0) |       // (1)
			         (0<<COM0B1 | 0<<COM0B0) |       // (2)
			         (1<<WGM01 | 0<<WGM00);          // (3)
			TCCR0B = (0<<FOC0A | 0<<FOC0B) |         // (4)
			         (0<<WGM02) |                    // (3)
			         (0<<CS02 | 1<<CS01 | 1<<CS00);  // (5)
			a = 0;
			
			SET_BIT(PCMSK, PCINT3); // включить прерывания от порта PB3	
			SET_BIT(GIMSK, PCIE); // включить прерывания GPIO
			break;
			
		/* Конфигурирование таймера в режиме ШИМ */
		case FADE:
		case GROW:
			// (1) ШИМ на пине OC0A включен в режиме увеличения DutyCycle или уменьшения
			// (2) ШИМ на пине OC0B отключен
			// (3) 8-бит Fast PWM режим 7. Duty cycle идет от 0 до значения в регистре OCR0B. Частота от 0 до OCR0A
			// (4) Force Output Compare отключен
			// (5) Запустить PWM. Источник частоты SysCLK
			
			CLEAR_BIT(GIMSK, PCIE); // выключить прерывания GPIO
			TCCR0B = 0; // отключить таймер
			TCCR0A = 0;
			TimerMode = TimerType;
			SET_BIT(TIMSK0, OCIE0A); //включить прерывание по совпадению OCRА
			OCR0A = 239; // Частота ШИМ = 20кГц
			OCR0B = 0; // Начальное значение рабочего цикла
			PwmDivider = 0;
			TCCR0A = (TimerType == GROW ? (1<<COM0B1 | 0<<COM0B0) : (1<<COM0B1 | 1<<COM0B0)) | // (1)
			         (0<<COM0A1 | 0<<COM0A0) |       // (2)
			         (1<<WGM01 | 1<<WGM00);          // (3)
			TCCR0B = (0<<FOC0A | 0<<FOC0B) |         // (4)
			         (1<<WGM02) |                    // (3)
			         (0<<CS02 | 0<<CS01 | 1<<CS00);  // (5)
			break;
	}
	
	SignalMode = NONE; // включили измерение сигналов
}

// ----------------------------------------------------------------------------
void ResetProgram()
{
	ticks = 0;
	PrgStep = 0;
}

// ----------------------------------------------------------------------------
/* Прерывание по таймеру */
ISR(TIM0_COMPA_vect)
{	
	if (SignalMode == SHUTDOWN) return;
	
	if (TimerMode == NORMAL)
	{
		// отсчет длительности сигнала/тишины. сброс по переполнению
		ticks++;
		if (ticks >= ticksMax)
		{
			ResetProgram();
		}
	}
	else
	{
		//// Плавное изменение яркости
		//PwmDivider++;
		//if (PwmDivider >= PwmDividerTop)
		//{
			//PwmDivider = 0;
			//OCR0B++;
			//if (OCR0B > OCR0A)
			//{
				//if (TimerMode == GROW)
				//{
					////SET_BIT(PORTB, PORTB1); // включили нагрузку постоянно
					//LoadState = 1;
				//}
				//else
				//{
					////CLEAR_BIT(PORTB, PORTB1); // выключили нагрузку постоянно
					//LoadState = 0;
				//}
				//ConfigureTimer(NORMAL);
			//}
		//}
	}
}

// ----------------------------------------------------------------------------
/* Прерывание по фронту сигнала */
ISR (PCINT0_vect)
{
	if (SignalMode == SHUTDOWN) return;
	
	// -- определение текущего состояния входа: звук или тишина
	if ((PINB & 8) == 8) // PIN3
	{
		// HI
		SignalMode = SILENCE_DETECTED;
		//SET_BIT(PORTB, PORTB4);
	}
	else
	{
		// LO
		SignalMode = SIGNAL_DETECTED;
		//CLEAR_BIT(PORTB, PORTB4);
	}
	
	//if (PrgStep == 0 && SignalMode == SILENCE_DETECTED)
	//{
		//ResetProgram();
		//SignalMode = NONE;
		//return;
	//}
	
	SignalMode = SHUTDOWN;
	
	dbgData[PrgStep] = ticks;
	dbgData[6] = PrgStep;
	dbgFlag = ticks;
	return;
	
    // проверка условия выполнения шага программы: если длительность звука/тишины в заданном диапазоне
    if ( (ticks >= Prg[(PrgStep<<1) + 0]) && (ticks <= Prg[(PrgStep<<1) + 1]) )
    {
	    PrgStep++; // следующей шаг программы

	    if (PrgStep >= PrgMaxSteps)
	    {
		    // программа выполнена - выполяем действие: меняем состояние светодиода
		    // и сбрасываем шаг программы в начало
			//if (LoadState == 0)
			//{
				//ConfigureTimer(GROW);
			//}
			//else
			//{
				//ConfigureTimer(FADE);
			//}
			
			//DbgSendStat();
		    ResetProgram();
	    }
    }
    else
    {
		// ОТЛАДКА!!!!
		//DbgSendStat();
		
	    // заданные данные шага программы не совпадают с реальными
	    // сбрасываем шаг программы в начало
		ResetProgram();
    }	
	
	// -- перезапуск отсчета
	ticks = 0;
	SignalMode = NONE;
}

// -----------------------------------------------------------------------------
/* Отправляет указанное кол-во байт данных, LSB first -> */
/*
Протокол:
150мс - начало фрейма
50мс -  передача "1"
25мс -  передача "0"
100мс - конец фрейма
пауза ("0") между импульсами 10мс

200мс - начало последовательности
*/
void DbgSend(uint16_t data, uint8_t bits)
{
	// начало фрейма
	SET_BIT(PORTB, PORTB4);
	delay5(15/5);
	
	CLEAR_BIT(PORTB, PORTB4);
	delay5(10/5);
	
	// данные
	for (uint8_t i=0; i<bits; i++)
	{
		if ( (data & 1) == 1)
		{
			SET_BIT(PORTB, PORTB4);
			delay5(50/5);
		}
		else
		{
			SET_BIT(PORTB, PORTB4);
			delay5(25/5);
		}
		
		data >>= 1;
		
		CLEAR_BIT(PORTB, PORTB4);
		delay5(10/5);
	}
	
	// конец фрейма
	SET_BIT(PORTB, PORTB4);
	delay5(100/5);
	CLEAR_BIT(PORTB, PORTB4);
	delay5(10/5);
}

// ----------------------------------------------------------------------------
void DbgStartSequence()
{
	SET_BIT(PORTB, PORTB4);
	delay5(200/5);
	CLEAR_BIT(PORTB, PORTB4);
	delay5(10/5);
}

void DbgSendStat()
{
	DbgStartSequence();
	
		
	for (uint8_t ii=0; ii<7; ii++)
		DbgSend(dbgData[ii], 16);
}

void delay5(uint8_t fives)
{
	while (fives--)
	{
		_delay_ms(5);
	}
}