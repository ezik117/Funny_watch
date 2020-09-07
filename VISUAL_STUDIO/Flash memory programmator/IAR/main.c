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
#define SPIon SPI1->CR1 |=SPI_CR1_SPE //включить SPI (CS=0)
#define SPIoff SPI1->CR1 &= ~SPI_CR1_SPE; //выключить SPI (CS=1)

#define HOLD1 GPIOA->BSRR = GPIO_BSRR_BS_0; //сигнал HOLD=1
#define HOLD0 GPIOA->BSRR = GPIO_BSRR_BR_0; //сигнал HOLD=0

#define CS1 GPIOA->BSRR = GPIO_BSRR_BS_1; //сигнал CS=1
#define CS0 GPIOA->BSRR = GPIO_BSRR_BR_1; //сигнал CS=0

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
  uint8_t temp8;
  uint32_t c;
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
    PA1 -  (/CS) (manual)
    PA5  - SPI SCK
    PA6  - SPI MISO
    PA7  - SPI MOSI
    PA0 - (/HOLD) (manual)
  */
  SET_BIT(GPIOA->MODER, (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1 |
                         GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1 |
                         GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0)); //PA2,3,5,6,7-альтернативная функция AF0(USART,SPI) + PA0,1-output
  SET_BIT(GPIOA->OSPEEDR, (GPIO_OSPEEDR_OSPEEDR5 | GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR7 |
                           GPIO_OSPEEDR_OSPEEDR2 | GPIO_OSPEEDR_OSPEEDR3 |
                           GPIO_OSPEEDR_OSPEEDR1 | GPIO_OSPEEDR_OSPEEDR0)); //all - high speed
  SET_BIT(GPIOA->PUPDR, (GPIO_PUPDR_PUPDR0_0 | GPIO_PUPDR_PUPDR1_0)); //PA4,10-pull up
  SET_BIT(GPIOA->OTYPER, (GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_2)); //As MISO pins of the slaves are connected together, all slaves must have the GPIO configuration
                                     //of their MISO pin set as alternate function open-drain
  GPIOA->AFR[0] = 0x1100; //номер альт.функции USART1 - AF01, SPI - AF00
  
  //--- конфигурируем SPI
  SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI; //Master mode, Fpclk/2 = 48/2
  SPI1->CR2 |= SPI_CR2_FRXTH; //8bit FIFO buffer, 8bit data size, NSS pin disabled, no interrupts
  
  //--- конфигурируем USART1 (full-duplex, 9600bps ,8bit, 1stop bit, no parity, no hardware flow control)
  v.udata_get = false;
  USART1->BRR = 0x1A1; //9600
  USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE; //interrupt enable, Rx/Tx enable, UART enable  
  
  //--- включаем прерывания
  NVIC_EnableIRQ(USART1_IRQn);
  __enable_irq();
  
  //--- другие инициализации
  v.isCommand = true;
  
HOLD1;
CS1;
SPIon;


//sample: read Manufacturer/Device IDs
//CS0;
//  SendSPI(0x90U);
//  SendSPI(0x00U);
//  SendSPI(0x00U);
//  SendSPI(0x00U);
//  v.c = SendSPI(0xFFU);
//  v.c <<= 8;
//  v.c |= SendSPI(0xFFU);  
//CS1;

//sample: read byte
//CS0;
//  spi_send(0x0B);
//  spi_send(0x00);
//  spi_send(0x00);
//  spi_send(0x00);
//  spi_send(0x00);
//  v.c = spi_send(0xFF); 
//CS1;

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
        v.temp8 = SendSPI(v.udata);
        v.isCommand = true;
      }
      
      //for (uint32_t t=0; t<10; t++) {};
      //отправка команды назад как сигнал подтверждения
      SendUData(v.udata);
      for (uint8_t t=0; t<10; t++) {};
      SendUData(v.temp8);
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
/* отправляет один байт данных в UART */
void SendUData(uint8_t data)
{
  while ((USART1->ISR & USART_ISR_TC) != USART_ISR_TC);
  USART1->TDR = data;
}

//------------------------------------------------------------------------------
/* производит отправку одного байта данных и возвращает считанные за этот же
   сеанс данные одного байта */
uint8_t SendSPI(uint8_t data)
{
  while ((SPI1->SR & SPI_SR_TXE) != SPI_SR_TXE);
  *(uint8_t *)&(SPI1->DR) = data;  //8bit data transfer!!!!
  while ((SPI1->SR & SPI_SR_RXNE) != SPI_SR_RXNE);
  return (uint8_t)SPI1->DR;
}