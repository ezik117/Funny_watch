#include "stm32f030x6.h"
#include "stm32f0xx.h"
#include "images.h"
#include "main.h"

#define BackGroundColor 0x6B4D

//------ переменные ------------------------------------------------------------
volatile struct
{
  uint8_t TimeChanged : 1;
  uint8_t isTouched : 1;
} flags;

struct
{
  uint8_t i; //временная переменная для циклов и т.п.
  uint8_t j; //временная переменная для циклов и т.п.
  volatile uint32_t DelayCounter; //для SysTick
    
  uint32_t mCalendar;
  uint32_t mLastCalendar;
  
  uint8_t tXX[5]; //массив медианного фильтра X. значение tXX[1] содержит медиану
  uint8_t tYY[5]; //массив медианного фильтра Y. значение tYY[1] содержит медиану
  uint8_t tIterations; //кол-во считываний с экрана в рамках текущей сессии
  uint8_t tCompareMatch;
  
} v;

      
int main()
{
  //--- перевести MCU на тактовую частоту 48МГц ---
  RCC->CFGR =RCC->CFGR & (~RCC_CFGR_PLLMUL) | (RCC_CFGR_PLLMUL12); //задать множитель PLL x12 (48MHz)
  RCC->CR |=RCC_CR_PLLON; //включить PLL
  while((RCC->CR &RCC_CR_PLLRDY) == 0); //дождаться включения PLL
  
  RCC->CFGR |= (uint32_t) (RCC_CFGR_SW_PLL); //выбрать PLL источником SYSCLK
  while ((RCC->CFGR &RCC_CFGR_SWS) !=RCC_CFGR_SWS_PLL); //дожаться пока PLL не включится как источник SYSCLK
  //---
  
  //--- включить тактирование периферии 
  SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOAEN); //включить тактирование GPIO PORT A
  RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN); //включить тактирование SPI1
  RCC->APB1ENR |= RCC_APB1ENR_PWREN; //включить тактирование интерфейса power
  for (v.i=0; v.i<100; v.i++); //небольшой цикл что бы включить тактирование
  //---  
  
  //--- включить RTC
  PWR->CR |= PWR_CR_DBP; //включить доступ к регистру RCC->BDCR
    RCC->CSR |= RCC_CSR_LSION; //включить внутренний тактовый генератор 40кгц
    while((RCC->CSR &RCC_CSR_LSIRDY) == 0); //дождаться включения LSI 
    RCC->BDCR |= RCC_BDCR_RTCSEL_LSI | RCC_BDCR_RTCEN; //LSI source, RTC on
  //PWR->CR &= ~PWR_CR_DBP; //запретить доступ к регистру RCC->BDCR
  //---

  //--- настраиваем пины
  GPIOA->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | 
                   GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | 
                   GPIO_MODER_MODER7_1 | GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0);
  GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR0 | GPIO_OSPEEDR_OSPEEDR1 | GPIO_OSPEEDR_OSPEEDR2 |
                     GPIO_OSPEEDR_OSPEEDR3 | GPIO_OSPEEDR_OSPEEDR4 | GPIO_OSPEEDR_OSPEEDR5 |
                     GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR7 | GPIO_OSPEEDR_OSPEEDR9 |
                     GPIO_OSPEEDR_OSPEEDR10);
  //GPIOA->PUPDR |= GPIO_PUPDR_PUPDR3_0;
  
  //--- конфигурируем SPI
  SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE; //Master mode, Fpclk/2 = 48/2, SPI on
  SPI1->CR2 |= SPI_CR2_FRXTH; //8bit FIFO buffer, 8bit data size, NSS pin disabled, no interrupts
  
  //--- настройка SysTick (1милисек)
  SysTick->LOAD = (48000UL & SysTick_LOAD_RELOAD_Msk) - 1;
  SysTick->VAL = 0; //необходимо вызвать для обнуления
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | //источник тактирования SYSCLK
                   SysTick_CTRL_TICKINT_Msk   | //запрашивать прерывание по достижению нуля
                   SysTick_CTRL_ENABLE_Msk;     //включить счетчик 
  
  //--- включение таймеров и прерываний ---
  EXTI->IMR |= EXTI_IMR_IM17; //17ое внешнее прерывание это Alarm A
  EXTI->RTSR |= EXTI_RTSR_RT17;
  NVIC_EnableIRQ(RTC_IRQn);
  NVIC_SetPriority(SysTick_IRQn, 0); //systick имеет высший приоритет
  __enable_irq(); //включить все прерывания
  
  MEM_CS(CS_DISABLE);
  MEM_RESUME;
  LCD_CS(CS_DISABLE);  
  
  //инициализируем периферию
  //S(tart).AAA(ddress).M(ode).S(er/dfr).PP(ower)
  //Start= 1
  //Address= 101(X), 001(Y)
  //Mode= 1(8-bits), 0(12-bits)
  //Ser/dfr= 0(difference), 1(single-ended)
  //Power= 00(IRQ on)
  SPI1->CR1 |= SPI_CR1_BR_2; //XPT2046 F_CLK max = 2,5Mhz!!!!
  TOUCH_CS(CS_ENABLE);
    SPI_Exchange(0xD8); //read X
    SPI_Exchange(0x00);
    SPI_Exchange(0x00);
    SPI_Exchange(0x98); //read Y
    SPI_Exchange(0x00);
    SPI_Exchange(0x00);    
  TOUCH_CS(CS_DISABLE);
  SPI1->CR1 ^= SPI_CR1_BR_2;
  //
  LCD_Init();
  
  flags.TimeChanged = false;
  flags.isTouched = false;
  v.tIterations = 0;
  v.tCompareMatch = 0;
  v.tXX[4] = 0;
  v.tYY[4] = 0;
  
  //--- настройка RTC ---
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53; //enable write
  RTC->ISR |=RTC_ISR_INIT; //enable init phase, stop calendar ticking
  while ((RTC->ISR &RTC_ISR_INITF) != RTC_ISR_INITF); //wait while it is possible to modify
  RTC->PRER = 0x007F0137; //1 Hz
  RTC->TR = 0x00020300; //23:12 (24 hour format)
  RTC->ISR &=~RTC_ISR_INIT; //exit init phase, run calendar with new values
    RTC->CR &=~RTC_CR_ALRAE; //disable alarm A to modify it
    while ((RTC->ISR & RTC_ISR_ALRAWF) != RTC_ISR_ALRAWF); //wait until it can be possible to modify alarm A
    RTC->ALRMAR = RTC_ALRMAR_MSK1 | RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK4 | 0x00000059;
    RTC->CR |= (RTC_CR_ALRAE | RTC_CR_ALRAIE); //alarm A enable, alarm A interrupt enable
  RTC->WPR = 0xFE; 
  RTC->WPR = 0x64; //disable write 
  
  
  //задний фон
  LCD_CS(CS_ENABLE);
  LCD_SetWindow(0,319, 0,239);
  LCD_FillBackground(BackGroundColor);//(0x4228);
   
  //вывод картинки
  Time_Show(1);
  
  LCD_ShowImage16FromMem(0*80, 190, memMap[11], 0x00);
  LCD_ShowImage16FromMem(1*80, 190, memMap[12], 0x00);
  LCD_ShowImage16FromMem(2*80, 190, memMap[13], 0x00);
  LCD_ShowImage16FromMem(3*80, 190, memMap[14], 0x00);
 
  while (true)
  {
    Time_Show(0);  
    
    //проверим нет ли нажатия на экран
    if (TOUCH_PRESSED && (!flags.isTouched))
    {
#define T_MAX 5
      //опустошим буферы SPI Rx Tx
      while ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE) v.i = SPI1->DR;
      while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY);
      
      //понизим частоту до допустимой частоты обмена с TouchScr
      SPI1->CR1 |= SPI_CR1_BR_2; //set 1,5MHz [100(48/32=1,5MHz) 000(48/2=24MHz)]       
      TOUCH_CS(CS_ENABLE);
      
      //считываем значения
      SPI_Exchange(0xD8); //read X
      v.tXX[3] =  (SPI_Exchange(0x00) << 1);
      v.tXX[3] |= ((SPI_Exchange(0x00) & 0x80) >> 7);
        SPI_Exchange(0x98); //read Y     
        v.tYY[3] =  (SPI_Exchange(0x00) << 1);
        v.tYY[3] |= ((SPI_Exchange(0x00) & 0x80) >> 7);
        
       //вернули частоту SPI обратно и отключились от TouchScr
      SPI1->CR1 ^= SPI_CR1_BR_2; //set 24Mhz
      TOUCH_CS(CS_DISABLE); 
        
      //проверяем было ли чтение успешно
      if ((v.tXX[3] != 0) && (v.tYY != 0))
      {
        if (v.tIterations < 3)
        {
          v.tXX[v.tIterations] = v.tXX[3];
          v.tYY[v.tIterations] = v.tYY[3];
          v.tIterations++;
          v.tCompareMatch = 0;
          v.tXX[4] = 0;
          v.tYY[4] = 0;
        }
        else
        {
          //применяем медианный фильтр
          v.tIterations++;
          v.tXX[0] = v.tXX[3];
          if (v.tXX[1] < v.tXX[0]) {v.tXX[3]=v.tXX[0]; v.tXX[0]=v.tXX[1]; v.tXX[1]=v.tXX[3];}
          if (v.tXX[2] < v.tXX[0]) {v.tXX[3]=v.tXX[0]; v.tXX[0]=v.tXX[2]; v.tXX[2]=v.tXX[3];}
          if (v.tXX[2] < v.tXX[1]) {v.tXX[3]=v.tXX[1]; v.tXX[1]=v.tXX[2]; v.tXX[2]=v.tXX[3];}
          v.tYY[0] = v.tYY[3];   
          if (v.tYY[1] < v.tYY[0]) {v.tYY[3]=v.tYY[0]; v.tYY[0]=v.tYY[1]; v.tYY[1]=v.tYY[3];}
          if (v.tYY[2] < v.tYY[0]) {v.tYY[3]=v.tYY[0]; v.tYY[0]=v.tYY[2]; v.tYY[2]=v.tYY[3];}
          if (v.tYY[2] < v.tYY[1]) {v.tYY[3]=v.tYY[1]; v.tYY[1]=v.tYY[2]; v.tYY[2]=v.tYY[3];}
          
          if (v.tIterations > 20) //дождаться затухания переходных процессов при нажатии
          {
            if (v.tYY[4] == 0) v.tYY[4] = v.tXX[1]; //координаты на touch обратные
            if (v.tXX[4] == 0) v.tXX[4] = v.tYY[1];
            if ((v.tXX[1] == v.tYY[4]) && (v.tYY[1] == v.tXX[4])) v.tCompareMatch++;
            if (v.tCompareMatch == 40) //устойчивость сигнала
            {
              flags.isTouched = true;
              if (v.tIterations == 255) v.tIterations = 3;
            }
          }
        }
      }
    } //if (TOUCH_PRESSED)
    
    if (!TOUCH_PRESSED)
    {
      v.tIterations = 0;
      flags.isTouched = false;
    }
    
    //-- модуль обработки событий -----
    if (flags.isTouched)
    {
        //вывели на экран координату
        uint16_t bcd = ConvertToBCD(v.tXX[4]); //show X
        LCD_ShowImage2FromRom(0,0,  ((bcd>>8)&0x0F), LCD_WHITE);
        LCD_ShowImage2FromRom(8,0,  ((bcd>>4)&0x0F), LCD_WHITE);
        LCD_ShowImage2FromRom(16,0, ((bcd>>0)&0x0F), LCD_WHITE);

        bcd = ConvertToBCD(v.tYY[4]); //show Y
        LCD_ShowImage2FromRom(0,8,  ((bcd>>8)&0x0F), LCD_WHITE);
        LCD_ShowImage2FromRom(8,8,  ((bcd>>4)&0x0F), LCD_WHITE);
        LCD_ShowImage2FromRom(16,8, ((bcd>>0)&0x0F), LCD_WHITE); 
        
        //и номер блока
        //y [70..0] - горизонтальная линия кнопок управления
        //y [180..75] - горизонтальная линия цифр
        //x [70..0] - кнопка "+"
        //x [123..71] - кнопка "-"
        //х [178..124] - кнопка "ОК"
        //x [255..179] - кнопка "CANCEL"
        //-- определим номер линии
        if (v.tYY[4] < 71)
        {
          //линия кнопок управления
          v.j = 1;
          if (v.tXX[4] < 71)
          {
            //кнопка "+"
            v.i = 4;
          }
          else
          {
            if (v.tXX[4] < 124)
            {
              //кнопка "-"
              v.i = 3;
            }
            else
            {
              if (v.tXX[4] < 179)
              {
                //кнопка "ОК"
                v.i = 2;
              }
              else
              {
                //кнопка "CANCEL"
                v.i = 1;
              }
            }
          }
        }
        else
        {
          if (v.tYY[4] < 181)
          {
            //линия цифр
            v.j = 2;
          }
        }
        
        LCD_ShowImage2FromRom(00,16, v.j, LCD_YELLOW); 
        LCD_ShowImage2FromRom(16,16, v.i, LCD_YELLOW);
    }
    
    
  } //while (true)
}

//=== NVIC функции =============================================================
/* счетчик для функции Delay */
void SysTick_Handler()
{
 if (v.DelayCounter != 0) v.DelayCounter--;
}

//------------------------------------------------------------------------------

void RTC_IRQHandler()
{
  if ((RTC->ISR & RTC_ISR_ALRAF) == RTC_ISR_ALRAF)
  {
    flags.TimeChanged = true;
    RTC->ISR &= ~RTC_ISR_ALRAF; //clear flag
    
  }
  EXTI->PR |= EXTI_PR_PIF17; //clear flag by writting 1
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
void LCD_Init()
{
  SPI_WAIT_TX_READY; //ждем завершения любых передач из буфера
  
  //hardware reset
  LCD_RESET(RST_HI);
  Delay(5);
  LCD_RESET(RST_LO); //активный - низкий
  Delay(20);
  LCD_RESET(RST_HI);
  Delay(150); 
  
  MEM_CS(CS_DISABLE);
  LCD_CS(CS_ENABLE);
  
  //set up driver parameters
  uint32_t len, pos=0;
  for (v.i=0; v.i<19; v.i++) //строки массива
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
/* заполняет фон указанным цветом
   Функция LCD_SetWindow(0,319, 0,239) должна быть вызвана вначале */
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
/* устанавливает окно рисования. после данной функции можно отправлять данные сразу
   без переключения DX сигнала.
   x0, x1, y0, y1. Рисовать с x0 по x1, и y0 по y1. */
void LCD_SetWindow(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1)
{
  //установить границы окна рисования
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
/* записывает байт данных в буфер TXFIFO интерфейса SPI */
void SPI_Send(uint8_t data)
{
  SPI_WAIT_TX_READY;
  *(uint8_t *)&(SPI1->DR) = data; //отправляем в буфер 
}

//------------------------------------------------------------------------------
uint8_t SPI_Exchange(uint8_t data)
{
  //while ((SPI1->SR & SPI_SR_TXE) != SPI_SR_TXE); это лишнее, мы передаем в режиме безбуферности
  *(uint8_t *)&(SPI1->DR) = data;  //8bit data transfer!!!!
  while ((SPI1->SR & SPI_SR_RXNE) != SPI_SR_RXNE);
  return (uint8_t)SPI1->DR;
}

//------------------------------------------------------------------------------
/* вывод картинки из MEM */
void LCD_ShowImage16FromMem(uint16_t x, uint8_t y, uint32_t address, uint8_t mask)
{
  uint8_t c;
  
  //опустошим буфер приема (1) и закончим все передачи (2)
  while ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE) c = SPI1->DR; //1
  while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY); //2
  
  //выберем MEM
  LCD_CS(CS_DISABLE);
  MEM_CS(CS_ENABLE);
  MEM_RESUME;
  
  //команда: считать данные
  SPI_Exchange(0x0B);
  SPI_Exchange((uint8_t)(address >> 16));
  SPI_Exchange((uint8_t)(address >> 8));
  SPI_Exchange((uint8_t)(address));
  SPI_Exchange(0x00);
  
  //загрузка header
  for (uint8_t i=0; i<4; i++)
  {
    c = SPI_Exchange(0xFF);
    *(&BmpHeader16.width + i)  = c;
  }
  
  //установка окна вывода LCD
  MEM_PAUSE;
  LCD_CS(CS_ENABLE);
  LCD_SetWindow(x, x+BmpHeader16.width-1, y, y+BmpHeader16.height-1);
  LCD_CS(CS_DISABLE);
  MEM_RESUME;
  
  while ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE) c = SPI1->DR; //очистить Rx буфер
  while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY);
  
  //----- потоковыый обмен -----
  c = SPI_Exchange(0x00) ^ mask; //считали 1 байт
  LCD_CS(CS_ENABLE);
  
  for (uint32_t count=0; count < BmpHeader16.length; count++)
  {
	  c = SPI_Exchange(c) ^ mask; //отправили предыдущий и считали следующий байт	  
  }
  
  LCD_CS(CS_DISABLE);
  MEM_CS(CS_DISABLE);
}

//------------------------------------------------------------------------------
/* вывод картинки из ROM */
void LCD_ShowImage2FromRom(uint16_t x, uint8_t y, uint8_t imgIdx, uint16_t color)
{
  uint8_t c;
  
  //опустошим буфер приема (1) и закончим все передачи (2)
  while ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE) c = SPI1->DR; //1
  while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY); //2
  
  //установка окна вывода LCD
  LCD_CS(CS_ENABLE);
  LCD_SetWindow(x, x+7, y, y+6);
  
  //вывод изображения
  for (uint8_t row=0; row < 7; row++)
  {
    c = digits8[7*imgIdx + row];
    for (uint8_t col=0; col<8; col++)
    {
      if (c & 0x80)
      {
	SPI_Exchange((uint8_t)(color>>8));
        SPI_Exchange((uint8_t)color);
      }
      else
      {
        SPI_Exchange((uint8_t)(BackGroundColor>>8));
        SPI_Exchange((uint8_t)BackGroundColor);
      }
      c <<= 1;
    }
  }
  
  LCD_CS(CS_DISABLE);
}

//------------------------------------------------------------------------------
/* отображение времени, updateAll прорисовывает все включая двоеточие */
/* после вывода на экран обновленного времени, обновляет переменную v.TimeStored */
void Time_Show(bool updateAll)
{
        uint32_t difference;
        while((RTC->ISR &RTC_ISR_RSF) != RTC_ISR_RSF);
        v.mCalendar = RTC->TR;
  
	if (updateAll)
	{
          	LCD_ShowImage16FromMem(base_digits_x + 0,
							   base_digits_y,
							   memMap[(v.mCalendar >> 20) & 0x00000003], 0x00); //Hx:xx
		LCD_ShowImage16FromMem(base_digits_x + 60,
							   base_digits_y,
							   memMap[(v.mCalendar >> 16) & 0x0000000F], 0x00); //xH:xx
		LCD_ShowImage16FromMem(base_digits_x + 120,
							   base_digits_y + 27,
							   memMap[10], 0x00); //xx:xx
		LCD_ShowImage16FromMem(base_digits_x + 120 + 15,
							   base_digits_y,
							   memMap[(v.mCalendar >> 12) & 0x00000007], 0x00); //Mx:xx
		LCD_ShowImage16FromMem(base_digits_x + 180 + 15,
							   base_digits_y,
							   memMap[(v.mCalendar >> 8) & 0x0000000F], 0x00); //xM:xx
		LCD_ShowImage2FromRom(base_digits_x + 120 + 15 + 2*60,
							   base_digits_y + 106,
							   ((v.mCalendar >> 4) & 0x00000007), LCD_CYAN); //xx:Sx
		LCD_ShowImage2FromRom(base_digits_x + 120 + 15 + 2*60 + 8,
							   base_digits_y + 106,
							   ((v.mCalendar) & 0x0000000F), LCD_CYAN); //xx:xS	                							   memMap[(v.mCalendar) & 0x0000000F], 0x00); //xx:xS						   
	}
	else
	{
		if (!flags.TimeChanged) return;
                difference = v.mCalendar ^ v.mLastCalendar;
		
		if ((difference & 0x00300000) != 0)
		{
			LCD_ShowImage16FromMem(base_digits_x + 0,
								   base_digits_y,
								   memMap[(v.mCalendar >> 20) & 0x00000003], 0x00); //Hx:xx			
		}
		if ((difference & 0x000F0000) != 0)
		{
			LCD_ShowImage16FromMem(base_digits_x + 60,
								   base_digits_y,
								   memMap[(v.mCalendar >> 16) & 0x0000000F], 0x00); //xH:xx	 
                }
		if ((difference & 0x00007000) != 0)
		{
			LCD_ShowImage16FromMem(base_digits_x + 120 + 15,
								   base_digits_y,
								   memMap[(v.mCalendar >> 12) & 0x00000007], 0x00); //Mx:xx			
		}
		if ((difference & 0x00000F00) != 0)
		{
			LCD_ShowImage16FromMem(base_digits_x + 180 + 15,
								   base_digits_y,
								   memMap[(v.mCalendar >> 8) & 0x0000000F], 0x00); //xM:xx		
		}
		if ((difference & 0x00000070) != 0)
		{
			LCD_ShowImage2FromRom(base_digits_x + 120 + 15 + 2*60,
								   base_digits_y + 106,
								   ((v.mCalendar >> 4) & 0x00000007), LCD_CYAN); //xx:Sx		
		}
		if ((difference & 0x0000000F) != 0)
		{
			LCD_ShowImage2FromRom(base_digits_x + 120 + 15 + 2*60 + 8,
								   base_digits_y + 106,
								   ((v.mCalendar) & 0x0000000F), LCD_CYAN); //xx:xS	                
                
		}
	}
        
        v.mLastCalendar = v.mCalendar;
        flags.TimeChanged = false;
}

//-------------------------------------------------------------------------------------------------
uint16_t ConvertToBCD(uint8_t data)
{
  uint16_t ret;
  ret = (data / 100) << 8;
  data -= (data / 100) * 100;
  ret |= (data / 10) << 4;
  data -= (data / 10) * 10;
  ret |= data;
  return ret;
}