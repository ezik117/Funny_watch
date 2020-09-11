#include "stm32f030x6.h"
#include "stm32f0xx.h"
#include "main.h"
#include "variables.h"
#include "functions.h"

struct Variables_TypeDef v;

/*##############################################################################
# МИКРОПРОЦЕССОР: stm32f030f4
# ОПИСАНИЕ НАЗНАЧЕНИЯ ПРОГРАММЫ ЗДЕСЬ
#
##############################################################################*/

/*
пины:
PA0 - /HOLD
PA1 - /CS
PA5 - SPI SCK
PA6 - SPI MISO
PA7 - SPI MOSI

PA2 - USART TX
PA3 - USART RX

управление идет блоками команд
блок записывается в буфер buf длиной 259 байт и состоит из
2 байта - длина блока
1 байт - служебная команда
0..255 байт - данные
1 байт - CRC

команды которыми отвечает программатор
0xAA - ACK - подтверждение получение байта данных, ожидание следующего
             служит для синхронизации потока
0xAB - FAIL - блок данных не принят (неверное CRC, не все данные получены и т.п.)
0xAC - COMPLETE - блок данных принят
0xAD - BUSY - обработка блока
0xAE - WAIT - свободен для выполнения
*/

#define MEM1CsOn  (GPIOA->BSRR = GPIO_BSRR_BR_1)
#define MEM1CsOff (GPIOA->BSRR = GPIO_BSRR_BS_1)
#define MEM1HoldOn  (GPIOA->BSRR = GPIO_BSRR_BR_0)
#define MEM1HoldOff (GPIOA->BSRR = GPIO_BSRR_BS_0)


int main()
{
  // Включить прерывания
  __enable_irq();
  
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
  
  // GPIO
  SET_BIT(GPIOA->MODER, (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1 |
                         GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1 |
                         GPIO_MODER_MODE1_0 | GPIO_MODER_MODER0_0));

  SET_BIT(GPIOA->OSPEEDR, (GPIO_OSPEEDR_OSPEEDR5 | GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR7 |
                           GPIO_OSPEEDR_OSPEEDR2 | GPIO_OSPEEDR_OSPEEDR3 |
                           GPIO_OSPEEDR_OSPEEDR1 | GPIO_OSPEEDR_OSPEEDR0)); 
  
  GPIOA->AFR[0] = 0x1100; //номер альт.функции USART1 - AF01, SPI - AF00
  
  //--- конфигурируем SPI
  SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI; //Master mode, Fpclk/2 = 48/2
  SPI1->CR2 |= SPI_CR2_FRXTH; //8bit FIFO buffer, 8bit data size, NSS pin disabled, no interrupts
  
  //--- конфигурируем USART1 (full-duplex, 9600bps ,8bit, 1stop bit, no parity, no hardware flow control)
  v.udata_get = false;
  USART1->BRR = 0x1A1; //115250
  USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE; //interrupt enable, Rx/Tx enable, UART enable 
  
  //--- включаем прерывания
  NVIC_EnableIRQ(USART1_IRQn);
  
  // начальные значения
  state = WAIT;
  isData = FALSE;
  dataFlow = FALSE;
  
  while (1)
  {
    while (!isData);
    
    if (!dataFlow)
    {
      switch (udata)
      {
        // Manufacturer ID
        case 0x01:
          ACK();
          
      }
    }
  }
}

//=== NVIC функции =============================================================
void USART1_IRQHandler()
{
  if ((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
  {
      udata = (uint8_t)(USART1->RDR); /* Receive data, clear flag */
      isData_get = TRUE;
  }
}

//=== функции ==================================================================
/* отправляет один байт данных в UART */
void usart(uint8_t data)
{
  while ((USART1->ISR & USART_ISR_TC) != USART_ISR_TC);
  USART1->TDR = data;
}

/* отправляет один байт данных в SPI */
uint8_t spi(uint8_t data)
{
  *(uint8_t *)&(SPI1->DR) = (uint8_t)data;
  while ((SPI1->SR & SPI_SR_RXNE) != SPI_SR_RXNE);
  return SPI1->DR;
}

/* подтверждение получения */
void ACK()
{
  usart(0xAA);
}

/* включение режима записи */
void writeEnable()
{
  MEM2CsOn;
    spi(0x06);
  MEM2CsOff;   
}

/* считывание статусного регистра 1 */
uint8_t readR1()
{
  uint8_t c;
  MEM1CsOn;
    spi(0x05);
    c = spi(0xFF);
  MEM1CsOff;
  *((uint8_t*)&reg1[0]) = c;
  return c;
}

/* очистка чипа */
void eraseChip()
{
  MEM2CsOn;
    spi(0xC7);
  MEM2CsOff;
  
  waitBusy();
}

/* ожидание окончания режима записи */
void waitBusy()
{
  MEM2CsOn;
    spi(0x05);
    while ( (spi(0xFF) & 0x01) == 0x01);
  MEM2CsOff;  
}