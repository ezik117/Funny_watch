#include "stm32f030x6.h"
#include "stm32f0xx.h"
#include "images.h"
#include "main.h"

#define true 1
#define false 0
#define bool uint8_t


//------ переменные ------------------------------------------------------------
struct
{
  uint8_t i; //временная переменная для циклов и т.п.
  uint8_t j; //временная переменная для циклов и т.п.
  uint16_t z; //временная переменная для циклов и т.п.
  volatile uint32_t DelayCounter; //для SysTick
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
  for (v.i=0; v.i<10; v.i++); //небольшой цикл что бы включить тактирование
  //---  

  //--- настраиваем пины
  GPIOA->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER4_0 | 
                   GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1 |
                   GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0);
  GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR0 | GPIO_OSPEEDR_OSPEEDR1 | GPIO_OSPEEDR_OSPEEDR4 |
                     GPIO_OSPEEDR_OSPEEDR5 | GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR7 |
                     GPIO_OSPEEDR_OSPEEDR9 | GPIO_OSPEEDR_OSPEEDR10);
  
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
  NVIC_SetPriority(SysTick_IRQn, 0); //systick имеет высший приоритет
  __enable_irq(); //включить все прерывания
  
  MEM_CS(CS_DISABLE);
  MEM_RESUME;
  LCD_CS(CS_DISABLE);  
  
  LCD_Init();
  
  //задний фон
  LCD_CS(CS_ENABLE);
  LCD_SetWindow(0,319, 0,239);
  LCD_FillBackground(LCD_GREY);
  
  
  //вывод картинки
  LCD_ShowImage16FromMem(0,0, 0x0000);
  
  uint8_t direction = 0;
  uint8_t ax = 0;
  uint8_t ay = 0;
  uint8_t speed  = 1;
  while (true)
  {  
    LCD_ShowImage16FromMem(ax,ay, 0x0000);
    
    switch (direction)
    {
    case 0:   
      if (ax < (320-130-1))
          ax += speed;
      else
          direction++;
      break;

    case 1:
      if (ay<(240-106-1))
          ay += speed;
      else
          direction++;
      break;
      
    case 2:
      if (ax > 0)
        ax -= speed;
      else
        direction++;
      break;
      
    case 3:
      if (ay > 0)
        ay -= speed;
      else
      {
        direction = 0;
        speed++;
        if (speed == 3) speed = 1;
      }
      break;
      
    default:
      break;
    }
  }
}

//=== NVIC функции =============================================================
/* счетчик для функции Delay */
void SysTick_Handler()
{
 if (v.DelayCounter != 0) v.DelayCounter--;
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
void LCD_ShowImage16FromMem(uint16_t x, uint8_t y, uint32_t address)
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
  
  //----- потоковыый обмен -----
  c = SPI_Exchange(0x00); //считали 1 байт
  LCD_CS(CS_ENABLE);
  
  for (uint32_t count=0; count < BmpHeader16.length; count++)
  {
	  c = SPI_Exchange(c); //отправили предыдущий и считали следующий байт	  
  }
  
  LCD_CS(CS_DISABLE);
  MEM_CS(CS_DISABLE);
}