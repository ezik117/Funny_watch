#include "stm32f030x6.h"
#include "stm32f0xx.h"
#include "images.h"
#include "main.h"
#include <stdarg.h> 

//------ переменные ------------------------------------------------------------
volatile struct
{
  uint8_t TimeChanged : 1; //индикатор того, что время изменилось на 1 секунду
  uint8_t DateChanged : 1; //индикатор того, что дата изменилась
  uint8_t isTouched : 1; //показывает есть ли касание в текущий момент
  uint8_t needRepeat: 1; //показывает что не нужно выполнять условие while (TOUCH_PRESSED)
  uint8_t menuIsShowed : 1; //панель кнопок изменения времени\даты показана
  uint8_t systemState; //флаг состояния системы
} flags;
/*systemState:
  0: отображение времени
  1: редактирование часов (НH)
  2: редактирование часов (MM)
  3: редактирование даты (YYYY)
  4: редактирование даты (MM)
  5: редактирование даты (DD)
  6: сервисное меню
  7: справка
  8: будильник - установка часов (НН)
  9: будильник - установка минут (ММ)
  10: будильник - установка дней недели
*/

struct
{
  uint8_t i; //временная переменная для циклов и т.п.
  uint8_t j; //временная переменная для циклов и т.п.
  volatile uint32_t DelayCounter; //для SysTick
  
  volatile uint8_t bStatus; //бипер. статус. 0-выключен, 1-запуск. устанавливается пользователем.
  volatile uint16_t bCycle; //бипер. внутренний цикл тактов
  volatile uint8_t bCount; //бипер. количество повторений сигнала. задается пользователем.
    
  uint32_t mCalendarTime;
  uint32_t mCalendarDate;
  uint32_t mLastCalendar;
  uint32_t mCalendarAlarmTime; //время будильника
  uint32_t mCalendarAlarmDays[7]; //дни будильника
  uint8_t  mDoW_Checked[7]; //временный массив выбранных дней
  
  uint16_t mYear;
  uint8_t  mMonth;
  uint8_t  mDay;
  
  uint8_t tXX[2]; //массив X для обработки touch screen 
  uint8_t tYY[2]; //массив Y для обработки touch screen
  uint8_t tObjectID; //код нажатого объекта
  
  uint8_t digitsOffset;
  
  uint8_t arrBmpHeader16[8]; //буфер для загрузки bmp header 
  
  
} v;

      
int main()
{
  __enable_irq(); //включить все прерывания
    
  // HSE. SYSCLK = 48MHz (via PLL)
  SET_BIT(RCC->CR, RCC_CR_CSSON | RCC_CR_HSEON); // Включить генератор HSE и контроль над пропаданием
  while (!(RCC->CR & RCC_CR_HSERDY)); // Ожидание готовности HSE.

  MODIFY_REG(RCC->CFGR, (RCC_CFGR_PLLSRC_Msk | RCC_CFGR_PLLMUL_Msk),
            (RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLMUL6)); //задать источник и множитель PLL x6 (8x6=48MHz)
  SET_BIT(RCC->CR, RCC_CR_PLLON); //включить PLL
  while((RCC->CR & RCC_CR_PLLRDY) != 0); // ожидание готовности PLL
  SET_BIT(RCC->CFGR, (uint32_t)RCC_CFGR_SW_PLL); //выбрать PLL источником SYSCLK
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); //дождаться пока PLL не включится как источник SYSCLK
  
  //--- включить тактирование периферии 
  SET_BIT(RCC->AHBENR, (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN)); //включить тактирование GPIO PORT A, B
  SET_BIT(RCC->APB2ENR, (RCC_APB2ENR_SPI1EN)); //включить тактирование SPI1
  SET_BIT(RCC->APB1ENR, (RCC_APB1ENR_PWREN)); //включить тактирование интерфейса power
  
    //--- настройка SysTick (1милисек)
  v.bStatus = 0;
  SysTick->LOAD = (48000UL & SysTick_LOAD_RELOAD_Msk) - 1;
  SysTick->VAL = 0; //необходимо вызвать для обнуления
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | //источник тактирования SYSCLK
                   SysTick_CTRL_TICKINT_Msk   | //запрашивать прерывание по достижению нуля
                   SysTick_CTRL_ENABLE_Msk;     //включить счетчик 
  NVIC_SetPriority(SysTick_IRQn, 1); //выше приоритет только у перехода в сон
  
  //--- настройка RTC
  SET_BIT(PWR->CR, PWR_CR_DBP); //разрешить запись в регистры RTC и Backup (не запрещать в течении работы RTC!)
  SET_BIT(RCC->BDCR, RCC_BDCR_BDRST); // сброс RTC
  CLEAR_BIT(RCC->BDCR, RCC_BDCR_BDRST);
  SET_BIT(RCC->BDCR, RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL_HSE); // включить RTC и задать источником HSE/32

  //--- настраиваем пины
  WRITE_REG(GPIOA->MODER, 0x28000000 |
                        (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 |
                         GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 |
                         GPIO_MODER_MODER7_1 | GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0));
  WRITE_REG(GPIOB->MODER, GPIO_MODER_MODER1_0);

  WRITE_REG(GPIOA->OSPEEDR, 0x0C000000 | 
                          (GPIO_OSPEEDR_OSPEEDR0 | GPIO_OSPEEDR_OSPEEDR1 | GPIO_OSPEEDR_OSPEEDR2 |
                           GPIO_OSPEEDR_OSPEEDR3 | GPIO_OSPEEDR_OSPEEDR4 | GPIO_OSPEEDR_OSPEEDR5 |
                           GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR7 | GPIO_OSPEEDR_OSPEEDR9 |
                           GPIO_OSPEEDR_OSPEEDR10));
  SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDR_OSPEEDR1);
  
  WRITE_REG(GPIOA->OTYPER, (GPIO_OTYPER_OT_6));
  
  EXTI->IMR |= EXTI_IMR_IM17; //17ое внешнее прерывание это Alarm A
  EXTI->RTSR |= EXTI_RTSR_RT17;
  NVIC_EnableIRQ(RTC_IRQn);
  
  //--- конфигурируем SPI
  WRITE_REG(SPI1->CR1, (SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE)); //Master mode, Fpclk/2 = 48/2, SPI on
  WRITE_REG(SPI1->CR2, 0x0700 | SPI_CR2_FRXTH); //8bit FIFO buffer, 8bit data size, NSS pin disabled, no interrupts
  
  // -
  MEM_Deselect;
  LCD_Deselect;
  MEM_OffHold;
  
  //инициализируем периферию
  //S(tart).AAA(ddress).M(ode).S(er/dfr).PP(ower)
  //Start= 1
  //Address= 101(X), 001(Y)
  //Mode= 1(8-bits), 0(12-bits)
  //Ser/dfr= 0(difference), 1(single-ended)
  //Power= 00(IRQ on)
  SPI1->CR1 |= SPI_CR1_BR_2; //XPT2046 F_CLK max = 2,5Mhz!!!!
  TOUCH_Select;
    SPI_Send2(3, 0xD8, 0x00, 0x00); //инициализация для активирования PEN_IRQ  
  TOUCH_Deselect;
  SPI1->CR1 ^= SPI_CR1_BR_2;
  //
  
  LCD_Select;
  LCD_Init();

  
  //активируем режим контроля перехода в сон при пропадании напряжения
  //EXTI->IMR |= EXTI_IMR_MR10; //line 10 (PA10)
  //EXTI->FTSR |= EXTI_FTSR_TR10; //falling edge
  //NVIC_EnableIRQ(EXTI4_15_IRQn);
  //NVIC_SetPriority(EXTI4_15_IRQn, 0);
  
  
  //начальные значения
  flags.TimeChanged = false;
  flags.DateChanged = false;
  flags.isTouched = false;
  flags.menuIsShowed = false;
  flags.systemState = (uint8_t)0;
  BmpHeader16 = (TBmpHeader16*)&v.arrBmpHeader16;
  LoadFromRom(); //загрузка digitsOffset, mCalendarAlarmTime, mCalendarAlarmDays
  
  //--- настройка RTC ---
  Time_SetCalendarTM(0x00000000, 0x00000000);
  
  //задний фон
  LCD_FillRectangle(0,319, 0,239, BackGroundColor); //(0x4228);
  
  //вывод картинки
  Time_Show(1, 1, (TS_HH | TS_MM));
  Date_Show();
  
  while (true)
  {
    ProcessTouching();
    
    //действие в зависимости от флага состояния системы
    switch (flags.systemState)
    {
      //часы тикают
      case 0: 
      case 6:
        v.j = ~v.j;
        if (v.j) BEEP(1); else BEEP(0);
        Delay(250);
        //покажем текущее время
        Time_Show(0, 1, (TS_HH | TS_MM));  
        //был переход на новую дату - обновим информацию на дисплее 
        if (flags.DateChanged) 
        {   
          LCD_FillRectangle(0, 319, 0, base_digits_y - 1, BackGroundColor); //верх
          LCD_FillRectangle(0,319, 190,239, BackGroundColor); //низ        
          Date_Show();
          flags.DateChanged = false;
        }
        //проверим срабатывание будильника
        if (v.mCalendarAlarmTime != 0)
          if ((v.mCalendarTime & 0x003F7F00) == v.mCalendarAlarmTime)
          { //значение v.mCalendarTime содержит текущее время после выполнения команды Time_Show
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
              v.bStatus = 1; //запустим будильник
            }
          }
        break;
        
      default:
        break;
    } //switch  
    
    //обработаем нажатия на объекты, если есть
    if (v.tObjectID != 0)
    {
      //проверим нет ли режима справки
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
            
      //если нажатие на одну из цифр
      if ((v.tObjectID & 64) == 64)
      {
        //если панель не показана - покажем
        if (!flags.menuIsShowed)
        {
          //уберем верхнюю информацию
          LCD_FillRectangle(0, 319, 0, base_digits_y - 1, BackGroundColor);
          //уберем секунды
          if (v.digitsOffset == 0)
            LCD_FillRectangle(base_digits_x + 255, base_digits_x + 255 + 15, base_digits_y + 106, base_digits_y + 106 + 6, BackGroundColor);
          else
            LCD_FillRectangle(152, 152+15, 125, 125 + 7, BackGroundColor);
            
          //выведем панель
          LCD_ShowImage16FromMem(0, 190, memMap[11]);
          
          //установим флаги
          flags.menuIsShowed = true; 
          flags.systemState = v.tObjectID - 64;
          
          //считаем время из RTC
          while((RTC->ISR &RTC_ISR_RSF) != RTC_ISR_RSF);
          v.mCalendarTime = RTC->TR;
          v.mCalendarDate = RTC->DR;
          
          //спрячем часы или минуты
          if (flags.systemState == 1)
          {
            LCD_ShowText(52, 3+0, (uint8_t*)&TMessages[0], 2, LCD_YELLOW, BackGroundColor); //"УСТАНОВКА"
            LCD_ShowText(100, 3+30, (uint8_t*)&TMessages[10], 2, LCD_YELLOW, BackGroundColor); //"ЧАСОВ"
            if (v.digitsOffset == 0)
              LCD_FillRectangle(base_digits_x + 135, base_digits_x + 254, base_digits_y, base_digits_y + 114, BackGroundColor);
            else
              LCD_FillRectangle(base_fdigits_x + 166, base_fdigits_x + 119 + 166, base_digits_y, base_digits_y + 114, BackGroundColor);
          }
          
          if (flags.systemState == 2)
          {
            LCD_ShowText(52, 3+0, (uint8_t*)&TMessages[0], 2, LCD_YELLOW, BackGroundColor); //"УСТАНОВКА"
            LCD_ShowText(100, 3+30, (uint8_t*)&TMessages[16], 2, LCD_YELLOW, BackGroundColor); //"МИНУТ"
            if (v.digitsOffset == 0)
              LCD_FillRectangle(base_digits_x + 0, base_digits_x + 119, base_digits_y, base_digits_y + 114, BackGroundColor);
            else
              LCD_FillRectangle(base_fdigits_x, base_fdigits_x + 119, base_digits_y, base_digits_y + 114, BackGroundColor);
          }
        }
        v.tObjectID = (uint8_t)0;
      }
      
      //если нажатие на одну из кнонок или нижнюю часть экрана
      if ((v.tObjectID & 128) == 128)
      {
        if (flags.menuIsShowed)
        {      
          flags.needRepeat = true;
          //обработка кнопок
          switch (v.tObjectID)
          {
            //клавиша "+" ******************************************************
            case 0x81:
              //добавить минуту
              if (flags.systemState == 2 || flags.systemState == 9) //если установка часов времени или будильника
              {
                v.mCalendarTime = Time_Add(v.mCalendarTime, 0,1);
                Time_Show(1, 0, TS_MM);
              }
              //добавить час
              if (flags.systemState == 1 || flags.systemState == 8) //если установка часов времени или будильника
              {
                v.mCalendarTime = Time_Add(v.mCalendarTime, 1,0);
                Time_Show(1, 0, TS_HH);
              }
              //добавить год
              if (flags.systemState == 3)
              {
                v.mYear++;
                if (v.mYear > 9999) v.mYear = 2000;
                LCD_ShowDigits(40, base_digits_y, v.mYear, 4);
              }
              //добавить месяц
              if (flags.systemState == 4)
              {
                v.mMonth++;
                if (v.mMonth > 12) v.mMonth = 12;
                LCD_ShowDigits(100, base_digits_y, v.mMonth, 2);
              }   
              //добавить день
              if (flags.systemState == 5)
              {
                v.mDay++;
                if ((v.mMonth & 0x01U) == 0x01U) 
                {//нечетные месяцы
                  if (v.mDay > 31) v.mDay = 31;
                }
                else
                {//четные месяцы
                  if (v.mMonth == 2)
                  {//если февраль
                    if ((v.mYear & 3) == 0 && ((v.mYear % 25) != 0 || (v.mYear & 15) == 0))
                    {//високосный год
                      if (v.mDay > 29) v.mDay = 29;
                    }
                    else
                    {//обычный год
                      if (v.mDay > 28) v.mDay = 28;
                    }
                  }
                  else
                  {//если не февраль
                    if (v.mDay > 30) v.mDay = 30;
                  }
                }
                LCD_ShowDigits(100, base_digits_y, v.mDay, 2);
              }              
              //установить будильник
              if (flags.systemState == 6)
              {
                //загрузим значение будильника
                v.mCalendarTime = v.mCalendarAlarmTime;
                //покачем часы будильника MM:xx
                Time_Show(1, 0, TS_HH);
                //выведем надпись
                LCD_FillRectangle(0, 319, 0, base_digits_y - 1, BackGroundColor);
                LCD_ShowText(52, 3+0, (uint8_t*)&TMessages[0], 2, LCD_YELLOW, BackGroundColor); //"УСТАНОВКА"
                LCD_ShowText(40, 3+30, (uint8_t*)&TMessages[38], 2, LCD_YELLOW, BackGroundColor); //"БУДИЛЬНИКА"
                //сотрем минуты
                if (v.digitsOffset == 0)
                  LCD_FillRectangle(base_digits_x + 135, base_digits_x + 254, base_digits_y, base_digits_y + 114, BackGroundColor);
                else
                  LCD_FillRectangle(base_fdigits_x + 166, base_fdigits_x + 119 + 166, base_digits_y, base_digits_y + 114, BackGroundColor);  
                //уберем секунды
                if (v.digitsOffset == 0)
                  LCD_FillRectangle(base_digits_x + 255, base_digits_x + 255 + 15, base_digits_y + 106, base_digits_y + 106 + 6, BackGroundColor);
                else
                  LCD_FillRectangle(152, 152+15, 125, 125 + 7, BackGroundColor);
                //покажем меню
                LCD_ShowImage16FromMem(0, 190, memMap[11]);
                //установим флаги
                flags.systemState = 8;
                flags.menuIsShowed = true;
                v.tObjectID = (uint8_t)0;
                flags.needRepeat = false;
              }
              break;
              
            //клавиша "-" или "смена шрифта" ***********************************
            case 0x82:
              //отнять минуту
              if (flags.systemState == 2 || flags.systemState == 9) //если установка часов времени или будильника
              {
                v.mCalendarTime = Time_Sub(v.mCalendarTime, 0,1);
                Time_Show(1, 0, TS_MM);
              }
              //отнять час
              if (flags.systemState == 1 || flags.systemState == 8) //если установка часов времени или будильника
              {
                v.mCalendarTime = Time_Sub(v.mCalendarTime, 1,0);
                Time_Show(1, 0, TS_HH);
              }
              //отнять год
              if (flags.systemState == 3)
              {
                v.mYear--;
                if (v.mYear < 2000) v.mYear = 2000;
                LCD_ShowDigits(40, base_digits_y, v.mYear, 4);
              }
              //отнять месяц
              if (flags.systemState == 4)
              {
                v.mMonth--;
                if (v.mMonth < 1) v.mMonth = 1;
                LCD_ShowDigits(100, base_digits_y, v.mMonth, 2);
              } 
              //отнять день
              if (flags.systemState == 5)
              {
                v.mDay--;
                if (v.mDay < 1) v.mDay = 1;
                LCD_ShowDigits(100, base_digits_y, v.mDay, 2);
              } 
              //изменить шрифт
              if (flags.systemState == 6)
              {
                if (v.digitsOffset == 0)
                  v.digitsOffset = 13;
                else
                  v.digitsOffset = (uint8_t)0;
                //выведем время и дату
                LCD_FillRectangle(0, 319, 0, 239, BackGroundColor);
                Time_Show(1, 1, (TS_HH | TS_MM));
                Date_Show();                
                //установим флаги
                flags.systemState = (uint8_t)0;
                flags.menuIsShowed = false;
                flags.needRepeat = false;
                //сохраним состояние
                SaveToRom();
              }
              break;
                      
            //клавиша "ОК" или "справка" ***************************************
            case 0x83:
              flags.needRepeat = false;
              //изменить значение времени
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
              //перейти к редактированию месяца
              if (flags.systemState == 3)
              {
                //уберем часть верхней информации
                LCD_FillRectangle(0, 319, 3+30, base_digits_y - 1, BackGroundColor);
                //выведем надпись
                LCD_ShowText(88, 3+30, (uint8_t*)&TMessages[27], 2, LCD_YELLOW, BackGroundColor); //"МЕСЯЦА"
                //выведем месяц
                LCD_FillRectangle(0, 319, base_digits_y, base_digits_y + 114, BackGroundColor);
                LCD_ShowDigits(100, base_digits_y, v.mMonth, 2);                
                flags.systemState = 4;
                break;
              }
              //перейти к редактированию дня
              if (flags.systemState == 4)
              {
                //уберем часть верхней информации
                LCD_FillRectangle(0, 319, 3+30, base_digits_y - 1, BackGroundColor);
                //выведем надпись
                LCD_ShowText(124, 3+30, (uint8_t*)&TMessages[34], 2, LCD_YELLOW, BackGroundColor); //"ДНЯ"
                //выведем день
                LCD_FillRectangle(0, 319, base_digits_y, base_digits_y + 114, BackGroundColor);
                LCD_ShowDigits(100, base_digits_y, v.mDay, 2);                
                flags.systemState = 5;
                break;
              }
              //перейти к редактированию минут будильника
              if (flags.systemState == 8)
              {
                //выведем минуты будильника
                Time_Show(1, 0, TS_MM);
                //сотрем часы
                if (v.digitsOffset == 0)
                  LCD_FillRectangle(base_digits_x + 0, base_digits_x + 119, base_digits_y, base_digits_y + 114, BackGroundColor);
                else
                  LCD_FillRectangle(base_fdigits_x, base_fdigits_x + 119, base_digits_y, base_digits_y + 114, BackGroundColor);
                //установим флаги
                flags.systemState = 9;
                break;
              }
              //перейти к редактированию дней недели будильника
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
                //отобразим чекбоксы
                for (uint8_t i=0; i<7; i++)
                  v.mDoW_Checked[i] = (uint8_t)(v.mCalendarAlarmDays[i] >> RTC_DR_WDU_Pos);
                Alarm_ShowDowCheckboxes();
                //установим флаги
                flags.systemState = 10;
                break;
              }
              //установим новое значение будильника
              if (flags.systemState == 10)
              {
                //сохраним значение
                v.mCalendarAlarmTime = v.mCalendarTime;
                for (uint8_t i=0; i<7; i++)
                  v.mCalendarAlarmDays[i] = (v.mDoW_Checked[i] << RTC_DR_WDU_Pos);
                //обновим экран
                LCD_FillRectangle(0, 319, 0, 239, BackGroundColor);
                Time_Show(1, 1, (TS_HH | TS_MM));
                Date_Show();
                //установим флаги
                flags.menuIsShowed = false;
                flags.systemState = 0;
                //сохраним состояние
                SaveToRom();
                break;                
              }
              
              //установим новую дату
              if (flags.systemState == 5)
              {
                //заполним структуру даты новыми значениями
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
                //установим время
                Time_SetCalendarTM(0, v.mCalendarDate);
                //выведем время
                LCD_FillRectangle(0, 319, base_digits_y, base_digits_y + 114, BackGroundColor);
                Time_Show(1, 1, (TS_HH | TS_MM));
                //выведем дату и закроем панель
                LCD_FillRectangle(0, 319, 0, base_digits_y - 1, BackGroundColor); //верх
                LCD_FillRectangle(0,319, 190,239, BackGroundColor); //низ
                Date_Show();                
                //настроим флаги
                flags.menuIsShowed = false;
                flags.systemState = 0;
                break;                
              }
              //вызвать справку
              if (flags.systemState == 6)
              {              
                LCD_ShowImage16FromMem(0, 0, memMap[24]);
                //установим флаги
                flags.systemState = 7;
                flags.menuIsShowed = false;
              }
              break;
              
            //клавиша "CANCEL" *************************************************
            case 0x84:
              LCD_FillRectangle(0,319, 0,239, BackGroundColor);
              //выведем время
              //LCD_FillRectangle(0, 319, base_digits_y, base_digits_y + 114, BackGroundColor);
              Time_Show(1, 1, (TS_HH | TS_MM));
              //выведем дату и закроем панель
              //LCD_FillRectangle(0, 319, 0, base_digits_y - 1, BackGroundColor); //верх
              //LCD_FillRectangle(0,319, 190,239, BackGroundColor); //низ
              Date_Show();
              //установим флаги
              flags.systemState = 0;
              flags.menuIsShowed = false;
              flags.needRepeat = false;
              break;
              
            default:
              break;
          } //switch
          //подождать пока клавиша не будет отжата
          if (!flags.needRepeat)
            while (TOUCH_PRESSED) {Delay(100);}
          else
            Delay(100);
          v.tObjectID = 0; //обработали
        }
        else
        {
          //проверим не сервисное ли меню вызвано
          if (flags.systemState == 0)
          {
            if (!flags.menuIsShowed)
            {
              //покажем меню
              LCD_ShowImage16FromMem(0, 190, memMap[12]); //будильник
              //установим флаги
              flags.systemState = 6;
              flags.menuIsShowed = true;
              v.tObjectID = 0;
              while (TOUCH_PRESSED) {Delay(100);}
              //continue;
            }
          }
        }
      }
      
      //если нажатие на область даты
      if ((v.tObjectID & 32) == 32)
        if (flags.systemState == 0) //система была в состоянии отображения времени
        {
          //считаем время из RTC
          while((RTC->ISR &RTC_ISR_RSF) != RTC_ISR_RSF);
          v.mCalendarDate = RTC->DR;
          v.mYear = 2000 + 10*((v.mCalendarDate & RTC_DR_YT_Msk) >> RTC_DR_YT_Pos) + ((v.mCalendarDate & RTC_DR_YU_Msk) >> RTC_DR_YU_Pos);
          v.mMonth = 10*((v.mCalendarDate & RTC_DR_MT_Msk) >> RTC_DR_MT_Pos) + ((v.mCalendarDate & RTC_DR_MU_Msk) >> RTC_DR_MU_Pos);
          v.mDay = 10*((v.mCalendarDate & RTC_DR_DT_Msk) >> RTC_DR_DT_Pos) + ((v.mCalendarDate & RTC_DR_DU_Msk) >> RTC_DR_DU_Pos);
          //уберем верхнюю информацию
          LCD_FillRectangle(0, 319, 0, base_digits_y - 1, BackGroundColor);
          //выведем надпись
          LCD_ShowText(52, 3+0, (uint8_t*)&TMessages[0], 2, LCD_YELLOW, BackGroundColor); //"УСТАНОВКА"
          LCD_ShowText(112, 3+30, (uint8_t*)&TMessages[22], 2, LCD_YELLOW, BackGroundColor); //"ГОДА"
          //выведем год
          LCD_FillRectangle(0, 319, base_digits_y, base_digits_y + 114, BackGroundColor);
          LCD_ShowDigits(40, base_digits_y, v.mYear, 4);
          //выведем панель
          LCD_ShowImage16FromMem(0, 190, memMap[11]);
          //установим флаги
          flags.systemState = 3; //установка года
          flags.menuIsShowed = true; //панель отображена
          v.tObjectID = 0; //обработали
        }
      
      //установка чекбоксов выбора дня для будильника
      if ((flags.systemState == 10) && (v.tObjectID != 0))
      {
        //отметим выборку в массиве (0-день не выбран. 1..7 день выбран и отобразим
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

//=== NVIC функции =============================================================
/* счетчик для функции Delay */
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
/* событие Alarm таймера (не будильника!) - +1 секунда */
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
/* обработка прерывания на линии PA10 - пропадание основного питания */
void EXTI4_15_IRQHandler()
{
  if (EXTI->PR & EXTI_PR_PR10) //event triggering
  {
    EXTI->PR |= EXTI_PR_PR10;//не уверен что код дойдет до сюда, но можно скинуть флаг
    //отключим RTC Alarm что бы не оно не будило ядро 
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53; //enable write
      RTC->CR &=~RTC_CR_ALRAE; //disable alarm A
    RTC->WPR = 0xFE; 
    RTC->WPR = 0x64; //disable write
    //переведем MCU в режим StandBy
    SCB->SCR |= 4U; //SCB_SCR_SLEEPDEEP
    PWR->CR |= PWR_CR_PDDS;
    PWR->CSR &= ~PWR_CSR_WUF;
    __disable_irq();
    __WFI();
  }
}

//=== функции ==================================================================
/* блокируемая задержка в мс */
void Delay(uint32_t msTime)
{	
      v.DelayCounter = msTime;
      while (v.DelayCounter);
}

//------------------------------------------------------------------------------
/* инициализация дисплея */
/* настройка пина PA10 как вход для анализа перехода в сон */
void LCD_Init()
{
  SPI_WAIT_TX_READY; //ждем завершения любых передач из буфера
  
  //переконфигурируем PA10 как выход
  //GPIOA->MODER |= GPIO_MODER_MODER10_0;
  //GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR10;
  
  //hardware reset
  LCD_RST_Deactivate;
  Delay(5);
  LCD_RST_Activate;
  Delay(20);
  LCD_RST_Deactivate;
  Delay(150); 
  
  //переконфигурируем PA10 как цифровой вход с подтяжкой к земле
  //GPIOA->MODER &= ~GPIO_MODER_MODER10;	
  //GPIOA->PUPDR |= GPIO_PUPDR_PUPDR10_1;		
  
  //set up driver parameters
  uint32_t len, pos=0;
  for (v.i=0; v.i<19; v.i++) //строки массива
  {
    LCD_DC(LCD_CMD);
    SPI_Send2(1, LcdInitData[pos++]);
    len = LcdInitData[pos++];
    
    SPI_WAIT_FOR_COMPLETION;
    LCD_DC(LCD_DATA);
    for (v.j=0; v.j<len; v.j++)
    {
      SPI_Send2(1, LcdInitData[pos++]);
    }
  }
  
  Delay(120);
  SPI_WAIT_FOR_COMPLETION;
  LCD_DC(LCD_CMD);
  SPI_Send2(2, 0x29, 0x2C); //Display on. Write registers
  
  SPI_WAIT_FOR_COMPLETION;
}

//------------------------------------------------------------------------------
/* устанавливает окно рисования. после данной функции можно отправлять данные сразу
   без переключения DX сигнала.
   x0, x1, y0, y1. Рисовать с x0 по x1, и y0 по y1. */
void LCD_SetWindow(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1)
{
  //установить границы окна рисования
  SPI_WAIT_FOR_COMPLETION;
  LCD_DC(LCD_CMD);
  SPI_Send2(1, 0x2A);
  SPI_WAIT_FOR_COMPLETION;
  LCD_DC(LCD_DATA);
  SPI_Send2(4, (uint8_t)(x0>>8), (uint8_t)(x0), (uint8_t)(x1>>8), (uint8_t)(x1));
  SPI_WAIT_FOR_COMPLETION;
  LCD_DC(LCD_CMD);
  SPI_Send2(1, 0x2B);
  SPI_WAIT_FOR_COMPLETION;
  LCD_DC(LCD_DATA);
  SPI_Send2(4, (uint8_t)(y0>>8), (uint8_t)(y0), (uint8_t)(y1>>8), (uint8_t)(y1));
  SPI_WAIT_FOR_COMPLETION;
  LCD_DC(LCD_CMD);
  SPI_Send2(1, 0x2C); //Write registers
  SPI_WAIT_FOR_COMPLETION;
  LCD_DC(LCD_DATA);
}

//------------------------------------------------------------------------------
/* заполняет указанную область указанным цветом */
void LCD_FillRectangle(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1, uint16_t color)
{
  //закончим все передачи
  while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY);

  LCD_SetWindow(x0, x1, y0, y1);
  for (uint32_t i=0; i<(2*((x1-x0)*(y1-y0))); i++)
  {
    SPI_Send2(2, (uint8_t)(color >> 8), (uint8_t)(color));
  }
}

//------------------------------------------------------------------------------
void SPI_Send2(uint8_t size, ...)
{
  uint8_t data;
  va_list ap;
  va_start(ap, size);
  while (size-- > 0)
  {
    SPI_WAIT_TX_READY;
    data =(uint8_t)va_arg(ap, int);
    *(uint8_t *)&(SPI1->DR) = data;
  }
  va_end(ap);
  SPI_FlushRX();
}

//------------------------------------------------------------------------------
uint8_t SPI_Exchange(uint8_t data)
{
  *(uint8_t *)&(SPI1->DR) = (uint8_t)data;  //8bit data transfer!!!!
  while ((SPI1->SR & SPI_SR_RXNE) != SPI_SR_RXNE);
  return (uint8_t)SPI1->DR;
}

//------------------------------------------------------------------------------
void SPI_FlushRX()
{
  uint8_t temp __attribute__((unused));
  //опустошим буфер приема (1) и закончим все передачи (2)
  while ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE) *((uint8_t*)&temp) = SPI1->DR; //1
  while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY); //2
}

//------------------------------------------------------------------------------
/* вывод картинки из MEM */
void LCD_ShowImage16FromMem(uint16_t x, uint8_t y, uint32_t address)
{
  uint32_t c;
  
  //выберем MEM
  LCD_Deselect;
  MEM_Select;
  
  //команда: считать данные
  SPI_Send2(5, 0x0B, (uint8_t)(address >> 16), (uint8_t)(address >> 8), (uint8_t)(address), 0x00);
  
  SPI_FlushRX();
  
  //загрузка header
  for (uint8_t i=0; i<8; i++)
  {
    c = SPI_Exchange(0xFF);
    v.arrBmpHeader16[i] = c;
  }
  
  //установка окна вывода LCD
  MEM_OnHold;
  LCD_Select;
  LCD_SetWindow(x, x+BmpHeader16->width-1, y, y+BmpHeader16->height-1);
  LCD_Deselect;
  MEM_OffHold;
  
  SPI_FlushRX();
  
  
  //----- потоковыый обмен -----
  c = SPI_Exchange(0x00); //считали 1 байт
  LCD_Select;
  
  for (uint32_t count=0; count < BmpHeader16->length; count++) //коррекция для ширины больше 255
  {
	  c = SPI_Exchange(c); //отправили предыдущий и считали следующий байт	  
  }
  
  MEM_Deselect;
}

//------------------------------------------------------------------------------
/* вывод картинки из ROM */
/* font=1 (7x7) =2 (24x27) */
void LCD_ShowImage2FromRom(uint16_t x, uint8_t y, uint8_t imgIdx, uint8_t font, uint16_t fcolor, uint16_t bcolor)
{
  uint8_t c;
  
  SPI_FlushRX();
  
  //установка окна вывода LCD
  if (font == 1) LCD_SetWindow(x, x+7, y, y+6);
  if (font == 2) LCD_SetWindow(x, x+23, y, y+26);
  if (font == 3) LCD_SetWindow(x, x+23, y, y+26);

  //вывод изображения
  uint8_t rows;
  rows = 7; // font = 1
  if (font == 2) rows = 81;
  if (font == 3) 
  {
    rows = 81;
    fcolor = (L24X27FUN[83*imgIdx]<<8) + L24X27FUN[83*imgIdx + 1];
  }
  
  for (uint8_t row=0; row < rows; row++)
  {
    c = digits8[7*imgIdx + row]; // font=1
    if (font == 2) c = L24X27[81*imgIdx + row];
    if (font == 3) c = L24X27FUN[83*imgIdx + row + 2];
    
    if (font < 3)
    {
      for (uint8_t col=0; col<8; col++)
      { //выводим обычным фонтом
        if (c & 0x80)
        {
          SPI_Send2(2, (uint8_t)(fcolor>>8), (uint8_t)fcolor);
        }
        else
        {
          SPI_Send2(2, (uint8_t)(bcolor>>8), (uint8_t)bcolor);
        }
        c <<= 1;
      }
    }
    else
    { //выводим забавными буквами
      for (uint8_t col=0; col<8; col++)
      {
        if (c & 0x80)
        {
          SPI_Send2(2, (uint8_t)(fcolor>>8), (uint8_t)fcolor);
        }
        else
        {
          SPI_Send2(2, (uint8_t)(bcolor>>8), (uint8_t)bcolor);
        }
        c <<= 1;
      }      
    }
  }
}

//------------------------------------------------------------------------------
/* отображает текст с указанного адреса, для выбранного типа шрифта, */
/* укзанным цветом на указанном фоне */
/* текст будет перенесен на новую строку если достигнут конец экрана */
void LCD_ShowText(uint16_t x, uint8_t y, uint8_t* address, uint8_t font, uint16_t fcolor, uint16_t bcolor)
{
  uint8_t s;
  
  if (v.digitsOffset != 0) font = 3;
  
  while (*address != 0)
  {
    s = *address;
    if (s == 0x20)
    {
      s = 255; //пробел
    }
    else
    {
      if (s >= 0xC0U)
      {
        s = ABC[s - 0xC0U];  //буквы
      }
      else
      {
          s = s - 0x30; //цифры
      }
    }
    
    if (s != 255) 
      LCD_ShowImage2FromRom(x, y, s, font, fcolor, bcolor);
    else
      LCD_FillRectangle(x, x+23, y, y+27, bcolor); //пробел
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
/* отображает число на экране (фонт: основные цифры) */
/* value-число в десятичном виде, digits-число отображаемых разрядов*/
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
/* отображение времени, updateAll прорисовывает все включая двоеточие */
/* после вывода на экран обновленного времени, обновляет переменную v.TimeStored */
/* если realTime = true, то значение берется из RTC, иначе из глобальной переменной v.mCalendar*/
/* HHMM влияют только если updateAll=1. HHMM=1 отобразит только часы, =2 только минуты, =3 все*/
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
                  if (difference == 0x00235959) flags.DateChanged = true; //был переход на новую дату, в следующее обновление секунд обновим дату на дисплее
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
/* добавляет время к переменной */ 
uint32_t Time_Add(uint32_t time, uint8_t HH, uint8_t MM)
{
  time &= ~0x7F; //обнулим секунды
  
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
/* вычитает время от переменной */ 
uint32_t Time_Sub(uint32_t time, uint8_t HH, uint8_t MM)
{
  time &= ~0x7F; //обнулим секунды
  
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
/* устанавливает новое время и/или дату */
/* если одно из значений = 0, оно не устанавливается */
void Time_SetCalendarTM(uint32_t newTime, uint32_t newDate)
{
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53; //enable write
  SET_BIT(RTC->ISR, RTC_ISR_INIT); // входим в режим инциализации
  while ((RTC->ISR & RTC_ISR_INITF) != RTC_ISR_INITF); // ждем входа в режим
  //RTC->PRER = 0x007F0137; //1 Hz, делитель для LSI
  WRITE_REG(RTC->PRER,
          124U << RTC_PRER_PREDIV_A_Pos |
          1999U << RTC_PRER_PREDIV_S_Pos); // значение предделителей
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
/* показывает дату */
/* если установлен будильник - то показывает его */
void Date_Show()
{
  uint8_t value;
  uint16_t x;
  uint8_t t;
  
  while((RTC->ISR &RTC_ISR_RSF) != RTC_ISR_RSF);
  v.mCalendarTime = RTC->TR;
  v.mCalendarDate = RTC->DR; //дата должна быть считана только после считывания времени (я не знаю почему, но только так работает)
  
  //выведем день недели по центру
  value = (v.mCalendarDate & RTC_DR_WDU_Msk) >> RTC_DR_WDU_Pos;
  if (value < 6)
  {
    LCD_ShowText((320-24*CharCount((uint8_t*)&TDAYS[TDaysIDx[value]])) / 2, 200, (uint8_t*)&TDAYS[TDaysIDx[value]], 2, 0x87F0, BackGroundColor);
  }
  else
  {
    LCD_ShowText((320-24*CharCount((uint8_t*)&TDAYS[TDaysIDx[value]])) / 2, 200, (uint8_t*)&TDAYS[TDaysIDx[value]], 2, 0xFC10, BackGroundColor);
  }
  
  //высчитаем начало вывода строки
  t = 10*((v.mCalendarDate & RTC_DR_MT_Msk) >> RTC_DR_MT_Pos);
  t += (v.mCalendarDate & RTC_DR_MU_Msk) >> RTC_DR_MU_Pos;
  x = (320 - ((24*CharCount((uint8_t*)&TMONTHS[TMonths_IDx[t]])) + (2*24 + 0*24 + 1*10))) / 2;
  
#define base_date_y 10
  //выведем день
  value = (v.mCalendarDate & RTC_DR_DT_Msk) >> RTC_DR_DT_Pos;
  LCD_ShowImage2FromRom(x, base_date_y, value, (v.digitsOffset == 0 ? 2 : 3), LCD_YELLOW, BackGroundColor);
  x += 24;
  value = (v.mCalendarDate & RTC_DR_DU_Msk) >> RTC_DR_DU_Pos;
  LCD_ShowImage2FromRom(x, base_date_y, value, (v.digitsOffset == 0 ? 2 : 3), LCD_YELLOW, BackGroundColor);
  x += (24 + 10);
  
  //выведем месяц
  LCD_ShowText(x, base_date_y, (uint8_t*)&TMONTHS[TMonths_IDx[t]], 2, 0xC7FF, BackGroundColor);
  //x += 24*CharCount((uint8_t*)&TMONTHS[TMonths_IDx[t]]);
  //x += 10;
  
  //выведем значок будильника, если установлен
  if (v.mCalendarAlarmTime != 0)
  {
    uint32_t temp = 0;
    for (uint8_t i=0; i<7; i++) temp |= v.mCalendarAlarmDays[i];
    if (temp != 0)
      LCD_ShowImage16FromMem(319-24, 0, memMap[25]);
  }
}

//-------------------------------------------------------------------------------------------------
/* дает день недели по дате (с пн=1 по вс=7) */
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
/* показывает чекбоксы выбора дня будильника */
void Alarm_ShowDowCheckboxes()
{
  uint8_t x, y;
  for (uint8_t i=1; i<8; i++)
  {
    //вычислим место отображения чекбокса
    if (i & 1U) //нечетные
    {
      x = 55 - (24*2);
      y = 8 + (i >> 1)*47;
    }
    else //четные
    {
      x = 216 - (24*2);
      y = 8 + ((i >> 1) - 1)*47;
    }
    //отобразим
    if (v.mDoW_Checked[i-1] == 0)
      LCD_ShowText(x, y, " \0", 2, 0x0680, (v.digitsOffset == 0 ? LCD_CYAN : LCD_YELLOW));
    else
      LCD_ShowImage2FromRom(x, y, 33, 2, 0x0680, (v.digitsOffset == 0 ? LCD_CYAN : LCD_YELLOW));
  }
}

//-------------------------------------------------------------------------------------------------
/* преобразует десяитичное число в BCD. Максимальное значение 9999 */
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
/* Вынесенные в отдельную функцию операции с тачскрином */
/* 1000.xxxx -- первая линия */
/* 0100.xxxx -- вторая линия */
/* 0010.xxxx -- третья линия */
/* 0000.xxxx -- разлиновка дней недели будильника */
/* xxxx - код объекта внутри */
void ProcessTouching()
{
    //проверим нет ли нажатия на экран
    if (TOUCH_PRESSED && (!flags.isTouched))
    {
      SPI_FlushRX();
      
      //понизим частоту до допустимой частоты обмена с TouchScr
      SPI1->CR1 |= SPI_CR1_BR_2; //set 1,5MHz [100(48/32=1,5MHz) 000(48/2=24MHz)]
      LCD_Deselect;
      TOUCH_Select;
   
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
        
      SPI_Send2(3, 0xD8, 0x00, 0x00);
      
      //вернули частоту SPI обратно и отключились от TouchScr
      SPI1->CR1 ^= SPI_CR1_BR_2; //set 24Mhz
      TOUCH_Deselect;
      LCD_Select;
    } //if (TOUCH_PRESSED)
    
    if (!TOUCH_PRESSED)
    {
      flags.isTouched = false;
    }
    
    //-- модуль обработки событий -----
    if (flags.isTouched)
    {
        //вывели на экран координату
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
        
        if (flags.systemState == 10) //выбор дней будильника
        {
          //1-пн 2-вт >192
          //3-ср 4-чт >153
          //5-пт 6-сб >109
          //7-вс 8-сброс >065
          if (v.tXX[0] < 123)
            v.tObjectID = 1;
//          else
//            v.tObjectID = 0;
          
          if (v.tYY[0] > 192)
          {
            v.tObjectID += 1; //пн вт
          }
          else
          {
            if (v.tYY[0] > 153)
            {
              v.tObjectID += 3; //ср чт
            }
            else
            {
              if (v.tYY[0] > 109)
              {
                v.tObjectID += 5; //пт сб
              }
              else
              {
                if (v.tYY[0] > 65)
                {
                  v.tObjectID += 7; //вс
                  //v.tObjectID &= ~8; //if o==8 then o=0  
                }
                else
                {
                  if (v.tXX[0] < 71)
                  {
                    v.tObjectID = 128 + 4;//кнопка "cancel"
                  }
                  else
                  {
                    if (v.tXX[0] < 123)
                    {
                      v.tObjectID = 128 + 3;//кнопка "ok"
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
          //-- определим номер линии
          if (v.tYY[0] < 71)
          {
            //линия кнопок управления
            if (v.tXX[0] < 71)
            {
              //кнопка "cancel"
              v.tObjectID = 128 + 4;
            }
            else
            {
              if (v.tXX[0] < 124)
              {
                //кнопка "ok"
                v.tObjectID = 128 + 3;
              }
              else
              {
                if (v.tXX[0] < 179)
                {
                  //кнопка "-"
                  v.tObjectID = 128 + 2;
                }
                else
                {
                  //кнопка "+"
                  v.tObjectID = 128 + 1;
                }
              }
            }
          }
          else
          {
            if (v.tYY[0] < 181)
            {
              //линия цифр
              if (v.tXX[0] < 35)
              {}
              else
              {
                if (v.tXX[0] < 123)
                {
                  //цифры минут
                  v.tObjectID = 64 + 2;
                }
                else
                {
                  if (v.tXX[0] < 205)
                  {
                    //цифры часов
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
/* подсчитывает кол-во букв в слове. Слово должно заканчиваться нулем*/
uint8_t CharCount(uint8_t* address)
{
  uint8_t ret = 0;
  while (*address++ != 0) ret++;
  return ret;
}

//-------------------------------------------------------------------------------------------------
/* сохранить в Winbond значения:
   v.digitsOffset
   v.mCalendarAlarmTime
   v.mCalendarAlarmDays[7]
 */
void SaveToRom()
{
  uint8_t c;
  
  SPI_FlushRX();
  
  //отключим все
  LCD_Deselect;
  
  //обмен данными 
  MemWriteEnable();
  
  //sector 2047 (last) erase
  MEM_Select;
    SPI_Send2(4, 0x20, 0x7F, 0xF0, 0x00);
  MEM_Deselect;
  //
  MemWaitForWriteEnd();
  //
  MemWriteEnable();
  
  //address 8384512 (first in sector 2047) program
  MEM_Select;
    SPI_Send2(4, 0x02, 0x7F, 0xF0, 0x00); 
    //data
    MemWriteSequence(&v.digitsOffset, 1);
    MemWriteSequence((uint8_t*)&v.mCalendarAlarmTime, 4);
    for (c=0; c<7; c++)
      MemWriteSequence((uint8_t*)&v.mCalendarAlarmDays[c] , 4);
  MEM_Deselect;
  LCD_Select;
  //
  MemWaitForWriteEnd();
}

//-------------------------------------------------------------------------------------------------
/* восстановить данные указанные выше из Winbond */
void LoadFromRom()
{
  uint8_t c;
  
  SPI_FlushRX();
  
  //выберем MEM
  LCD_Deselect;

  //обмен данными (fast read)
  MEM_Select;
    SPI_Send2(5, 0x0B, 0x7F, 0xF0, 0x00, 0x00); //A2, A1, A0, dummy
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
  //завершение
  MEM_Deselect;  
  LCD_Select;
}

//-------------------------------------------------------------------------------------------------
void MemWriteEnable()
{
  MEM_Select; 
    SPI_Exchange(0x06); //write enable
  MEM_Deselect; 
  MEM_Select; 
    SPI_Exchange(0x05); //read SR
    while (!(SPI_Exchange(0xFF) & 0x02));
  MEM_Deselect;   
}

//-------------------------------------------------------------------------------------------------
void MemWaitForWriteEnd()
{
  MEM_Select; 
    SPI_Exchange(0x05); //read SR
    while (SPI_Exchange(0xFF) & 0x01);
  MEM_Deselect; 
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
