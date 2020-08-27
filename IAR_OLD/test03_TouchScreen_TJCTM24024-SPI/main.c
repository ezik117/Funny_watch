#include "stm32f030x6.h"
#include "stm32f0xx.h"
#include "main.h"

#define true 1
#define false 0
#define bool uint8_t

/*##############################################################################
# МИКРОПРОЦЕССОР: STM32F030F4P6
# ДАННЫЙ ПРИМЕР ПОКАЗЫВАЕТ ПОРЯДОК НАСТРОЙКИ SPI ИНТЕРФЕЙСА
# И КОММУНИКАЦИЮ С СЕНСОРНЫМ ЭКРАНОМ TJCTM24024
##############################################################################*/
//SPI:
// LCD_CS       - PA4
// LCD_SPI_MISO - PA6
// LCD_SPI_MOSI - PA7
// LCD_SPI_SCK  - PA5
// LCD_RST      - PA10
// LCD_DCX      - PA9

//--- макросы ------------------------------------------------------------------
#define LCD_RST_0 (GPIOA->BSRR = GPIO_BSRR_BR_10)
#define LCD_RST_1 (GPIOA->BSRR = GPIO_BSRR_BS_10)
#define LCD_CS_SEL  (GPIOA->BSRR = GPIO_BSRR_BR_4) //select LCD
#define LCD_CS_FREE  (GPIOA->BSRR = GPIO_BSRR_BS_4) //deselect LCD
#define TS_CS_SEL  (GPIOA->BSRR = GPIO_BSRR_BR_3) //select TouchScreen
#define TS_CS_FREE  (GPIOA->BSRR = GPIO_BSRR_BS_3) //deselect TouchScreen

#define LCD_DATA (GPIOA->BSRR = GPIO_BSRR_BS_9) //DCX = 1
#define LCD_CMD (GPIOA->BSRR = GPIO_BSRR_BR_9) //DCX = 0
#define SPI_WAIT_TX_END while((SPI1->SR & SPI_SR_BSY) != 0) {} //ждем пока полностью не будет завершена передача

#define LCD_BLACK        0x0000
#define LCD_BLUE         0x001F
#define LCD_RED          0xF800
#define LCD_GREEN        0x07E0
#define LCD_CYAN         0x07FF
#define LCD_MAGENTA      0xF81F
#define LCD_YELLOW       0xFFE0  
#define LCD_WHITE        0xFFFF

#define TEMP_MEASUR_VAL 20 //период измерения температуры


#define MY 128
#define MX 64
#define MV 32
#define ML 16
#define BGR 8
#define MH 4
static const uint8_t LcdInitData[] = {0xCB,5,0x39,0x2C,0x00,0x34,0x02,
                                      0xCF,3,0x00,0xC1,0x30,
                                      0xE8,3,0x85,0x00,0x78,
                                      0xEA,2,0x00,0x00,
                                      0xED,4,0x64,0x03,0x12,0x81,
                                      0xF7,1,0x20,
                                      0xC0,1,0x23, //power control: VRH[5:0]
                                      0xC1,1,0x10, //power control: SAP[2:0];BT[3:0]
                                      0xC5,1,0x3E, //VCM control
                                      0xC7,1,0x86, //VCM control2
                                      0x36,1, MV | MX | MY | BGR, //Memory access control:сверху вниз, слева направо, горизонтальная
                                      0x3A,1,0x55,
                                      0xB1,2,0x00,0x18,
                                      0xB6,3,0x08,0x82,0x27, //Display Function Control
                                      0xF2,1,0x00, //3Gamma Function Disable
                                      0x26,1,0x01, //Gamma curve selected
                                      0xE0,15,0x0F,0x31,0x2B,0x0C,0x0E,0x08,0x4E,0xF1,0x37,0x07,0x10,0x03,0x0E,0x09,0x00, //Set positive gamma
                                      0xE1,15,0x00,0x0E,0x14,0x03,0x11,0x07,0x31,0xC1,0x48,0x08,0x0F,0x0C,0x31,0x36,0x0F, //Set negative gamma
                                     0x11,0 //Exit sleep
}; //19 rows

//=== MAIN =====================================================================
int main()
{
  //*********** ПОДГОТОВКА ПЕРИФЕРИИ  ******************************************
  
  //--- перевести MCU на тактовую частоту 48МГц ---
  RCC->CFGR =RCC->CFGR & (~RCC_CFGR_PLLMUL) | (RCC_CFGR_PLLMUL12); //задать множитель PLL x12 (48MHz)
  RCC->CR |=RCC_CR_PLLON; //включить PLL
  while((RCC->CR &RCC_CR_PLLRDY) == 0); //дождаться включения PLL
  
  RCC->CFGR |= (uint32_t) (RCC_CFGR_SW_PLL); //выбрать PLL источником SYSCLK
  while ((RCC->CFGR &RCC_CFGR_SWS) !=RCC_CFGR_SWS_PLL); //дожаться пока PLL не включится как источник SYSCLK
  
  //--- включить тактирование периферии 
  RCC->AHBENR |= (RCC_AHBENR_GPIOAEN); //включить тактирование GPIO PORT A
  RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN | RCC_APB2ENR_USART1EN); //включить тактирование SPI1 и USART1
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; //включить тактирование TIM3

  //включить RTC
  RCC->APB1ENR |= RCC_APB1ENR_PWREN; //включить тактирование интерфейса power
  RCC->CSR |= RCC_CSR_LSION; //включить внутренний тактовый генератор 40кгц
  while((RCC->CSR &RCC_CSR_LSIRDY) == 0); //дождаться включения PLL
  PWR->CR |= PWR_CR_DBP; //включить доступ к регистру RCC->BDCR
  RCC->BDCR |= RCC_BDCR_RTCSEL_LSI | RCC_BDCR_RTCEN; //LSI source, RTC on
  while((RCC->BDCR &RCC_BDCR_RTCEN) == 0); //дождаться включения RTC
  //PWR->CR &= ~PWR_CR_DBP; //запретить доступ к регистру RCC->BDCR
  
  //--- настраиваем пины (потом заменить на числа)
  GPIOA->MODER |= (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1 |
                   GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0 | GPIO_MODER_MODER1_0); //PA5,6,7-альтернативная функция AF0(SPI) + PA9,10-output
  GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR5 | GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR7 |
                     GPIO_OSPEEDR_OSPEEDR9 | GPIO_OSPEEDR_OSPEEDR10); //all - high speed
  GPIOA->PUPDR |= GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR10_0; //PA9,10-pull up
  GPIOA->OTYPER |= GPIO_OTYPER_OT_0; //As MISO pins of the slaves are connected together, all slaves must have the GPIO configuration
                                     //of their MISO pin set as alternate function open-drain

  //--- настройка PA4, PA3: выход, push-pull, high speed, push-up
  GPIOA->MODER |= (GPIO_MODER_MODER3_0 | GPIO_MODER_MODER4_0);
  GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_3 | GPIO_OTYPER_OT_4);
  GPIOA->PUPDR |= (GPIO_PUPDR_PUPDR3_0 | GPIO_PUPDR_PUPDR4_0);
  GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR3 | GPIO_OSPEEDR_OSPEEDR4);  
  
  GPIOA->BSRR = GPIO_BSRR_BS_1; //в другой версии на этой ноге MEM CHIP
  
  //--- конфигурируем SPI1. Скорость SPI будет меняться в обработчиках прерываний
  LCD_CS_FREE;
  TS_CS_FREE;
  SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI /* | SPI_CR1_BR_0*/; //Master mode, Fpclk/2 = 48/2
  SPI1->CR2 = SPI_CR2_FRXTH | SPI_CR2_DS_2 |SPI_CR2_DS_1 |SPI_CR2_DS_0; //8bit FIFO buffer, no interrupts
  SPI1->CR1 |=SPI_CR1_SPE; //enable SPI  

  //--- настройка SysTick (1милисек)
  SysTick->LOAD = (48000UL & SysTick_LOAD_RELOAD_Msk) - 1;
  SysTick->VAL = 0; //необходимо вызвать для обнуления
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | //источник тактирования SYSCLK
                   SysTick_CTRL_TICKINT_Msk   | //запрашивать прерывание по достижению нуля
                   SysTick_CTRL_ENABLE_Msk;     //включить счетчик 
  
  //--- настройка таймера TIM3 (проверка обновления экрана времени)
  TIM3->PSC = 47999; //1ms
  TIM3->ARR = 1000; //1s
  TIM3->DIER |= TIM_DIER_UIE; //enable interrupts
  
  //--- конфигурируем USART1 (single-wire half-duplex, PA2-TX, no RX, 8bit, 1stop bit, no parity, no hardware flow control)
  GPIOA->MODER |= GPIO_MODER_MODER2_1; //PA2 - альтернативная функция
  GPIOA->AFR[0] = 0x0100; //номер альт.функции - AF01
  GPIOA->OTYPER |= GPIO_OTYPER_OT_2; //open-drain
  GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR2; //PA2-high speed
  USART1->CR3 |= USART_CR3_HDSEL; //single-wire half-duplex
  USART1->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE; //Rx/Tx enable, UART enable  
  
  //configure 1wire-probe to measure Temp with acquracy = 0.5C (9bits of data)
  v.TempMeasurementCountdown = 2;
  if (OW_PresenceDetected())
  {
    OW_SendByte(0xCC); //skip ROM command
    OW_SendByte(0x4E); //write Scratchpad
    OW_SendByte(0xFF); //write to T(hi) registry
    OW_SendByte(0x00); //write to T(lo) registry
    OW_SendByte(0x1F); //write to configuration registry
      //configure probe to measure Temp with acquracy = 0.5C (9bits of data)
      if (OW_PresenceDetected())
      {
        OW_SendByte(0xCC); //skip ROM command
        OW_SendByte(0x48); //copy Scratchpad into internal ROM
        Delay(50);
      }
  }
  
  //программируем TouchScreen драйвер, на то что бы он выдавал внешнее прерывание P_IRQ
  //прим: биты PD1-PD0 в ControlByte = 0
  SPI1->CR1 = ((SPI1->CR1 & (~SPI_CR1_BR_Msk)) | SPI_CR1_BR_2);//fpclk/32 = 1.5MHz  
  TS_CS_SEL;
  SPI_SingleData(0xD0); //read X
  SPI_SingleData(0x00);
  SPI_SingleData(0x00);
  SPI_SingleData(0x90); //read Y
  SPI_SingleData(0x00);
  SPI_SingleData(0x00);  
  TS_CS_FREE;

  //конфигурируем EXTI конороллер на внешнее прерывание от touchscreen
  SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PA; //PA1 -> EXTI1
  EXTI->IMR |= EXTI_IMR_MR1; //interrupt for EXTI1 line
  EXTI->RTSR |= EXTI_RTSR_TR1; //on rising edge

  //--- включение таймеров и прерываний ---
  
  NVIC_SetPriority(SysTick_IRQn, 0); //systick имеет высший приоритет
  NVIC_SetPriority(TIM3_IRQn_IRQn, 1); //обновление экрана идет со средним проритетом
  NVIC_SetPriority(EXTI0_1_IRQn, 3); //опрос touchscreen идет с меньшим приоритетом 
  
  NVIC_EnableIRQ(TIM3_IRQn_IRQn); //прерывание таймера обновления дисплея
  NVIC_EnableIRQ(EXTI0_1_IRQn);   //прерывание от драйвера touchscreen
  
  __enable_irq(); //включить все прерывания
    
  TIM3->CR1 |= TIM_CR1_CEN; //обновление дисплея вкл.

  
  
  //--- инициализация дисплея
  LCD_CS_SEL;
  LCD_Init();
  
  LCD_SetWindow(0x0000013F, 0x000000EF);
  LCD_FillBackground((uint16_t)LCD_BLACK);
  
  //сохраняем текущие данные регистров часов
  //в последствии мы будем смотреть не изменились ли они по таймеру что бы
  //понимать не нужно ли обновить информацию на экране
  v.RTC_DR_last = 0;
  v.RTC_TR_last = 0;
  
  //HH:MM
  LCD_SetWindow(H1X, H1Y);
  LCD_ShowChar((uint8_t*)&D56X93[0], 7, 93, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);
  LCD_SetWindow(H2X, H2Y);
  LCD_ShowChar((uint8_t*)&D56X93[0], 7, 93, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);  
  LCD_SetWindow(DPX, DPY);
  LCD_ShowChar((uint8_t*)&D56X93[10*7*93], 2, 93, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);  
  LCD_SetWindow(M1X, M1Y);
  LCD_ShowChar((uint8_t*)&D56X93[0], 7, 93, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);
  LCD_SetWindow(M2X, M2Y);
  LCD_ShowChar((uint8_t*)&D56X93[0], 7, 93, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK); 
  
  //CCo
  LCD_SetWindow(C1X, C1Y);
  LCD_ShowChar((uint8_t*)&D56X93[0], 7, 93, (uint16_t)LCD_GREEN, (uint16_t)LCD_BLACK);
  LCD_SetWindow(C2X, C2Y);
  LCD_ShowChar((uint8_t*)&D56X93[0], 7, 93, (uint16_t)LCD_GREEN, (uint16_t)LCD_BLACK);  
  LCD_SetWindow(COX, COY);
  LCD_ShowChar((uint8_t*)&D56X93[10*7*93+2*18], 2, 18, (uint16_t)LCD_GREEN, (uint16_t)LCD_BLACK);
  
  //день недели
  LCD_ShowText(Calendar_FindDayNameByNumber(0, (uint8_t*)&TDAYS[0]), 0, 0, 0, 24, 27, (uint16_t)LCD_WHITE, (uint16_t)LCD_BLACK);
  
  //календарь
  LCD_ShowText("2000", 0, 2, 0, 24, 27, (uint16_t)LCD_YELLOW, (uint16_t)LCD_BLACK);
  LCD_ShowText("01-ЯНВ", 0, 3, 8, 24, 27, (uint16_t)LCD_YELLOW, (uint16_t)LCD_BLACK);
  
  //запоминаем последнее значение календаря, инициализируем
  PWR->CR |= PWR_CR_DBP;
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;  
  RTC->ISR |=RTC_ISR_INIT;
  while ((RTC->ISR &RTC_ISR_INITF) !=RTC_ISR_INITF);
    RTC->PRER = 0x007F0137;
    RTC->TR = 2 << RTC_TR_HT_Pos | 1 << RTC_TR_HU_Pos | 4 << RTC_TR_MNT_Pos | 3 << RTC_TR_MNU_Pos;
    RTC->DR = 1 << RTC_DR_YT_Pos | 7 << RTC_DR_YU_Pos | 1 << RTC_DR_MT_Pos | 0 << RTC_DR_MU_Pos |
              2 << RTC_DR_DT_Pos | 0 << RTC_DR_DU_Pos |
              5 << RTC_DR_WDU_Pos;
  RTC->ISR &=~RTC_ISR_INIT;
  RTC->WPR = 0xFE;
  RTC->WPR = 0x64;
  


  
  while (1)
  {
    //end of program
  }
  

}

//=== NVIC функции =============================================================
void SysTick_Handler()
{
 if (v.DelayCounter != 0) v.DelayCounter--;
}

//------------------------------------------------------------------------------
void TIM3_IRQHandler()
{
 
//  //CCo - sec
//  LCD_SetWindow(C1X, C1Y);
//  LCD_ShowChar((uint8_t*)&D56X93[651*((RTC->TR & RTC_TR_ST_Msk)>>RTC_TR_ST_Pos)], 7, 93, (uint16_t)LCD_GREEN, (uint16_t)LCD_BLACK);
//  LCD_SetWindow(C2X, C2Y);
//  LCD_ShowChar((uint8_t*)&D56X93[651*((RTC->TR & RTC_TR_SU_Msk))], 7, 93, (uint16_t)LCD_GREEN, (uint16_t)LCD_BLACK);  

  if (((v.RTC_TR_last ^ RTC->TR) & 0x0000007F) != 0) //если последнее время без учета секунд изменилось - обновим дисплей
  {
    //обновим минуты
    LCD_SetWindow(M1X, M1Y);
    LCD_ShowChar((uint8_t*)&D56X93[651*((RTC->TR & RTC_TR_MNT_Msk)>>RTC_TR_MNT_Pos)], 7, 93, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);
    LCD_SetWindow(M2X, M2Y);
    LCD_ShowChar((uint8_t*)&D56X93[651*((RTC->TR & RTC_TR_MNU_Msk)>>RTC_TR_MNU_Pos)], 7, 93, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);  
    v.RTC_TR_last = RTC->TR;
    //и часы
    LCD_SetWindow(H1X, H1Y);
    LCD_ShowChar((uint8_t*)&D56X93[651*((RTC->TR & RTC_TR_HT_Msk)>>RTC_TR_HT_Pos)], 7, 93, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);
    LCD_SetWindow(H2X, H2Y);
    LCD_ShowChar((uint8_t*)&D56X93[651*((RTC->TR & RTC_TR_HU_Msk)>>RTC_TR_HU_Pos)], 7, 93, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);   
    v.RTC_TR_last = RTC->TR;
  }  
  
  if ((v.RTC_DR_last ^ RTC->DR) != 0) //если последняя дата изменилась - обновим дисплей
  {
    //год
    LCD_ShowText((uint8_t*)&TDIGITS[2*((RTC->DR & RTC_DR_YT_Msk)>>RTC_DR_YT_Pos)], 2, 2, 0, 24, 27, (uint16_t)LCD_YELLOW, (uint16_t)LCD_BLACK);
    LCD_ShowText((uint8_t*)&TDIGITS[2*((RTC->DR & RTC_DR_YU_Msk)>>RTC_DR_YU_Pos)], 3, 2, 0, 24, 27, (uint16_t)LCD_YELLOW, (uint16_t)LCD_BLACK);
    //месяц 
    LCD_ShowText((uint8_t*)&TMONTHS[4* (((0x0A*((RTC->DR & RTC_DR_MT_Msk)>>RTC_DR_MT_Pos)) +
                                               ((RTC->DR & RTC_DR_MU_Msk)>>RTC_DR_MU_Pos)) - 1)
                 ], 3, 3, 8, 24, 27, (uint16_t)LCD_YELLOW, (uint16_t)LCD_BLACK);    
    //день
    LCD_ShowText((uint8_t*)&TDIGITS[2*((RTC->DR & RTC_DR_DT_Msk)>>RTC_DR_DT_Pos)], 0, 3, 8, 24, 27, (uint16_t)LCD_YELLOW, (uint16_t)LCD_BLACK);
    LCD_ShowText((uint8_t*)&TDIGITS[2*((RTC->DR & RTC_DR_DU_Msk)>>RTC_DR_DU_Pos)], 1, 3, 8, 24, 27, (uint16_t)LCD_YELLOW, (uint16_t)LCD_BLACK);
    //день недели
    LCD_ShowText((uint8_t*)&TDAYS[12*(((RTC->DR & RTC_DR_WDU_Msk)>>RTC_DR_WDU_Pos)-1)], 0, 0, 0, 24, 27, (uint16_t)LCD_WHITE, (uint16_t)LCD_BLACK);
    
    v.RTC_DR_last = RTC->DR;   
  }
  
  //температура  
  
  if (v.TempMeasurementCountdown == 0)
  {
    if (OW_PresenceDetected())
    {
      //read temperature to probe
      OW_SendByte(0xCC); //skip ROM command
      OW_SendByte(0x44); //conversation T
      for (v.TempDelay=0; v.TempDelay<10000; v.TempDelay++) {__NOP();} //Delay
        //read temperature from probe
        if (OW_PresenceDetected())
        {
          v.TempCRC = 0;
          OW_SendByte(0xCC); //skip ROM command
          OW_SendByte(0xBE); //read Scratchpad
          for (v.i=0; v.i<8; v.i++)
          {
            OW_Data[v.i] = OW_ReadByte();
            v.TempCRC = TCRC[(OW_Data[v.i] ^ v.TempCRC)];
          }
          OW_Data[8] = OW_ReadByte();
         
          if (OW_Data[8] == v.TempCRC) //если CRC совпал
          {
            v.TempVal = 256*OW_Data[1] + OW_Data[0];
            v.TempVal &= 0x07FF; //удалим биты знака температуры
            v.TempVal >>= 3; //удалим неиспользуемые десятичные данные
            OW_Data[8] = (uint8_t)(v.TempVal & 0x01);//сохраним десятичное значение 0=0 или 1=0.5
            v.TempVal >>= 1; //удалим десятичные данные
            v.TempVal += OW_Data[8]; //округлим целое число
            //вставить код коррекции считанных данных здесь
            LCD_SetWindow(C1X, C1Y);
            LCD_ShowChar((uint8_t*)&D56X93[651*(v.TempVal / 10)], 7, 93, (uint16_t)LCD_GREEN, (uint16_t)LCD_BLACK);
            LCD_SetWindow(C2X, C2Y);
            LCD_ShowChar((uint8_t*)&D56X93[651*(v.TempVal % 10)], 7, 93, (uint16_t)LCD_GREEN, (uint16_t)LCD_BLACK); 
          }         
        } 
    }  
    v.TempMeasurementCountdown = TEMP_MEASUR_VAL;
  }
  
  v.TempMeasurementCountdown--;

  TIM3->SR &=~ TIM_SR_UIF; //сбрасываем флаг прерывания
}

//------------------------------------------------------------------------------
void EXTI0_1_IRQHandler(void)
{
  if( (EXTI->IMR & EXTI_IMR_MR1) && (EXTI->PR & EXTI_PR_PR1))
    {

     EXTI->PR |= EXTI_PR_PR0; //clear interrupt flag
    }
}

//=== функции ==================================================================
void Delay(uint32_t msTime)
{	
      v.DelayCounter = msTime;
      while (v.DelayCounter);
}

//------------------------------------------------------------------------------
void LCD_Init()
{
  SPI_WAIT_TX_END; //ждем завершения любых передач из буфера
  
  //hardware reset
  LCD_CS_FREE;
  LCD_RST_1;
  Delay(5);
  LCD_RST_0; //активный - низкий
  Delay(20);
  LCD_RST_1;
  Delay(150); 
  LCD_CS_SEL;
  
  //set up driver parameters
  uint32_t i,j,len, pos=0;
  for (i=0; i<19; i++) //строки массива
  {
    SPI_WAIT_TX_END;
    LCD_CMD;
    SPI_DataStream(LcdInitData[pos++]);
    len = LcdInitData[pos++];
    SPI_WAIT_TX_END;
    LCD_DATA;
    for (j=0; j<len; j++)
    {
      SPI_DataStream(LcdInitData[pos++]);
    }
  }
  
  Delay(120);
  SPI_WAIT_TX_END;
  LCD_CMD;
  SPI_DataStream(0x29); //Display on
  SPI_DataStream(0x2C); //Write registers
  
  SPI_WAIT_TX_END;
}
                   
//------------------------------------------------------------------------------
/* заполняет заданную ранее область указанным цветом */
void LCD_FillBackground(uint16_t color)
{
  uint32_t t;
  for (t=0; t<(240*320); t++)
  {
    SPI_DataStream((uint8_t)(color>>8));
    SPI_DataStream((uint8_t)(color));
  }
}

//------------------------------------------------------------------------------
/* устанавливает окно рисования. после данной функции можно отправлять данные сразу
   без переключения DX сигнала.
   columns, rows - 32х битные переменные, где первые 16 бит содержат начальное значение
   вторые 16 бит конечное значение. Например 0x00010002 - рисовать со первой по вторую строку/столбец */
void LCD_SetWindow(uint32_t columns, uint32_t rows)
{
  //установить границы окна рисования
  SPI_WAIT_TX_END;
  LCD_CMD;
  SPI_DataStream(0x2A);
  SPI_WAIT_TX_END;
  LCD_DATA;
  SPI_DataStream((uint8_t)(columns>>24));
  SPI_DataStream((uint8_t)(columns>>16)); //SC = column start
  SPI_DataStream((uint8_t)(columns>>8));
  SPI_DataStream((uint8_t)(columns)); //EC = column end  
  SPI_WAIT_TX_END;
  LCD_CMD;
  SPI_DataStream(0x2B);
  SPI_WAIT_TX_END;
  LCD_DATA;
  SPI_DataStream((uint8_t)(rows>>24));
  SPI_DataStream((uint8_t)(rows>>16));
  SPI_DataStream((uint8_t)(rows>>8));
  SPI_DataStream((uint8_t)(rows));
  SPI_WAIT_TX_END;
  LCD_CMD;
  SPI_DataStream(0x2C); //Write registers
  SPI_WAIT_TX_END;
  LCD_DATA;
}

//------------------------------------------------------------------------------
/* задает окно рисования для символа с симметричнымы размерами A на B, где А=B.
   X,Y - координаты симовола в терминах символьного экрана, size - размеры символа
   (А или В) точках */
void LCD_SetAtPos(uint8_t X, uint8_t Y, uint8_t size)
{
  LCD_SetWindow( (uint32_t)(((X*size)<<16) | ((X+1)*size-1)), (uint32_t)((Y*size)<<16) | ((Y+1)*size-1) );
}

//------------------------------------------------------------------------------
/* выводит символ в заданную ранее область.
   pdata - указатель на массив с данными символа
   XBytes - количество байтов содержащих горизонтальные точки символа
   YLines - количество вертикальных линий символа 
   (например XBytes=3, YLines = 24, где 3 это 8*3=24 точек по горизонтали = YLines)
   fcolor - цвет символа
   bgcolor - цвет заднего фона */
void LCD_ShowChar(uint8_t* pdata, uint8_t XBytes, uint8_t YLines, uint16_t fcolor, uint16_t bgcolor)
{
  uint8_t t, xb, n, c;
  while (YLines--)
  {
    for (xb=0; xb<XBytes; xb++)
    {
      t = *pdata++;
      for (n=0; n<8; n++)
      {
        c = t & 128;
        t <<= 1;
        if (c == 128) 
        {
          SPI_DataStream((uint8_t)(fcolor>>8));
          SPI_DataStream((uint8_t)(fcolor));
        }
        else
        {
          SPI_DataStream((uint8_t)(bgcolor>>8));
          SPI_DataStream((uint8_t)(bgcolor));
        }
      } 
    }
  }
}

//------------------------------------------------------------------------------
void LCD_ShowText(uint8_t* text, uint8_t XPos, uint8_t YPos, int8_t YOffset, uint8_t WChar, uint8_t HChar, uint16_t fcolor, uint16_t bgcolor)
{
  uint8_t idx;
  while (*text != 0)
  {
    LCD_SetWindow((((XPos*WChar)<<16) | ((XPos+1)*WChar-1)),
                  (((YOffset + YPos*HChar)<<16) | (YOffset + (YPos+1)*HChar-1)));
    idx = *text;
    if (idx >= 0xC0)
    {
      idx = 11 + idx - 0xC0;
    }
    else
    {
      if (idx >= 0x30)
      {
        idx = idx - 0x30;
      }
      else
      {
        if (idx == 0x20) idx = 43; //пробел
        if (idx == 0x2D) idx = 10; //дефис
      }
    }
    LCD_ShowChar((uint8_t*)&L24X27[idx*3*27], (WChar / 8), HChar, fcolor, bgcolor);
    text++;
    XPos++;
  }
}

//------------------------------------------------------------------------------
/* ищет вхождение в массиве названий в зависимости от цифры начиная с 0 */
uint8_t* Calendar_FindDayNameByNumber(uint8_t NDay, uint8_t* array)
{
  uint8_t CDay = 0;
  while (CDay != NDay)
  {
    while (*array++ != 0);
    CDay++;
  }
  return array;
}

//------------------------------------------------------------------------------
/* записывает байт данных в буфер TXFIFO интерфейса SPI, если места в буфере нет
   ожидает пока оно не появится */
void SPI_DataStream(uint8_t data)
{
  while ((SPI1->SR & SPI_SR_TXE) == 0); 
  *(uint8_t *)&(SPI1->DR) = data; //отправляем в буфер 
}

//------------------------------------------------------------------------------
/* отправляет байт данных через SPI и дожидается его отправки, возвращает считанное значение */
uint8_t SPI_SingleData(uint8_t data)
{
  while (SPI1->SR & SPI_SR_BSY);
  SPI1->DR = data;
  while ((SPI1->SR & SPI_SR_RXNE) != SPI_SR_RXNE);
  return SPI1->DR;
}

//------------------------------------------------------------------------------
/*1-wire. Определяет присутствие устройств(а) на линии. 1-если есть, 0-если нет*/
uint8_t OW_PresenceDetected()
{
  uint8_t ret,b;
  ret = 0;
  USART1->CR1 &= ~USART_CR1_UE;
  USART1->BRR = 0x1388; //переключаем в 9600 baud для передачи presence импульса
  USART1->CR1 |= USART_CR1_UE;
  b = OW_SendBit(0xF0);
  if (b < 0xF0) ret = 1;
  USART1->CR1 &= ~USART_CR1_UE;
  USART1->BRR = 0x01A1; //переключаем в 115200 baud для передачи данных
  USART1->CR1 |= USART_CR1_UE;
  return ret;
}

//------------------------------------------------------------------------------
/*1-wire. Отправляет и считывает 1 бит данных*/
uint8_t OW_SendBit(uint8_t bit)
{
//  USART1->TDR = bit;
//  while ((USART1->ISR & USART_ISR_TC) == 0);
//  while ((USART1->ISR & USART_ISR_RXNE) == 0);
//  return USART1->RDR;
  return 0; //у нас нет устройств 1wire
}

//------------------------------------------------------------------------------
/*1-wire. Отправляет 1 байт данных. Возвращаемое значение должно быть равно отправляемому,
иначе ошибка передачи данных*/
uint8_t OW_SendByte(uint8_t data)
{
  uint32_t i, b, ret;
  ret = 0;
  for (i=0; i<8; i++)
  {
    if ((data & 0x01) == 1) //отправим 1
      b = OW_SendBit(0xFF);
    else                    //отправим 0
      b = OW_SendBit(0x00);
    
    ret >>= 1;
    if (b == 0xFF) ret |= 0x80;

    data >>= 1;
  }
  return ret;
}

//------------------------------------------------------------------------------
/*1-wire. Считывает 1 байт данных.*/
uint8_t OW_ReadByte()
{
  uint32_t i, ret;
  ret = 0;
  for (i=0; i<8; i++)
  {
    ret >>= 1; 
    if (OW_SendBit(0xFF) == 0xFF) ret |= 0x80;
  }
  return ret;
}