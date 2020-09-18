#include "stm32f030x6.h"
#include "stm32f0xx.h"

#define true 1
#define false 0
#define bool uint8_t

/*##############################################################################
# МИКРОПРОЦЕССОР: 
# ОПИСАНИЕ НАЗНАЧЕНИЯ ПРОГРАММЫ ЗДЕСЬ
#
##############################################################################*/

//--- макросы ------------------------------------------------------------------


//--- объявления функций -------------------------------------------------------
void SendUData(uint8_t data);
uint8_t SendSPI(uint8_t data);

//--- объявления переменных ----------------------------------------------------
static struct
{
  uint8_t i; //universal counter for loops
  volatile uint8_t udata; //data read from USART1 by interrupt routine
  volatile bool udata_get; //flag set to TRUE when a new data arrived from USART1
  bool isCommand;
  uint16_t c;
} v;

//=== MAIN =====================================================================
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
  RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN | RCC_APB2ENR_USART1EN); //включить тактирование SPI1 и USART1
  for (v.i=0; v.i<10; v.i++); //небольшой цикл что бы включить тактирование
  //---
  
  //--- настраиваем пины
  /*
    -- USART --
    PA2 - USART TX
    PA3 - USART RX

    -- SPI/MEM --
    PA5  - SPI SCK
    PA6  - SPI MISO
    PA7  - SPI MOSI
    PA9  - MEM CS (manual)
    PA10 - MEM HOLD (manual)
  */
  GPIOA->MODER |= (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1 |
                   GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1 |
                   GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0); //PA2,3,5,6,7-альтернативная функция AF0(USART,SPI) + PA9,10-output
  GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR5 | GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR7 |
                     GPIO_OSPEEDR_OSPEEDR2 | GPIO_OSPEEDR_OSPEEDR3 |
                     GPIO_OSPEEDR_OSPEEDR9 | GPIO_OSPEEDR_OSPEEDR10); //all - high speed
  GPIOA->PUPDR |= GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR10_0; //PA9,10-pull up
  GPIOA->OTYPER |= GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_2; //As MISO pins of the slaves are connected together, all slaves must have the GPIO configuration
                                     //of their MISO pin set as alternate function open-drain
  GPIOA->AFR[0] = 0x1100; //номер альт.функции USART1 - AF01, SPI - AF00
  
  //--- конфигурируем SPI
  SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI /* | SPI_CR1_BR_0*/; //Master mode, Fpclk/2 = 48/2
  SPI1->CR2 |= SPI_CR2_FRXTH | SPI_CR2_DS_2 |SPI_CR2_DS_1 |SPI_CR2_DS_0; //8bit FIFO buffer, no interrupts
  SPI1->CR1 |=SPI_CR1_SPE; //enable SPI 
  
  //--- конфигурируем USART1 (full-duplex, 9600bps ,8bit, 1stop bit, no parity, no hardware flow control)
  v.udata_get = false;
  USART1->BRR = 0x1388; //9600
  USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE; //interrupt enable, Rx/Tx enable, UART enable  
  
  //--- включаем прерывания
  NVIC_EnableIRQ(USART1_IRQn);
  __enable_irq();
  
  //--- другие инициализации
  v.isCommand = true;
  
#define CS1 GPIOA->BSRR = GPIO_BSRR_BS_9;
#define CS0 GPIOA->BSRR = GPIO_BSRR_BR_9;
#define HOLD1 GPIOA->BSRR = GPIO_BSRR_BS_10;
#define HOLD0 GPIOA->BSRR = GPIO_BSRR_BR_10;
  
HOLD1;
CS1;

CS0;
  SendSPI(0x90);
  SendSPI(0x00);
  SendSPI(0x00);
  SendSPI(0x00);
  v.c = SendSPI(0xFF);
  v.c <<= 8;
  v.c |= SendSPI(0xFF);
CS1;

  for (uint32_t t=0; t<65535; t++) {};

  
CS0;
  SendSPI(0x90);
  SendSPI(0x00);
  SendSPI(0x00);
  SendSPI(0x00);
  v.c = SendSPI(0xFF);
  v.c <<= 8;
  v.c |= SendSPI(0xFF);
CS1;

  while (1)
  {
    //проверка получения команды из USART1
    if (v.udata_get)
    {
      //обработка команды
      if (v.isCommand)
      {
        switch (v.udata)
        {
          case 0xAA: //next byte is SPI data
            v.isCommand = false;
            break;
          case 10: //CS=0
            CS0;
            break;
          case 11: //CS=1
            CS1;
            break;          
          case 20: //HOLD=0
            HOLD0;
            break;
          case 21: //HOLD=1
            HOLD1;
            break; 
          default:
            break;
        }
      }
      //обработка данных для SPI
      else
      {
        v.c = SendSPI(v.udata);
        v.isCommand = true;
      }
      
      for (uint32_t t=0; t<1000; t++) {};
      //отправка команды назад как сигнал подтверждения
      SendUData(v.udata);
      for (uint32_t t=0; t<1000; t++) {};
      SendUData(v.c);
      v.udata_get = false;
    }
  }
}

//=== NVIC функции =============================================================
void USART1_IRQHandler()
{
  if ((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
  {
      v.udata = (uint8_t)(USART1->RDR); /* Receive data, clear flag */
      v.udata_get = true;
  }
}

//=== функции ==================================================================
void SendUData(uint8_t data)
{
  while ((USART1->ISR & USART_ISR_TC) != USART_ISR_TC);
  USART1->TDR = data;
}

//------------------------------------------------------------------------------
uint8_t SendSPI(uint8_t data)
{
  //отправляем данные
  SPI1->DR = data;  
  //ждем пока не передадуться
  while ((SPI1->SR &SPI_SR_TXE) != SPI_SR_TXE);
  //ждем пока данные не придут обратно
  while ((SPI1->SR &SPI_SR_RXNE) != SPI_SR_RXNE);
  return SPI1->DR;
}
