#include "stm32f030x6.h"
#include "stm32f0xx.h"
#include "images.h"
#include "main.h"

//------ ���������� ------------------------------------------------------------
volatile struct
{
  uint8_t TimeChanged : 1; //��������� ����, ��� ����� ���������� �� 1 �������
  uint8_t DateChanged : 1; //��������� ����, ��� ���� ����������
  uint8_t isTouched : 1; //���������� ���� �� ������� � ������� ������
  uint8_t needRepeat: 1; //���������� ��� �� ����� ��������� ������� while (TOUCH_PRESSED)
  uint8_t menuIsShowed : 1; //������ ������ ��������� �������\���� ��������
  uint8_t systemState; //���� ��������� �������
} flags;
/*systemState:
  0: ����������� �������
  1: �������������� ����� (�H)
  2: �������������� ����� (MM)
  3: �������������� ���� (YYYY)
  4: �������������� ���� (MM)
  5: �������������� ���� (DD)
  6: ��������� ����
  7: �������
  8: ��������� - ��������� ����� (��)
  9: ��������� - ��������� ����� (��)
  10: ��������� - ��������� ���� ������
*/

struct
{
  uint8_t i; //��������� ���������� ��� ������ � �.�.
  uint8_t j; //��������� ���������� ��� ������ � �.�.
  volatile uint32_t DelayCounter; //��� SysTick
  
  volatile uint8_t bStatus; //�����. ������. 0-��������, 1-������. ��������������� �������������.
  volatile uint16_t bCycle; //�����. ���������� ���� ������
  volatile uint8_t bCount; //�����. ���������� ���������� �������. �������� �������������.
    
  uint32_t mCalendarTime;
  uint32_t mCalendarDate;
  uint32_t mLastCalendar;
  uint32_t mCalendarAlarmTime; //����� ����������
  uint32_t  mCalendarAlarmDays[7]; //��� ����������
  uint8_t  mDoW_Checked[7]; //��������� ������ ��������� ����
  
  uint16_t mYear;
  uint8_t  mMonth;
  uint8_t  mDay;
  
  uint8_t tXX[2]; //������ X ��� ��������� touch screen 
  uint8_t tYY[2]; //������ Y ��� ��������� touch screen
  uint8_t tObjectID; //��� �������� �������
  
  uint8_t digitsOffset;
  
  uint8_t arrBmpHeader16[8]; //����� ��� �������� bmp header 
  
  
} v;

      
int main()
{
  //--- ��������� MCU �� �������� ������� 48��� ---
  RCC->CFGR =RCC->CFGR & (~RCC_CFGR_PLLMUL) | (RCC_CFGR_PLLMUL12); //������ ��������� PLL x12 (48MHz)
  RCC->CR |=RCC_CR_PLLON; //�������� PLL
  while((RCC->CR &RCC_CR_PLLRDY) == 0); //��������� ��������� PLL
  
  RCC->CFGR |= (uint32_t) (RCC_CFGR_SW_PLL); //������� PLL ���������� SYSCLK
  while ((RCC->CFGR &RCC_CFGR_SWS) !=RCC_CFGR_SWS_PLL); //�������� ���� PLL �� ��������� ��� �������� SYSCLK
  //---
  
  //--- �������� ������������ ��������� 
  RCC->AHBENR |= (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN); //�������� ������������ GPIO PORT A, B
  RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN); //�������� ������������ SPI1
  RCC->APB1ENR |= RCC_APB1ENR_PWREN; //�������� ������������ ���������� power
  for (v.i=0; v.i<100; v.i++); //��������� ���� ��� �� �������� ������������
  //---  
  
  //--- �������� RTC
  PWR->CR |= PWR_CR_DBP; //�������� ������ � �������� RCC->BDCR
    RCC->CSR |= RCC_CSR_LSION; //�������� ���������� �������� ��������� 40���
    while((RCC->CSR &RCC_CSR_LSIRDY) == 0); //��������� ��������� LSI 
    //RCC->BDCR |= RCC_BDCR_LSEON; //�������� ������� �������� ��������� 32768 ��
    //while((RCC->BDCR &RCC_BDCR_LSERDY) == 0); //��������� ��������� LSE
    RCC->BDCR |= RCC_BDCR_RTCSEL_LSI | RCC_BDCR_RTCEN; //LSE source, RTC on
    //RCC->BDCR |= RCC_BDCR_RTCSEL_LSE | RCC_BDCR_RTCEN; //LSE source, RTC on
  //---

  //--- ����������� ����
  GPIOA->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | 
                   GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | 
                   GPIO_MODER_MODER7_1 | GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0);
  GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR0 | GPIO_OSPEEDR_OSPEEDR1 | GPIO_OSPEEDR_OSPEEDR2 |
                     GPIO_OSPEEDR_OSPEEDR3 | GPIO_OSPEEDR_OSPEEDR4 | GPIO_OSPEEDR_OSPEEDR5 |
                     GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR7 | GPIO_OSPEEDR_OSPEEDR9 |
                     GPIO_OSPEEDR_OSPEEDR10);
  GPIOB->MODER |= GPIO_MODER_MODER1_0;
  //GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR1;
  
  //--- ������������� SPI
  SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE; //Master mode, Fpclk/2 = 48/2, SPI on
  SPI1->CR2 |= SPI_CR2_FRXTH; //8bit FIFO buffer, 8bit data size, NSS pin disabled, no interrupts
  
  //--- ��������� SysTick (1�������)
  v.bStatus = 0;
  SysTick->LOAD = (48000UL & SysTick_LOAD_RELOAD_Msk) - 1;
  SysTick->VAL = 0; //���������� ������� ��� ���������
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | //�������� ������������ SYSCLK
                   SysTick_CTRL_TICKINT_Msk   | //����������� ���������� �� ���������� ����
                   SysTick_CTRL_ENABLE_Msk;     //�������� ������� 
  
  //--- ��������� �������� � ���������� ---
  EXTI->IMR |= EXTI_IMR_IM17; //17�� ������� ���������� ��� Alarm A
  EXTI->RTSR |= EXTI_RTSR_RT17;
  NVIC_EnableIRQ(RTC_IRQn);
  NVIC_SetPriority(SysTick_IRQn, 1); //���� ��������� ������ � �������� � ���
  __enable_irq(); //�������� ��� ����������
  
  MEM_CS(CS_DISABLE);
  MEM_RESUME;
  LCD_CS(CS_DISABLE);  
  
  //�������������� ���������
  //S(tart).AAA(ddress).M(ode).S(er/dfr).PP(ower)
  //Start= 1
  //Address= 101(X), 001(Y)
  //Mode= 1(8-bits), 0(12-bits)
  //Ser/dfr= 0(difference), 1(single-ended)
  //Power= 00(IRQ on)
  SPI1->CR1 |= SPI_CR1_BR_2; //XPT2046 F_CLK max = 2,5Mhz!!!!
  TOUCH_CS(CS_ENABLE);
    SPI_Exchange(0xD8); //������������� ��� ������������� PEN_IRQ
    SPI_Exchange(0x00);
    SPI_Exchange(0x00);   
  TOUCH_CS(CS_DISABLE);
  SPI1->CR1 ^= SPI_CR1_BR_2;
  //
  LCD_Init();
  
  //���������� ����� �������� �������� � ��� ��� ���������� ����������
  EXTI->IMR |= EXTI_IMR_MR10; //line 10 (PA10)
  EXTI->FTSR |= EXTI_FTSR_TR10; //falling edge
  NVIC_EnableIRQ(EXTI4_15_IRQn);
  NVIC_SetPriority(EXTI4_15_IRQn, 0);
  
  
  //��������� ��������
  BEEP(0);
  flags.TimeChanged = false;
  flags.DateChanged = false;
  flags.isTouched = false;
  flags.menuIsShowed = false;
  flags.systemState = (uint8_t)0;
  BmpHeader16 = (TBmpHeader16*)&v.arrBmpHeader16;
  LoadFromRom(); //�������� digitsOffset, mCalendarAlarmTime, mCalendarAlarmDays
  
  //--- ��������� RTC ---
  Time_SetCalendarTM(0x00000000, 0x00000000);
  
  //������ ���
  LCD_CS(CS_ENABLE);
  LCD_SetWindow(0,319, 0,239);
  LCD_FillBackground(BackGroundColor);//(0x4228);
  
  //����� ��������
  Time_Show(1, 1, (TS_HH | TS_MM));
  Date_Show();
  
  while (true)
  {
    ProcessTouching();
    
    //�������� � ����������� �� ����� ��������� �������
    switch (flags.systemState)
    {
      //���� ������
      case 0: 
      case 6:
        v.j = ~v.j;
        if (v.j) BEEP(1); else BEEP(0);
        Delay(250);
        //������� ������� �����
        Time_Show(0, 1, (TS_HH | TS_MM));  
        //��� ������� �� ����� ���� - ������� ���������� �� ������� 
        if (flags.DateChanged) 
        {   
          LCD_FillRectangle(0, 319, 0, base_digits_y - 1, BackGroundColor); //����
          LCD_FillRectangle(0,319, 190,239, BackGroundColor); //���        
          Date_Show();
          flags.DateChanged = false;
        }
        //�������� ������������ ����������
        if (v.mCalendarAlarmTime != 0)
          if ((v.mCalendarTime & 0x003F7F00) == v.mCalendarAlarmTime)
          { //�������� v.mCalendarTime �������� ������� ����� ����� ���������� ������� Time_Show
            uint8_t isDayEqual = false;
            for (uint8_t i=0; i<7; i++)
            {
              if ((v.mCalendarDate & RTC_DR_WDU_Msk) == v.mCalendarAlarmDays[i])
              {
                isDayEqual = true;
                break;
              }
            }
            if ((v.bStatus == 0) && isDayEqual)
            {
              v.bCount = 3;
              v.bStatus = 1; //�������� ���������
            }
          }
        break;
        
      default:
        break;
    } //switch  
    
    //���������� ������� �� �������, ���� ����
    if (v.tObjectID != 0)
    {
      //�������� ��� �� ������ �������
      if (flags.systemState == 7)
      {
        LCD_FillRectangle(0, 319, 0, 239, BackGroundColor);
        Time_Show(1, 1, (TS_HH | TS_MM));  
        Date_Show();
        flags.systemState = (uint8_t)0;
        v.tObjectID = (uint8_t)0;
        while (TOUCH_PRESSED) {Delay(100);}
        continue;
      }
            
      //���� ������� �� ���� �� ����
      if ((v.tObjectID & 64) == 64)
      {
        //���� ������ �� �������� - �������
        if (!flags.menuIsShowed)
        {
          //������ ������� ����������
          LCD_FillRectangle(0, 319, 0, base_digits_y - 1, BackGroundColor);
          //������ �������
          if (v.digitsOffset == 0)
            LCD_FillRectangle(base_digits_x + 255, base_digits_x + 255 + 15, base_digits_y + 106, base_digits_y + 106 + 6, BackGroundColor);
          else
            LCD_FillRectangle(152, 152+15, 125, 125 + 7, BackGroundColor);
            
          //������� ������
          LCD_ShowImage16FromMem(0, 190, memMap[11]);
          
          //��������� �����
          flags.menuIsShowed = true; 
          flags.systemState = v.tObjectID - 64;
          
          //������� ����� �� RTC
          while((RTC->ISR &RTC_ISR_RSF) != RTC_ISR_RSF);
          v.mCalendarTime = RTC->TR;
          v.mCalendarDate = RTC->DR;
          
          //������� ���� ��� ������
          if (flags.systemState == 1)
          {
            LCD_ShowText(52, 3+0, (uint8_t*)&TMessages[0], 2, LCD_YELLOW, BackGroundColor); //"���������"
            LCD_ShowText(100, 3+30, (uint8_t*)&TMessages[10], 2, LCD_YELLOW, BackGroundColor); //"�����"
            if (v.digitsOffset == 0)
              LCD_FillRectangle(base_digits_x + 135, base_digits_x + 254, base_digits_y, base_digits_y + 114, BackGroundColor);
            else
              LCD_FillRectangle(base_fdigits_x + 166, base_fdigits_x + 119 + 166, base_digits_y, base_digits_y + 114, BackGroundColor);
          }
          
          if (flags.systemState == 2)
          {
            LCD_ShowText(52, 3+0, (uint8_t*)&TMessages[0], 2, LCD_YELLOW, BackGroundColor); //"���������"
            LCD_ShowText(100, 3+30, (uint8_t*)&TMessages[16], 2, LCD_YELLOW, BackGroundColor); //"�����"
            if (v.digitsOffset == 0)
              LCD_FillRectangle(base_digits_x + 0, base_digits_x + 119, base_digits_y, base_digits_y + 114, BackGroundColor);
            else
              LCD_FillRectangle(base_fdigits_x, base_fdigits_x + 119, base_digits_y, base_digits_y + 114, BackGroundColor);
          }
        }
        v.tObjectID = (uint8_t)0;
      }
      
      //���� ������� �� ���� �� ������ ��� ������ ����� ������
      if ((v.tObjectID & 128) == 128)
      {
        if (flags.menuIsShowed)
        {      
          flags.needRepeat = true;
          //��������� ������
          switch (v.tObjectID)
          {
            //������� "+" ******************************************************
            case 0x81:
              //�������� ������
              if (flags.systemState == 2 || flags.systemState == 9) //���� ��������� ����� ������� ��� ����������
              {
                v.mCalendarTime = Time_Add(v.mCalendarTime, 0,1);
                Time_Show(1, 0, TS_MM);
              }
              //�������� ���
              if (flags.systemState == 1 || flags.systemState == 8) //���� ��������� ����� ������� ��� ����������
              {
                v.mCalendarTime = Time_Add(v.mCalendarTime, 1,0);
                Time_Show(1, 0, TS_HH);
              }
              //�������� ���
              if (flags.systemState == 3)
              {
                v.mYear++;
                if (v.mYear > 9999) v.mYear = 2000;
                LCD_ShowDigits(40, base_digits_y, v.mYear, 4);
              }
              //�������� �����
              if (flags.systemState == 4)
              {
                v.mMonth++;
                if (v.mMonth > 12) v.mMonth = 12;
                LCD_ShowDigits(100, base_digits_y, v.mMonth, 2);
              }   
              //�������� ����
              if (flags.systemState == 5)
              {
                v.mDay++;
                if ((v.mMonth & 0x01U) == 0x01U) 
                {//�������� ������
                  if (v.mDay > 31) v.mDay = 31;
                }
                else
                {//������ ������
                  if (v.mMonth == 2)
                  {//���� �������
                    if ((v.mYear & 3) == 0 && ((v.mYear % 25) != 0 || (v.mYear & 15) == 0))
                    {//���������� ���
                      if (v.mDay > 29) v.mDay = 29;
                    }
                    else
                    {//������� ���
                      if (v.mDay > 28) v.mDay = 28;
                    }
                  }
                  else
                  {//���� �� �������
                    if (v.mDay > 30) v.mDay = 30;
                  }
                }
                LCD_ShowDigits(100, base_digits_y, v.mDay, 2);
              }              
              //���������� ���������
              if (flags.systemState == 6)
              {
                //�������� �������� ����������
                v.mCalendarTime = v.mCalendarAlarmTime;
                //������� ���� ���������� MM:xx
                Time_Show(1, 0, TS_HH);
                //������� �������
                LCD_FillRectangle(0, 319, 0, base_digits_y - 1, BackGroundColor);
                LCD_ShowText(52, 3+0, (uint8_t*)&TMessages[0], 2, LCD_YELLOW, BackGroundColor); //"���������"
                LCD_ShowText(40, 3+30, (uint8_t*)&TMessages[38], 2, LCD_YELLOW, BackGroundColor); //"����������"
                //������ ������
                if (v.digitsOffset == 0)
                  LCD_FillRectangle(base_digits_x + 135, base_digits_x + 254, base_digits_y, base_digits_y + 114, BackGroundColor);
                else
                  LCD_FillRectangle(base_fdigits_x + 166, base_fdigits_x + 119 + 166, base_digits_y, base_digits_y + 114, BackGroundColor);  
                //������ �������
                if (v.digitsOffset == 0)
                  LCD_FillRectangle(base_digits_x + 255, base_digits_x + 255 + 15, base_digits_y + 106, base_digits_y + 106 + 6, BackGroundColor);
                else
                  LCD_FillRectangle(152, 152+15, 125, 125 + 7, BackGroundColor);
                //������� ����
                LCD_ShowImage16FromMem(0, 190, memMap[11]);
                //��������� �����
                flags.systemState = 8;
                flags.menuIsShowed = true;
                v.tObjectID = (uint8_t)0;
                flags.needRepeat = false;
              }
              break;
              
            //������� "-" ��� "����� ������" ***********************************
            case 0x82:
              //������ ������
              if (flags.systemState == 2 || flags.systemState == 9) //���� ��������� ����� ������� ��� ����������
              {
                v.mCalendarTime = Time_Sub(v.mCalendarTime, 0,1);
                Time_Show(1, 0, TS_MM);
              }
              //������ ���
              if (flags.systemState == 1 || flags.systemState == 8) //���� ��������� ����� ������� ��� ����������
              {
                v.mCalendarTime = Time_Sub(v.mCalendarTime, 1,0);
                Time_Show(1, 0, TS_HH);
              }
              //������ ���
              if (flags.systemState == 3)
              {
                v.mYear--;
                if (v.mYear < 2000) v.mYear = 2000;
                LCD_ShowDigits(40, base_digits_y, v.mYear, 4);
              }
              //������ �����
              if (flags.systemState == 4)
              {
                v.mMonth--;
                if (v.mMonth < 1) v.mMonth = 1;
                LCD_ShowDigits(100, base_digits_y, v.mMonth, 2);
              } 
              //������ ����
              if (flags.systemState == 5)
              {
                v.mDay--;
                if (v.mDay < 1) v.mDay = 1;
                LCD_ShowDigits(100, base_digits_y, v.mDay, 2);
              } 
              //�������� �����
              if (flags.systemState == 6)
              {
                if (v.digitsOffset == 0)
                  v.digitsOffset = 13;
                else
                  v.digitsOffset = (uint8_t)0;
                //������� ����� � ����
                LCD_FillRectangle(0, 319, 0, 239, BackGroundColor);
                Time_Show(1, 1, (TS_HH | TS_MM));
                Date_Show();                
                //��������� �����
                flags.systemState = (uint8_t)0;
                flags.menuIsShowed = false;
                flags.needRepeat = false;
                //�������� ���������
                SaveToRom();
              }
              break;
                      
            //������� "��" ��� "�������" ***************************************
            case 0x83:
              flags.needRepeat = false;
              //�������� �������� �������
              if ((flags.systemState == 1) || (flags.systemState == 2))
              {
                Time_SetCalendarTM(v.mCalendarTime, 0);
                LCD_FillRectangle(0,319, 190,239, BackGroundColor);
                flags.systemState = (uint8_t)0;
                flags.menuIsShowed = false;
                LCD_FillRectangle(0, 319, 0, base_digits_y-1, BackGroundColor);
                Time_Show(1, 1, (TS_HH | TS_MM));  
                Date_Show();
                break;
              }
              //������� � �������������� ������
              if (flags.systemState == 3)
              {
                //������ ����� ������� ����������
                LCD_FillRectangle(0, 319, 3+30, base_digits_y - 1, BackGroundColor);
                //������� �������
                LCD_ShowText(88, 3+30, (uint8_t*)&TMessages[27], 2, LCD_YELLOW, BackGroundColor); //"������"
                //������� �����
                LCD_FillRectangle(0, 319, base_digits_y, base_digits_y + 114, BackGroundColor);
                LCD_ShowDigits(100, base_digits_y, v.mMonth, 2);                
                flags.systemState = 4;
                break;
              }
              //������� � �������������� ���
              if (flags.systemState == 4)
              {
                //������ ����� ������� ����������
                LCD_FillRectangle(0, 319, 3+30, base_digits_y - 1, BackGroundColor);
                //������� �������
                LCD_ShowText(124, 3+30, (uint8_t*)&TMessages[34], 2, LCD_YELLOW, BackGroundColor); //"���"
                //������� ����
                LCD_FillRectangle(0, 319, base_digits_y, base_digits_y + 114, BackGroundColor);
                LCD_ShowDigits(100, base_digits_y, v.mDay, 2);                
                flags.systemState = 5;
                break;
              }
              //������� � �������������� ����� ����������
              if (flags.systemState == 8)
              {
                //������� ������ ����������
                Time_Show(1, 0, TS_MM);
                //������ ����
                if (v.digitsOffset == 0)
                  LCD_FillRectangle(base_digits_x + 0, base_digits_x + 119, base_digits_y, base_digits_y + 114, BackGroundColor);
                else
                  LCD_FillRectangle(base_fdigits_x, base_fdigits_x + 119, base_digits_y, base_digits_y + 114, BackGroundColor);
                //��������� �����
                flags.systemState = 9;
                break;
              }
              //������� � �������������� ���� ������ ����������
              if (flags.systemState == 9)
              {
                LCD_FillRectangle(0, 319, 0, 189, BackGroundColor);
                LCD_FillRectangle(0, 159, 190, 239, BackGroundColor);
                uint8_t j = 0;
                for (uint8_t i=0; i<4; i++)
                {
                  LCD_FillRectangle(0, 158, i*47, i*47+44, (v.digitsOffset == 0 ? LCD_CYAN : LCD_YELLOW));
                  LCD_ShowText(55, 8 + i*47, (uint8_t*)&TShortDAYS[j], 2, LCD_BLACK, (v.digitsOffset == 0 ? LCD_CYAN : LCD_YELLOW));
                    j += 3;
                  LCD_FillRectangle(161, 319, i*47, i*47+44, (v.digitsOffset == 0 ? (i == 3 ? LCD_YELLOW : LCD_CYAN) : (i == 3 ? LCD_CYAN : LCD_YELLOW)));
                    LCD_ShowText((i == 3 ? 180 : 216),
                                 8 + i*47, (uint8_t*)&TShortDAYS[j], 2, LCD_BLACK,
                                 (v.digitsOffset == 0 ? (i == 3 ? LCD_YELLOW : LCD_CYAN) : (i == 3 ? LCD_CYAN : LCD_YELLOW)));
                    j += 3;
                }
                //��������� ��������
                for (uint8_t i=0; i<7; i++)
                  v.mDoW_Checked[i] = (uint8_t)(v.mCalendarAlarmDays[i] >> RTC_DR_WDU_Pos);
                Alarm_ShowDowCheckboxes();
                //��������� �����
                flags.systemState = 10;
                break;
              }
              //��������� ����� �������� ����������
              if (flags.systemState == 10)
              {
                //�������� ��������
                v.mCalendarAlarmTime = v.mCalendarTime;
                for (uint8_t i=0; i<7; i++)
                  v.mCalendarAlarmDays[i] = (v.mDoW_Checked[i] << RTC_DR_WDU_Pos);
                //������� �����
                LCD_FillRectangle(0, 319, 0, 239, BackGroundColor);
                Time_Show(1, 1, (TS_HH | TS_MM));
                Date_Show();
                //��������� �����
                flags.menuIsShowed = false;
                flags.systemState = 0;
                //�������� ���������
                SaveToRom();
                break;                
              }
              
              //��������� ����� ����
              if (flags.systemState == 5)
              {
                //�������� ��������� ���� ������ ����������
                uint16_t c;
                c = ConvertToBCD(v.mYear);
                v.mCalendarDate = 0;
                v.mCalendarDate |= (c & 0x000FU) << RTC_DR_YU_Pos;
                v.mCalendarDate |= ((c & 0x00F0U) >> 4) << RTC_DR_YT_Pos;
                c = ConvertToBCD(v.mMonth);
                v.mCalendarDate |= (c & 0x000FU) << RTC_DR_MU_Pos;
                v.mCalendarDate |= ((c & 0x00F0U) >> 4) << RTC_DR_MT_Pos;    
                c = ConvertToBCD(v.mDay);
                v.mCalendarDate |= (c & 0x000FU) << RTC_DR_DU_Pos;
                v.mCalendarDate |= ((c & 0x00F0U) >> 4) << RTC_DR_DT_Pos;  
                c = Date_GetDayOfWeek(v.mDay, v.mMonth, v.mYear);
                v.mCalendarDate |= c << RTC_DR_WDU_Pos;
                //��������� �����
                Time_SetCalendarTM(0, v.mCalendarDate);
                //������� �����
                LCD_FillRectangle(0, 319, base_digits_y, base_digits_y + 114, BackGroundColor);
                Time_Show(1, 1, (TS_HH | TS_MM));
                //������� ���� � ������� ������
                LCD_FillRectangle(0, 319, 0, base_digits_y - 1, BackGroundColor); //����
                LCD_FillRectangle(0,319, 190,239, BackGroundColor); //���
                Date_Show();                
                //�������� �����
                flags.menuIsShowed = false;
                flags.systemState = 0;
                break;                
              }
              //������� �������
              if (flags.systemState == 6)
              {              
                LCD_ShowImage16FromMem(0, 0, memMap[24]);
                //��������� �����
                flags.systemState = 7;
                flags.menuIsShowed = false;
              }
              break;
              
            //������� "CANCEL" *************************************************
            case 0x84:
              LCD_FillRectangle(0,319, 0,239, BackGroundColor);
              //������� �����
              //LCD_FillRectangle(0, 319, base_digits_y, base_digits_y + 114, BackGroundColor);
              Time_Show(1, 1, (TS_HH | TS_MM));
              //������� ���� � ������� ������
              //LCD_FillRectangle(0, 319, 0, base_digits_y - 1, BackGroundColor); //����
              //LCD_FillRectangle(0,319, 190,239, BackGroundColor); //���
              Date_Show();
              //��������� �����
              flags.systemState = 0;
              flags.menuIsShowed = false;
              flags.needRepeat = false;
              break;
              
            default:
              break;
          } //switch
          //��������� ���� ������� �� ����� ������
          if (!flags.needRepeat)
            while (TOUCH_PRESSED) {Delay(100);}
          else
            Delay(100);
          v.tObjectID = 0; //����������
        }
        else
        {
          //�������� �� ��������� �� ���� �������
          if (flags.systemState == 0)
          {
            if (!flags.menuIsShowed)
            {
              //������� ����
              LCD_ShowImage16FromMem(0, 190, memMap[12]); //���������
              //��������� �����
              flags.systemState = 6;
              flags.menuIsShowed = true;
              v.tObjectID = 0;
              while (TOUCH_PRESSED) {Delay(100);}
              //continue;
            }
          }
        }
      }
      
      //���� ������� �� ������� ����
      if ((v.tObjectID & 32) == 32)
        if (flags.systemState == 0) //������� ���� � ��������� ����������� �������
        {
          //������� ����� �� RTC
          while((RTC->ISR &RTC_ISR_RSF) != RTC_ISR_RSF);
          v.mCalendarDate = RTC->DR;
          v.mYear = 2000 + 10*((v.mCalendarDate & RTC_DR_YT_Msk) >> RTC_DR_YT_Pos) + ((v.mCalendarDate & RTC_DR_YU_Msk) >> RTC_DR_YU_Pos);
          v.mMonth = 10*((v.mCalendarDate & RTC_DR_MT_Msk) >> RTC_DR_MT_Pos) + ((v.mCalendarDate & RTC_DR_MU_Msk) >> RTC_DR_MU_Pos);
          v.mDay = 10*((v.mCalendarDate & RTC_DR_DT_Msk) >> RTC_DR_DT_Pos) + ((v.mCalendarDate & RTC_DR_DU_Msk) >> RTC_DR_DU_Pos);
          //������ ������� ����������
          LCD_FillRectangle(0, 319, 0, base_digits_y - 1, BackGroundColor);
          //������� �������
          LCD_ShowText(52, 3+0, (uint8_t*)&TMessages[0], 2, LCD_YELLOW, BackGroundColor); //"���������"
          LCD_ShowText(112, 3+30, (uint8_t*)&TMessages[22], 2, LCD_YELLOW, BackGroundColor); //"����"
          //������� ���
          LCD_FillRectangle(0, 319, base_digits_y, base_digits_y + 114, BackGroundColor);
          LCD_ShowDigits(40, base_digits_y, v.mYear, 4);
          //������� ������
          LCD_ShowImage16FromMem(0, 190, memMap[11]);
          //��������� �����
          flags.systemState = 3; //��������� ����
          flags.menuIsShowed = true; //������ ����������
          v.tObjectID = 0; //����������
        }
      
      //��������� ��������� ������ ��� ��� ����������
      if ((flags.systemState == 10) && (v.tObjectID != 0))
      {
        //������� ������� � ������� (0-���� �� ������. 1..7 ���� ������ � ���������
        if (v.tObjectID != 8)
        {
          if (v.mDoW_Checked[v.tObjectID - 1])
            v.mDoW_Checked[v.tObjectID - 1] = (uint8_t)0;
          else
            v.mDoW_Checked[v.tObjectID - 1] = v.tObjectID;
        }
        else
        {
          for (uint8_t i=0; i<7; i++) v.mDoW_Checked[i] = (uint8_t)0;
        }
        //
        Alarm_ShowDowCheckboxes();
        //
        v.tObjectID = 0;
        while (TOUCH_PRESSED) {Delay(100);}
      }

    } //if (v.ObjectID != 0)
    
  } //while (true)
}

//=== NVIC ������� =============================================================
/* ������� ��� ������� Delay */
void SysTick_Handler()
{
  v.bCycle++;
  if (v.DelayCounter != 0) v.DelayCounter--;
  if (v.bStatus != 0)
  {
    switch (v.bStatus)
    {
      case 1:
      case 4:  
      case 7:
        BEEP(1);
        v.bCycle = (uint8_t)0;
        v.bStatus++;
        break;
        
      case 2:
      case 5:
        if (v.bCycle > 100)
        {
          BEEP(0);
          v.bStatus++;
          v.bCycle = (uint8_t)0;
        }
        break;
        
      case 3:
      case 6:
        if (v.bCycle > 100)
        {
          v.bStatus++;
        }
        break;
        
      case 8:
        if (v.bCycle > 150)
        {
          BEEP(0);
          v.bStatus++;
          v.bCycle = (uint8_t)0;
        }
        break;  
        
      case 9:
        if (v.bCycle > 500)
        {
          v.bStatus++;
        }
        break;        
        
      case 10:
        v.bCount--;
        if (v.bCount == 0)
          v.bStatus = (uint8_t)0; //stop beeping
        else
          v.bStatus = 1; //reload
        break;
        
      default:
        break;
  }
 }
}

//------------------------------------------------------------------------------
/* ������� Alarm ������� (�� ����������!) - +1 ������� */
void RTC_IRQHandler()
{
  if ((RTC->ISR & RTC_ISR_ALRAF) == RTC_ISR_ALRAF)
  {
    flags.TimeChanged = true;
    RTC->ISR &= ~RTC_ISR_ALRAF; //clear flag
    
  }
  EXTI->PR |= EXTI_PR_PIF17; //clear flag by writting 1
}

//------------------------------------------------------------------------------
/* ��������� ���������� �� ����� PA10 - ���������� ��������� ������� */
void EXTI4_15_IRQHandler()
{
  if (EXTI->PR & EXTI_PR_PR10) //event triggering
  {
    EXTI->PR |= EXTI_PR_PR10;//�� ������ ��� ��� ������ �� ����, �� ����� ������� ����
    //�������� RTC Alarm ��� �� �� ��� �� ������ ���� 
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53; //enable write
      RTC->CR &=~RTC_CR_ALRAE; //disable alarm A
    RTC->WPR = 0xFE; 
    RTC->WPR = 0x64; //disable write
    //��������� MCU � ����� StandBy
    SCB->SCR |= 4U; //SCB_SCR_SLEEPDEEP
    PWR->CR |= PWR_CR_PDDS;
    PWR->CSR &= ~PWR_CSR_WUF;
    __disable_irq();
    __WFI();
  }
}

//=== ������� ==================================================================
/* ����������� �������� � �� */
void Delay(uint32_t msTime)
{	
      v.DelayCounter = msTime;
      while (v.DelayCounter);
}

//------------------------------------------------------------------------------
/* ������������� ������� */
/* ��������� ���� PA10 ��� ���� ��� ������� �������� � ��� */
void LCD_Init()
{
  SPI_WAIT_TX_READY; //���� ���������� ����� ������� �� ������
  
  //����������������� PA10 ��� �����
  //GPIOA->MODER |= GPIO_MODER_MODER10_0;
  //GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR10;
  
  //hardware reset
  LCD_RESET(RST_HI);
  Delay(5);
  LCD_RESET(RST_LO); //�������� - ������
  Delay(20);
  LCD_RESET(RST_HI);
  Delay(150); 
  
  //����������������� PA10 ��� �������� ���� � ��������� � �����
  GPIOA->MODER &= ~GPIO_MODER_MODER10;	
  //GPIOA->PUPDR |= GPIO_PUPDR_PUPDR10_1;		

  
  
  MEM_CS(CS_DISABLE);
  LCD_CS(CS_ENABLE);
  
  //set up driver parameters
  uint32_t len, pos=0;
  for (v.i=0; v.i<19; v.i++) //������ �������
  {
    LCD_DC(LCD_CMD);
    SPI_Send(LcdInitData[pos++]);
    len = LcdInitData[pos++];
    
    SPI_WAIT_FOR_COMPLETION;
    LCD_DC(LCD_DATA);
    for (v.j=0; v.j<len; v.j++)
    {
      SPI_Send(LcdInitData[pos++]);
    }
  }
  
  Delay(120);
  SPI_WAIT_FOR_COMPLETION;
  LCD_DC(LCD_CMD);
  SPI_Send(0x29); //Display on
  SPI_Send(0x2C); //Write registers
  
  SPI_WAIT_FOR_COMPLETION;
  LCD_CS(CS_DISABLE);
}

//------------------------------------------------------------------------------
/* ��������� ��� ��������� ������
   ������� LCD_SetWindow(0,319, 0,239) ������ ���� ������� ������� */
void LCD_FillBackground(uint16_t color)
{
  uint32_t t;
  for (t=0; t<(240*320); t++)
  {
    SPI_Send((uint8_t)(color>>8));
    SPI_Send((uint8_t)(color));
  }
}

//------------------------------------------------------------------------------
/* ������������� ���� ���������. ����� ������ ������� ����� ���������� ������ �����
   ��� ������������ DX �������.
   x0, x1, y0, y1. �������� � x0 �� x1, � y0 �� y1. */
void LCD_SetWindow(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1)
{
  //���������� ������� ���� ���������
  SPI_WAIT_FOR_COMPLETION;
  LCD_DC(LCD_CMD);
  SPI_Send(0x2A);
  SPI_WAIT_FOR_COMPLETION;
  LCD_DC(LCD_DATA);
  SPI_Send((uint8_t)(x0>>8));
  SPI_Send((uint8_t)(x0)); //SC = column start
  SPI_Send((uint8_t)(x1>>8));
  SPI_Send((uint8_t)(x1)); //EC = column end  
  SPI_WAIT_FOR_COMPLETION;
  LCD_DC(LCD_CMD);
  SPI_Send(0x2B);
  SPI_WAIT_FOR_COMPLETION;
  LCD_DC(LCD_DATA);
  SPI_Send((uint8_t)(y0>>8));
  SPI_Send((uint8_t)(y0));
  SPI_Send((uint8_t)(y1>>8));
  SPI_Send((uint8_t)(y1));
  SPI_WAIT_FOR_COMPLETION;
  LCD_DC(LCD_CMD);
  SPI_Send(0x2C); //Write registers
  SPI_WAIT_FOR_COMPLETION;
  LCD_DC(LCD_DATA);
}

//------------------------------------------------------------------------------
/* ��������� ��������� ������� ��������� ������ */
void LCD_FillRectangle(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1, uint16_t color)
{
  //�������� ��� ��������
  while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY);
  
  LCD_CS(CS_ENABLE);

  LCD_SetWindow(x0, x1, y0, y1);
  for (uint32_t i=0; i<(2*((x1-x0)*(y1-y0))); i++)
  {
    SPI_Send((uint8_t)(color >> 8));
    SPI_Send((uint8_t)(color));
  }
  
  LCD_CS(CS_DISABLE);
}

//------------------------------------------------------------------------------
/* ���������� ���� ������ � ����� TXFIFO ���������� SPI */
void SPI_Send(uint8_t data)
{
  SPI_WAIT_TX_READY;
  *(uint8_t *)&(SPI1->DR) = data; //���������� � ����� 
}

//------------------------------------------------------------------------------
uint8_t SPI_Exchange(uint8_t data)
{
  //while ((SPI1->SR & SPI_SR_TXE) != SPI_SR_TXE); ��� ������, �� �������� � ������ �������������
  *(uint8_t *)&(SPI1->DR) = data;  //8bit data transfer!!!!
  while ((SPI1->SR & SPI_SR_RXNE) != SPI_SR_RXNE);
  return (uint8_t)SPI1->DR;
}

//------------------------------------------------------------------------------
/* ����� �������� �� MEM */
void LCD_ShowImage16FromMem(uint16_t x, uint8_t y, uint32_t address)
{
  uint8_t c;
  
  //��������� ����� ������ (1) � �������� ��� �������� (2)
  while ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE) c = SPI1->DR; //1
  while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY); //2
  
  //������� MEM
  LCD_CS(CS_DISABLE);
  MEM_CS(CS_ENABLE);
  MEM_RESUME;
  
  //�������: ������� ������
  SPI_Exchange(0x0B);
  SPI_Exchange((uint8_t)(address >> 16));
  SPI_Exchange((uint8_t)(address >> 8));
  SPI_Exchange((uint8_t)(address));
  SPI_Exchange(0x00);
  
  //�������� header
  for (uint8_t i=0; i<8; i++)
  {
    c = SPI_Exchange(0xFF);
    v.arrBmpHeader16[i] = c;
  }
  
  //��������� ���� ������ LCD
  MEM_PAUSE;
  LCD_CS(CS_ENABLE);
  LCD_SetWindow(x, x+BmpHeader16->width-1, y, y+BmpHeader16->height-1);
  LCD_CS(CS_DISABLE);
  MEM_RESUME;
  
  while ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE) c = SPI1->DR; //�������� Rx �����
  while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY);
  
  //----- ���������� ����� -----
  c = SPI_Exchange(0x00); //������� 1 ����
  LCD_CS(CS_ENABLE);
  
  for (uint32_t count=0; count < BmpHeader16->length; count++) //��������� ��� ������ ������ 255
  {
	  c = SPI_Exchange(c); //��������� ���������� � ������� ��������� ����	  
  }
  
  LCD_CS(CS_DISABLE);
  MEM_CS(CS_DISABLE);
}

//------------------------------------------------------------------------------
/* ����� �������� �� ROM */
/* font=1 (7x7) =2 (24x27) */
void LCD_ShowImage2FromRom(uint16_t x, uint8_t y, uint8_t imgIdx, uint8_t font, uint16_t fcolor, uint16_t bcolor)
{
  uint8_t c;
  
  //��������� ����� ������ (1) � �������� ��� �������� (2)
  while ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE) c = SPI1->DR; //1
  while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY); //2
  
  //��������� ���� ������ LCD
  LCD_CS(CS_ENABLE);
  if (font == 1) LCD_SetWindow(x, x+7, y, y+6);
  if (font == 2) LCD_SetWindow(x, x+23, y, y+26);
  if (font == 3) LCD_SetWindow(x, x+23, y, y+26);

  //����� �����������
  uint8_t rows;
  if (font == 1) rows = 7;
  if (font == 2) rows = 81;
  if (font == 3) 
  {
    rows = 81;
    fcolor = (L24X27FUN[83*imgIdx]<<8) + L24X27FUN[83*imgIdx + 1];
  }
  
  for (uint8_t row=0; row < rows; row++)
  {
    if (font == 1) c = digits8[7*imgIdx + row];
    if (font == 2) c = L24X27[81*imgIdx + row];
    if (font == 3) c = L24X27FUN[83*imgIdx + row + 2];
    
    if (font < 3)
    {
      for (uint8_t col=0; col<8; col++)
      { //������� ������� ������
        if (c & 0x80)
        {
          SPI_Exchange((uint8_t)(fcolor>>8));
          SPI_Exchange((uint8_t)fcolor);
        }
        else
        {
          SPI_Exchange((uint8_t)(bcolor>>8));
          SPI_Exchange((uint8_t)bcolor);
        }
        c <<= 1;
      }
    }
    else
    { //������� ��������� �������
      for (uint8_t col=0; col<8; col++)
      {
        if (c & 0x80)
        {
          SPI_Exchange((uint8_t)(fcolor>>8));
          SPI_Exchange((uint8_t)fcolor);
        }
        else
        {
          SPI_Exchange((uint8_t)(bcolor>>8));
          SPI_Exchange((uint8_t)bcolor);
        }
        c <<= 1;
      }      
    }
  }
  
  LCD_CS(CS_DISABLE);
}

//------------------------------------------------------------------------------
/* ���������� ����� � ���������� ������, ��� ���������� ���� ������, */
/* �������� ������ �� ��������� ���� */
/* ����� ����� ��������� �� ����� ������ ���� ��������� ����� ������ */
void LCD_ShowText(uint16_t x, uint8_t y, uint8_t* address, uint8_t font, uint16_t fcolor, uint16_t bcolor)
{
  uint8_t s;
  
  if (v.digitsOffset != 0) font = 3;
  
  while (*address != 0)
  {
    s = *address;
    if (s == 0x20)
    {
      s = 255; //������
    }
    else
    {
      if (s >= 0xC0U)
      {
        s = ABC[s - 0xC0U];  //�����
      }
      else
      {
          s = s - 0x30; //�����
      }
    }
    
    if (s != 255) 
      LCD_ShowImage2FromRom(x, y, s, font, fcolor, bcolor);
    else
      LCD_FillRectangle(x, x+23, y, y+27, bcolor); //������
    if ((font == 2) || (font == 3))
    {
      x += 24;
      if (x > (320-24))
      {
        x = 0;
        y += 30;
      }
    }
    else
    {
      if (font == 1)
      {
        x += 8;
        if (x > (320-8))
        {
          x = 0;
          y += 8;
        }        
      }
    }
    
    address++;
  }
}

//------------------------------------------------------------------------------
/* ���������� ����� �� ������ (����: �������� �����) */
/* value-����� � ���������� ����, digits-����� ������������ ��������*/
void LCD_ShowDigits(uint16_t x, uint8_t y, uint16_t value, uint8_t digits)
{
  value = ConvertToBCD(value);
  for (uint8_t i=digits; i>0; i--)
  {
    LCD_ShowImage16FromMem(x + 60*(i-1),
                           y,
                           memMap[v.digitsOffset + (value & 0x000FU)]); //Nxxx
    value >>= 4;
  }					   
}

//------------------------------------------------------------------------------
/* ����������� �������, updateAll ������������� ��� ������� ��������� */
/* ����� ������ �� ����� ������������ �������, ��������� ���������� v.TimeStored */
/* ���� realTime = true, �� �������� ������� �� RTC, ����� �� ���������� ���������� v.mCalendar*/
/* HHMM ������ ������ ���� updateAll=1. HHMM=1 ��������� ������ ����, =2 ������ ������, =3 ���*/
void Time_Show(bool updateAll, bool realTime, uint8_t HHMM)
{
        uint32_t difference;
        
        if (realTime)
        {
          while((RTC->ISR &RTC_ISR_RSF) != RTC_ISR_RSF);
          v.mCalendarTime = RTC->TR;
          v.mCalendarDate = RTC->DR;
        }
  
	if (updateAll)
	{
          if ((HHMM & TS_HH) == TS_HH)
          {
                LCD_ShowImage16FromMem((v.digitsOffset == 0 ? base_digits_x : base_fdigits_x),
							      base_digits_y,
							      memMap[v.digitsOffset + ((v.mCalendarTime >> 20) & 0x00000003)]); //Hx:xx
		LCD_ShowImage16FromMem((v.digitsOffset == 0 ? base_digits_x + 60 : base_fdigits_x + 60),
							      base_digits_y,
							      memMap[v.digitsOffset + ((v.mCalendarTime >> 16) & 0x0000000F)]); //xH:xx
          }
          
		LCD_ShowImage16FromMem((v.digitsOffset == 0 ? base_digits_x + 120 : base_fdigits_x + 125),
				       (v.digitsOffset == 0 ? base_digits_y + 27 : 110),
							      (v.digitsOffset == 0 ? memMap[10] : memMap[23])); //xx:xx
          if ((HHMM & TS_MM) == TS_MM)
          {
                LCD_ShowImage16FromMem((v.digitsOffset == 0 ? base_digits_x + 135 : base_fdigits_x + 166),
							      base_digits_y,
							      memMap[v.digitsOffset + ((v.mCalendarTime >> 12) & 0x00000007)]); //Mx:xx
		LCD_ShowImage16FromMem((v.digitsOffset == 0 ? base_digits_x + 195 : base_fdigits_x + 166 + 60),
							      base_digits_y,
							      memMap[v.digitsOffset + ((v.mCalendarTime >> 8) & 0x0000000F)]); //xM:xx
          }
          if (HHMM == (TS_HH | TS_MM))
              {
                if (v.digitsOffset == 0)
                {
                  LCD_ShowImage2FromRom(base_digits_x + 120 + 15 + 2*60,
                                                             base_digits_y + 106,
                                                             ((v.mCalendarTime >> 4) & 0x00000007), 1, LCD_CYAN, BackGroundColor); //xx:Sx
                  LCD_ShowImage2FromRom(base_digits_x + 120 + 15 + 2*60 + 8,
                                                             base_digits_y + 106,
                                                             ((v.mCalendarTime) & 0x0000000F), 1, LCD_CYAN, BackGroundColor); //xx:xS
                }
                else
                {
                  LCD_ShowImage2FromRom(152, 125,
                                        ((v.mCalendarTime >> 4) & 0x00000007), 1, LCD_YELLOW, BackGroundColor); //xx:Sx
                  LCD_ShowImage2FromRom(152 + 8, 125,
                                        ((v.mCalendarTime) & 0x0000000F), 1, LCD_YELLOW, BackGroundColor); //xx:xS                  
                }
              }
	}
	else
	{
		if (!flags.TimeChanged) return;
                difference = v.mCalendarTime ^ v.mLastCalendar;
		
		if ((difference & 0x00000070) != 0)
		{
                  if (v.digitsOffset == 0)
                  {
                    LCD_ShowImage2FromRom(base_digits_x + 120 + 15 + 2*60,
                                          base_digits_y + 106,
                                          ((v.mCalendarTime >> 4) & 0x00000007), 1, LCD_CYAN, BackGroundColor); //xx:Sx	
                  }
                  else
                  {
                    LCD_ShowImage2FromRom(152, 125,
                                          ((v.mCalendarTime >> 4) & 0x00000007), 1, LCD_YELLOW, BackGroundColor); //xx:Sx                    
                  }
		}
		if ((difference & 0x0000000F) != 0)
		{
                  if (v.digitsOffset == 0)
                  {
                    LCD_ShowImage2FromRom(base_digits_x + 120 + 15 + 2*60 + 8,
                                          base_digits_y + 106,
                                          ((v.mCalendarTime) & 0x0000000F), 1, LCD_CYAN, BackGroundColor); //xx:xS
                  }
                  else
                  {
                    LCD_ShowImage2FromRom(152 + 8, 125,
                                          ((v.mCalendarTime) & 0x0000000F), 1, LCD_YELLOW, BackGroundColor); //xx:xS                      
                  }
		}                
		if ((difference & 0x00300000) != 0)
		{
                  LCD_ShowImage16FromMem((v.digitsOffset == 0 ? base_digits_x : base_fdigits_x),
                                                                base_digits_y,
                                                                memMap[v.digitsOffset + ((v.mCalendarTime >> 20) & 0x00000003)]); //Hx:xx	
                  if (difference == 0x00235959) flags.DateChanged = true; //��� ������� �� ����� ����, � ��������� ���������� ������ ������� ���� �� �������
		}
		if ((difference & 0x000F0000) != 0)
		{
                  LCD_ShowImage16FromMem((v.digitsOffset == 0 ? base_digits_x + 60 : base_fdigits_x + 60),
                                                                base_digits_y,
                                                                memMap[v.digitsOffset + ((v.mCalendarTime >> 16) & 0x0000000F)]); //xH:xx	 
                }
		if ((difference & 0x00007000) != 0)
		{
                  LCD_ShowImage16FromMem((v.digitsOffset == 0 ? base_digits_x + 135 : base_fdigits_x + 166),
                                                                base_digits_y,
                                                                memMap[v.digitsOffset + ((v.mCalendarTime >> 12) & 0x00000007)]); //Mx:xx			
		}
		if ((difference & 0x00000F00) != 0)
		{
                  LCD_ShowImage16FromMem((v.digitsOffset == 0 ? base_digits_x + 195 : base_fdigits_x + 166 + 60),
                                                                base_digits_y,
                                                                memMap[v.digitsOffset + ((v.mCalendarTime >> 8) & 0x0000000F)]); //xM:xx	
		}
	}
        
        if (realTime) v.mLastCalendar = v.mCalendarTime;
        flags.TimeChanged = false;
}

//-------------------------------------------------------------------------------------------------
/* ��������� ����� � ���������� */ 
uint32_t Time_Add(uint32_t time, uint8_t HH, uint8_t MM)
{
  time &= ~0x7F; //������� �������
  
  if (MM != 0)
  {
    time  += (0x01U << RTC_TR_MNU_Pos);
    if ((time & RTC_TR_MNU_Msk) == (0x0AU << RTC_TR_MNU_Pos))
        time = (time & ~RTC_TR_MNU_Msk) + (0x01U << RTC_TR_MNT_Pos);
    if ((time & RTC_TR_MNT_Msk) == (0x06U << RTC_TR_MNT_Pos))
        time &= ~RTC_TR_MNT_Msk;
  }
  else
  {
    time  += (0x01U << RTC_TR_HU_Pos);
    if ((time & (RTC_TR_HU_Msk | RTC_TR_HT_Msk)) == ((0x02U << RTC_TR_HT_Pos) | (0x04U << RTC_TR_HU_Pos)))
    {
      time &= ~(RTC_TR_HU_Msk | RTC_TR_HT_Msk);
    }
    else
    {
      if ((time & RTC_TR_HU_Msk) == (0x0AU << RTC_TR_HU_Pos))
        time = (time & ~RTC_TR_HU_Msk) + (0x01U << RTC_TR_HT_Pos);
    }
  }
  
  return time;
}

//-------------------------------------------------------------------------------------------------
/* �������� ����� �� ���������� */ 
uint32_t Time_Sub(uint32_t time, uint8_t HH, uint8_t MM)
{
  time &= ~0x7F; //������� �������
  
  if (MM != 0)
  {
    time  -= (0x01U << RTC_TR_MNU_Pos);
    if ((time & RTC_TR_MNU_Msk) == (0x0FU << RTC_TR_MNU_Pos))
    {
      time  += (0x01U << RTC_TR_MNU_Pos);
      time = ((time & ~RTC_TR_MNU_Msk) | (0x09U << RTC_TR_MNU_Pos));
      if ((time & RTC_TR_MNT_Msk) != 0x00000000U)
        time -= (0x01U << RTC_TR_MNT_Pos);
      else
        time = ((time & ~RTC_TR_MNT_Msk) | (0x05U << RTC_TR_MNT_Pos));
    }
  }
  else
  {
    time  -= (0x01U << RTC_TR_HU_Pos);
    if ((time & RTC_TR_HU_Msk) == (0x0FU << RTC_TR_HU_Pos))
    {
      time  += (0x01U << RTC_TR_HU_Pos);
      if ((time & (RTC_TR_HU_Msk | RTC_TR_HT_Msk)) == 0x00000000)
      {
        time &= ~(RTC_TR_HU_Msk | RTC_TR_HT_Msk);
        time |= (0x02U << RTC_TR_HT_Pos) | (0x03U << RTC_TR_HU_Pos);
      }
      else
      {
        time &=  ~RTC_TR_HU_Msk;
        time |= 0x09U << RTC_TR_HU_Pos;
        time -= 0x01U << RTC_TR_HT_Pos;
      }
    }
  }
  
  return time;
}

//-------------------------------------------------------------------------------------------------
/* ������������� ����� ����� �/��� ���� */
/* ���� ���� �� �������� = 0, ��� �� ��������������� */
void Time_SetCalendarTM(uint32_t newTime, uint32_t newDate)
{
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53; //enable write
  RTC->ISR |=RTC_ISR_INIT; //enable init phase, stop calendar ticking
  while ((RTC->ISR &RTC_ISR_INITF) != RTC_ISR_INITF); //wait while it is possible to modify
  RTC->PRER = 0x007F0137; //1 Hz, �������� ��� LSI
  if (newTime != 0) RTC->TR = newTime;
  if (newDate != 0) RTC->DR = newDate;
  RTC->ISR &=~RTC_ISR_INIT; //exit init phase, run calendar with new values
    RTC->CR &=~RTC_CR_ALRAE; //disable alarm A to modify it
    while ((RTC->ISR & RTC_ISR_ALRAWF) != RTC_ISR_ALRAWF); //wait until it can be possible to modify alarm A
    RTC->ALRMAR = RTC_ALRMAR_MSK1 | RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK4 | 0x00000059;
    RTC->CR |= (RTC_CR_ALRAE | RTC_CR_ALRAIE); //alarm A enable, alarm A interrupt enable
  RTC->WPR = 0xFE; 
  RTC->WPR = 0x64; //disable write 
}

//-------------------------------------------------------------------------------------------------
/* ���������� ���� */
/* ���� ���������� ��������� - �� ���������� ��� */
void Date_Show()
{
  uint8_t value;
  uint16_t x;
  uint8_t t;
  
  while((RTC->ISR &RTC_ISR_RSF) != RTC_ISR_RSF);
  v.mCalendarTime = RTC->TR;
  v.mCalendarDate = RTC->DR; //���� ������ ���� ������� ������ ����� ���������� ������� (� �� ���� ������, �� ������ ��� ��������)
  
  //������� ���� ������ �� ������
  value = (v.mCalendarDate & RTC_DR_WDU_Msk) >> RTC_DR_WDU_Pos;
  if (value < 6)
  {
    LCD_ShowText((320-24*CharCount((uint8_t*)&TDAYS[TDaysIDx[value]])) / 2, 200, (uint8_t*)&TDAYS[TDaysIDx[value]], 2, 0x87F0, BackGroundColor);
  }
  else
  {
    LCD_ShowText((320-24*CharCount((uint8_t*)&TDAYS[TDaysIDx[value]])) / 2, 200, (uint8_t*)&TDAYS[TDaysIDx[value]], 2, 0xFC10, BackGroundColor);
  }
  
  //��������� ������ ������ ������
  t = 10*((v.mCalendarDate & RTC_DR_MT_Msk) >> RTC_DR_MT_Pos);
  t += (v.mCalendarDate & RTC_DR_MU_Msk) >> RTC_DR_MU_Pos;
  x = (320 - ((24*CharCount((uint8_t*)&TMONTHS[TMonths_IDx[t]])) + (2*24 + 0*24 + 1*10))) / 2;
  
#define base_date_y 10
  //������� ����
  value = (v.mCalendarDate & RTC_DR_DT_Msk) >> RTC_DR_DT_Pos;
  LCD_ShowImage2FromRom(x, base_date_y, value, (v.digitsOffset == 0 ? 2 : 3), LCD_YELLOW, BackGroundColor);
  x += 24;
  value = (v.mCalendarDate & RTC_DR_DU_Msk) >> RTC_DR_DU_Pos;
  LCD_ShowImage2FromRom(x, base_date_y, value, (v.digitsOffset == 0 ? 2 : 3), LCD_YELLOW, BackGroundColor);
  x += (24 + 10);
  
  //������� �����
  LCD_ShowText(x, base_date_y, (uint8_t*)&TMONTHS[TMonths_IDx[t]], 2, 0xC7FF, BackGroundColor);
  x += 24*CharCount((uint8_t*)&TMONTHS[TMonths_IDx[t]]);
  x += 10;
  
  //������� ������ ����������, ���� ����������
  if (v.mCalendarAlarmTime != 0)
  {
    uint32_t temp = 0;
    for (uint8_t i=0; i<7; i++) temp |= v.mCalendarAlarmDays[i];
    if (temp != 0)
      LCD_ShowImage16FromMem(319-24, 0, memMap[25]);
  }
}

//-------------------------------------------------------------------------------------------------
/* ���� ���� ������ �� ���� (� ��=1 �� ��=7) */
uint8_t Date_GetDayOfWeek(uint8_t D, uint8_t M, uint16_t Y)
{
    uint16_t a, y, m, R;
    a = ( 14 - M ) / 12;
    y = Y - a;
    m = M + 12 * a - 2;
    R = 7000 + ( D + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12 );
    R = R % 7;
    if (R == 0) R = 7;
    return R;
}

//-------------------------------------------------------------------------------------------------
/* ���������� �������� ������ ��� ���������� */
void Alarm_ShowDowCheckboxes()
{
  uint8_t x, y;
  for (uint8_t i=1; i<8; i++)
  {
    //�������� ����� ����������� ��������
    if (i & 1U) //��������
    {
      x = 55 - (24*2);
      y = 8 + (i >> 1)*47;
    }
    else //������
    {
      x = 216 - (24*2);
      y = 8 + ((i >> 1) - 1)*47;
    }
    //���������
    if (v.mDoW_Checked[i-1] == 0)
      LCD_ShowText(x, y, " \0", 2, 0x0680, (v.digitsOffset == 0 ? LCD_CYAN : LCD_YELLOW));
    else
      LCD_ShowImage2FromRom(x, y, 33, 2, 0x0680, (v.digitsOffset == 0 ? LCD_CYAN : LCD_YELLOW));
  }
}

//-------------------------------------------------------------------------------------------------
/* ����������� ����������� ����� � BCD. ������������ �������� 9999 */
uint16_t ConvertToBCD(uint16_t data)
{
  uint16_t ret;
  ret = (data / 1000) << 12;
  data = data % 1000;
  ret |= (data / 100) << 8;
  data = data % 100;
  ret |= (data / 10) << 4;
  data = data % 10;
  ret |= data;
  return ret;
}
        
//-------------------------------------------------------------------------------------------------
/* ���������� � ��������� ������� �������� � ���������� */
/* 1000.xxxx -- ������ ����� */
/* 0100.xxxx -- ������ ����� */
/* 0010.xxxx -- ������ ����� */
/* 0000.xxxx -- ���������� ���� ������ ���������� */
/* xxxx - ��� ������� ������ */
void ProcessTouching()
{
    //�������� ��� �� ������� �� �����
    if (TOUCH_PRESSED && (!flags.isTouched))
    {
      //��������� ������ SPI Rx Tx
      while ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE) v.i = SPI1->DR;
      while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY);
      
      //������� ������� �� ���������� ������� ������ � TouchScr
      SPI1->CR1 |= SPI_CR1_BR_2; //set 1,5MHz [100(48/32=1,5MHz) 000(48/2=24MHz)]       
      TOUCH_CS(CS_ENABLE);
   
      v.tXX[0] = 0xFF;
      
      do {
        v.tXX[1] = v.tXX[0];
        SPI_Exchange(0x9B); //read X
        v.tXX[0] =  (SPI_Exchange(0x00) << 1);
        v.tXX[0] |= ((SPI_Exchange(0x00) & 0x80) >> 7);
      } while (v.tXX[1] != v.tXX[0]);
      
      v.tYY[0] = 0xFF;
      
      do {
        v.tYY[1] = v.tYY[0];
        SPI_Exchange(0xDB); //read X
        v.tYY[0] =  (SPI_Exchange(0x00) << 1);
        v.tYY[0] |= ((SPI_Exchange(0x00) & 0x80) >> 7);
      } while (v.tYY[1] != v.tYY[0]);
      
      if ((v.tXX[0] != 0xFF) && (v.tYY[0] != 0xFF)) flags.isTouched = true;
        
      SPI_Exchange(0xD8);
      SPI_Exchange(0x00);
      SPI_Exchange(0x00);
      
      //������� ������� SPI ������� � ����������� �� TouchScr
      SPI1->CR1 ^= SPI_CR1_BR_2; //set 24Mhz
      TOUCH_CS(CS_DISABLE); 
    } //if (TOUCH_PRESSED)
    
    if (!TOUCH_PRESSED)
    {
      flags.isTouched = false;
    }
    
    //-- ������ ��������� ������� -----
    if (flags.isTouched)
    {
        //������ �� ����� ����������
//        uint16_t bcd = ConvertToBCD(v.tXX[0]); //show X
//        LCD_ShowImage2FromRom(0,0,  ((bcd>>8)&0x0F), 1, LCD_WHITE, BackGroundColor);
//        LCD_ShowImage2FromRom(8,0,  ((bcd>>4)&0x0F), 1, LCD_WHITE, BackGroundColor);
//        LCD_ShowImage2FromRom(16,0, ((bcd>>0)&0x0F), 1, LCD_WHITE, BackGroundColor);
//
//        bcd = ConvertToBCD(v.tYY[0]); //show Y
//        LCD_ShowImage2FromRom(0,8,  ((bcd>>8)&0x0F), 1, LCD_WHITE, BackGroundColor);
//        LCD_ShowImage2FromRom(8,8,  ((bcd>>4)&0x0F), 1, LCD_WHITE, BackGroundColor);
//        LCD_ShowImage2FromRom(16,8, ((bcd>>0)&0x0F), 1, LCD_WHITE, BackGroundColor); 
        
        v.tObjectID = 0;
        
        if (flags.systemState == 10) //����� ���� ����������
        {
          //1-�� 2-�� >192
          //3-�� 4-�� >153
          //5-�� 6-�� >109
          //7-�� 8-����� >065
          if (v.tXX[0] < 123)
            v.tObjectID = 1;
//          else
//            v.tObjectID = 0;
          
          if (v.tYY[0] > 192)
          {
            v.tObjectID += 1; //�� ��
          }
          else
          {
            if (v.tYY[0] > 153)
            {
              v.tObjectID += 3; //�� ��
            }
            else
            {
              if (v.tYY[0] > 109)
              {
                v.tObjectID += 5; //�� ��
              }
              else
              {
                if (v.tYY[0] > 65)
                {
                  v.tObjectID += 7; //��
                  //v.tObjectID &= ~8; //if o==8 then o=0  
                }
                else
                {
                  if (v.tXX[0] < 71)
                  {
                    v.tObjectID = 128 + 4;//������ "cancel"
                  }
                  else
                  {
                    if (v.tXX[0] < 123)
                    {
                      v.tObjectID = 128 + 3;//������ "ok"
                    }
                    else
                    {
                      v.tObjectID = 0;
                    }
                  }
                }
              }
            }
          }
        }
        else
        {      
          //-- ��������� ����� �����
          if (v.tYY[0] < 71)
          {
            //����� ������ ����������
            if (v.tXX[0] < 71)
            {
              //������ "cancel"
              v.tObjectID = 128 + 4;
            }
            else
            {
              if (v.tXX[0] < 124)
              {
                //������ "ok"
                v.tObjectID = 128 + 3;
              }
              else
              {
                if (v.tXX[0] < 179)
                {
                  //������ "-"
                  v.tObjectID = 128 + 2;
                }
                else
                {
                  //������ "+"
                  v.tObjectID = 128 + 1;
                }
              }
            }
          }
          else
          {
            if (v.tYY[0] < 181)
            {
              //����� ����
              if (v.tXX[0] < 35)
              {}
              else
              {
                if (v.tXX[0] < 123)
                {
                  //����� �����
                  v.tObjectID = 64 + 2;
                }
                else
                {
                  if (v.tXX[0] < 205)
                  {
                    //����� �����
                    v.tObjectID = 64 + 1;
                  }
                }
              }
            }
            else
            {
              v.tObjectID = 32 + 0;
            }
          }
        }
        
//        LCD_ShowImage2FromRom(00,16, v.tObjectID / 10, 1, LCD_YELLOW, BackGroundColor); 
//        LCD_ShowImage2FromRom(8,16, v.tObjectID % 10, 1, LCD_YELLOW, BackGroundColor);
    }
}

//-------------------------------------------------------------------------------------------------
/* ������������ ���-�� ���� � �����. ����� ������ ������������� �����*/
uint8_t CharCount(uint8_t* address)
{
  uint8_t ret = 0;
  while (*address++ != 0) ret++;
  return ret;
}

//-------------------------------------------------------------------------------------------------
/* ��������� � Winbond ��������:
   v.digitsOffset
   v.mCalendarAlarmTime
   v.mCalendarAlarmDays[7]
 */
void SaveToRom()
{
  uint8_t c;
  
  //��������� ����� ������ (1) � �������� ��� �������� (2)
  while ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE) c = SPI1->DR; //1
  while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY); //2
  
  //�������� ���
  LCD_CS(CS_DISABLE);
  MEM_CS(CS_DISABLE);
  MEM_RESUME;
  
  //����� ������� 
  MemWriteEnable();
  
  //sector 2047 (last) erase
  MEM_CS(CS_ENABLE);
    SPI_Exchange(0x20);
    SPI_Exchange(0x7F);
    SPI_Exchange(0xF0);
    SPI_Exchange(0x00);
  MEM_CS(CS_DISABLE);
  //
  MemWaitForWriteEnd();
  //
  MemWriteEnable();
  
  //address 8384512 (first in sector 2047) program
  MEM_CS(CS_ENABLE);
    SPI_Exchange(0x02);
    SPI_Exchange(0x7F);
    SPI_Exchange(0xF0);
    SPI_Exchange(0x00);
    //data
    MemWriteSequence(&v.digitsOffset, 1);
    MemWriteSequence((uint8_t*)&v.mCalendarAlarmTime, 4);
    for (c=0; c<7; c++)
      MemWriteSequence((uint8_t*)&v.mCalendarAlarmDays[c] , 4);
  MEM_CS(CS_DISABLE);
  //
  MemWaitForWriteEnd();
}

//-------------------------------------------------------------------------------------------------
/* ������������ ������ ��������� ���� �� Winbond */
void LoadFromRom()
{
  uint8_t c;
  
  //��������� ����� ������ (1) � �������� ��� �������� (2)
  while ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE) c = SPI1->DR; //1
  while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY); //2
  
  //������� MEM
  LCD_CS(CS_DISABLE);
  MEM_CS(CS_DISABLE);
  MEM_RESUME;
  //����� ������� (fast read)
  MEM_CS(CS_ENABLE);
    SPI_Exchange(0x0B);
    SPI_Exchange(0x7F); //A2
    SPI_Exchange(0xF0); //A1
    SPI_Exchange(0x00); //A0
    SPI_Exchange(0x00); //dummy
    //
    MemReadSequence(&v.digitsOffset, 1);
    if (v.digitsOffset == 0xFF) v.digitsOffset = 0;
    //
    MemReadSequence((uint8_t*)&v.mCalendarAlarmTime , 4);
    if (v.mCalendarAlarmTime == 0xFFFFFFFF) v.mCalendarAlarmTime = 0; 
    //
    for (c=0; c<7; c++)
    {
      MemReadSequence((uint8_t*)&v.mCalendarAlarmDays[c] , 4);  
      if (v.mCalendarAlarmDays[c] == 0xFFFFFFFF) v.mCalendarAlarmDays[c] = 0; 
    }
  //����������
  MEM_CS(CS_DISABLE);  
}

//-------------------------------------------------------------------------------------------------
void MemWriteEnable()
{
  MEM_CS(CS_ENABLE); 
    SPI_Exchange(0x06); //write enable
  MEM_CS(CS_DISABLE); 
    Delay(10);
  MEM_CS(CS_ENABLE); 
    SPI_Exchange(0x05); //read SR
    while (!(SPI_Exchange(0xFF) & 0x02));
  MEM_CS(CS_DISABLE); 
    Delay(10);    
}

//-------------------------------------------------------------------------------------------------
void MemWaitForWriteEnd()
{
  MEM_CS(CS_ENABLE); 
    SPI_Exchange(0x05); //read SR
    while (SPI_Exchange(0xFF) & 0x01);
  MEM_CS(CS_DISABLE); 
    Delay(10);  
}

//-------------------------------------------------------------------------------------------------
void MemWriteSequence(uint8_t* data, uint8_t length)
{
  for (uint8_t i=0; i<length; i++)
  {
    SPI_Exchange(*data);
    data++;
  }
}

//-------------------------------------------------------------------------------------------------
void MemReadSequence(uint8_t* value, uint8_t length)
{
  for (uint8_t i=0; i<length; i++)
  {
    *value = SPI_Exchange(0xFF);
    value++;
  }
}
