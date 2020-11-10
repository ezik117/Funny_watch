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
	
	// ��������� GPIO
	CLEAR_BIT(DDRB, DD3); // PB3 - ����
	SET_BIT(DDRB, DD4); // PB4 - ����� ���������
	
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
/* ���������������� � ������ ������� */
void ConfigureTimer(TimerTypes TimerType)
{
	SignalMode = SHUTDOWN; // �������� ��������� ��������� ��������
	
	switch (TimerType)
	{
		/* ���������������� ������� � ������ ��������, � �������� ~1������������ */
		case NORMAL:
			// (1) ��� �� ���� OC0A ��������
			// (2) ��� �� ���� OC0B ��������
			// (3) CTC �����. ���� ���� �� 0 �� �������� � �������� OCR0A
			// (4) Force Output Compare ��������
			// (5) ��������� ������. �������� ������� SysCLK/64
	
			TCCR0B = 0; // ��������� ������
			TCCR0A = 0;
			TimerMode = TimerType;
			ResetProgram();
			SET_BIT(TIMSK0, OCIE0A); //�������� ���������� �� ���������� OCR�
			OCR0A = 149; // 149 = 1�� (1���)
			OCR0B = 0;
			PwmDivider = 0;
			TCCR0A = (0<<COM0A1 | 0<<COM0A0) |       // (1)
			         (0<<COM0B1 | 0<<COM0B0) |       // (2)
			         (1<<WGM01 | 0<<WGM00);          // (3)
			TCCR0B = (0<<FOC0A | 0<<FOC0B) |         // (4)
			         (0<<WGM02) |                    // (3)
			         (0<<CS02 | 1<<CS01 | 1<<CS00);  // (5)
			a = 0;
			
			SET_BIT(PCMSK, PCINT3); // �������� ���������� �� ����� PB3	
			SET_BIT(GIMSK, PCIE); // �������� ���������� GPIO
			break;
			
		/* ���������������� ������� � ������ ��� */
		case FADE:
		case GROW:
			// (1) ��� �� ���� OC0A ������� � ������ ���������� DutyCycle ��� ����������
			// (2) ��� �� ���� OC0B ��������
			// (3) 8-��� Fast PWM ����� 7. Duty cycle ���� �� 0 �� �������� � �������� OCR0B. ������� �� 0 �� OCR0A
			// (4) Force Output Compare ��������
			// (5) ��������� PWM. �������� ������� SysCLK
			
			CLEAR_BIT(GIMSK, PCIE); // ��������� ���������� GPIO
			TCCR0B = 0; // ��������� ������
			TCCR0A = 0;
			TimerMode = TimerType;
			SET_BIT(TIMSK0, OCIE0A); //�������� ���������� �� ���������� OCR�
			OCR0A = 239; // ������� ��� = 20���
			OCR0B = 0; // ��������� �������� �������� �����
			PwmDivider = 0;
			TCCR0A = (TimerType == GROW ? (1<<COM0B1 | 0<<COM0B0) : (1<<COM0B1 | 1<<COM0B0)) | // (1)
			         (0<<COM0A1 | 0<<COM0A0) |       // (2)
			         (1<<WGM01 | 1<<WGM00);          // (3)
			TCCR0B = (0<<FOC0A | 0<<FOC0B) |         // (4)
			         (1<<WGM02) |                    // (3)
			         (0<<CS02 | 0<<CS01 | 1<<CS00);  // (5)
			break;
	}
	
	SignalMode = NONE; // �������� ��������� ��������
}

// ----------------------------------------------------------------------------
void ResetProgram()
{
	ticks = 0;
	PrgStep = 0;
}

// ----------------------------------------------------------------------------
/* ���������� �� ������� */
ISR(TIM0_COMPA_vect)
{	
	if (SignalMode == SHUTDOWN) return;
	
	if (TimerMode == NORMAL)
	{
		// ������ ������������ �������/������. ����� �� ������������
		ticks++;
		if (ticks >= ticksMax)
		{
			ResetProgram();
		}
	}
	else
	{
		//// ������� ��������� �������
		//PwmDivider++;
		//if (PwmDivider >= PwmDividerTop)
		//{
			//PwmDivider = 0;
			//OCR0B++;
			//if (OCR0B > OCR0A)
			//{
				//if (TimerMode == GROW)
				//{
					////SET_BIT(PORTB, PORTB1); // �������� �������� ���������
					//LoadState = 1;
				//}
				//else
				//{
					////CLEAR_BIT(PORTB, PORTB1); // ��������� �������� ���������
					//LoadState = 0;
				//}
				//ConfigureTimer(NORMAL);
			//}
		//}
	}
}

// ----------------------------------------------------------------------------
/* ���������� �� ������ ������� */
ISR (PCINT0_vect)
{
	if (SignalMode == SHUTDOWN) return;
	
	// -- ����������� �������� ��������� �����: ���� ��� ������
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
	
    // �������� ������� ���������� ���� ���������: ���� ������������ �����/������ � �������� ���������
    if ( (ticks >= Prg[(PrgStep<<1) + 0]) && (ticks <= Prg[(PrgStep<<1) + 1]) )
    {
	    PrgStep++; // ��������� ��� ���������

	    if (PrgStep >= PrgMaxSteps)
	    {
		    // ��������� ��������� - �������� ��������: ������ ��������� ����������
		    // � ���������� ��� ��������� � ������
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
		// �������!!!!
		//DbgSendStat();
		
	    // �������� ������ ���� ��������� �� ��������� � ���������
	    // ���������� ��� ��������� � ������
		ResetProgram();
    }	
	
	// -- ���������� �������
	ticks = 0;
	SignalMode = NONE;
}

// -----------------------------------------------------------------------------
/* ���������� ��������� ���-�� ���� ������, LSB first -> */
/*
��������:
150�� - ������ ������
50�� -  �������� "1"
25�� -  �������� "0"
100�� - ����� ������
����� ("0") ����� ���������� 10��

200�� - ������ ������������������
*/
void DbgSend(uint16_t data, uint8_t bits)
{
	// ������ ������
	SET_BIT(PORTB, PORTB4);
	delay5(15/5);
	
	CLEAR_BIT(PORTB, PORTB4);
	delay5(10/5);
	
	// ������
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
	
	// ����� ������
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