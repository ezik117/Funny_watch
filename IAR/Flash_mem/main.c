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

struct
{
  volatile uint8_t udata; //data read from USART1 by interrupt routine
  volatile bool udata_get; //flag set to TRUE when a new data arrived from USART1
  bool isCommand;
  uint8_t temp8;
} var;

int main()
{
  // Включить прерывания
  __enable_irq();
  
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PLLMUL_Msk, RCC_CFGR_PLLMUL12); //задать множитель PLL x12 (HSI/2*12=48MHz)
  SET_BIT(RCC->CR, RCC_CR_PLLON); //включить PLL
  while((RCC->CR & RCC_CR_PLLRDY) == 0); //дождаться включения PLL
    
  SET_BIT(RCC->CFGR, (uint32_t)RCC_CFGR_SW_PLL); //выбрать PLL источником SYSCLK
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); //дождаться пока PLL не включится как источник SYSCLK
  
  // Реализация функции Delay (тактовая частота SYSCLK = 8МГц)
  SysTick->LOAD = (48000UL & SysTick_LOAD_RELOAD_Msk) - 1;
  SysTick->VAL = 0;
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
  
  // Clocking
  SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOAEN); // GPIOA
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN); // USART
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN); // SPI
  
  // GPIO
  GPIOA->MODER |= (GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1); // usart
  GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR2 | GPIO_OSPEEDR_OSPEEDR3); //usart
  GPIOA->AFR[0] = 0x1100; //номер альт.функции USART1 - AF01 
  
  GPIOA->MODER |= (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1); //spi
  GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR5 | GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR7); //spi
  
  // SPI configuration
  SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE; //Master mode, Fpclk/2 = 48/2, SPI on
  SPI1->CR2 |= SPI_CR2_FRXTH; //8bit FIFO buffer, 8bit data size, NSS pin disabled, no interrupts
  
  // USART configuration
  //full-duplex, 9600bps ,8bit, 1stop bit, no parity, no hardware flow control
  var.udata_get = false;
  USART1->BRR = 0x1388; //9600
  USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE; //interrupt enable, Rx/Tx enable, UART enable 
  
  var.isCommand = true;
  
  NVIC_EnableIRQ(USART1_IRQn);
  
  while (1)
  {
    processUsart();
  }
}

//=== NVIC функции =============================================================

void USART1_IRQHandler()
{
  if ((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
  {
      var.udata = (uint8_t)(USART1->RDR); /* Receive data, clear flag */
      var.udata_get = true;
  }
}

// Прерывание для реализации функции Delay -------------------------------------
void SysTick_Handler()
{
  if (v.DelayCounter != 0) v.DelayCounter--;
}

//=== ФУНКЦИИ ==================================================================

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
void processUsart()
{
    //проверка получения команды из USART1
    if (var.udata_get)
    {
      //обработка команды
      if (var.isCommand)
      {
        switch (var.udata)
        {
          case 0xAA: //next byte is SPI data
            var.isCommand = false;
            break;
          case 10: //CS=0
            MEM_CS(CS_ENABLE);
            break;
          case 11: //CS=1
            MEM_CS(CS_DISABLE);
            break;          
          case 20: //HOLD=0
            MEM_HOLD(MEM_HOLD_ON);
            break;
          case 21: //HOLD=1
            MEM_HOLD(MEM_HOLD_OFF);
            break; 
          default:
            break;
        }
      }
      //обработка данных для SPI
      else
      {
        var.temp8 = SendSPI(var.udata);
        var.isCommand = true;
      }
      
      //for (uint32_t t=0; t<10; t++) {};
      //отправка команды назад как сигнал подтверждения
      SendUData(var.udata);
      for (uint8_t t=0; t<10; t++) {};
      //SendUData(var.temp8);
      var.udata_get = false;
    }
}