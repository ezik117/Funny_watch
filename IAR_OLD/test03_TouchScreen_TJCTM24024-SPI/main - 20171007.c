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
#define LCD_CS_0  (GPIOA->BSRR = GPIO_BSRR_BR_4) //select slave
#define LCD_CS_1  (GPIOA->BSRR = GPIO_BSRR_BS_4) //deselect slave

#define LCD_BLACK        0x0000
#define LCD_BLUE         0x001F
#define LCD_RED          0xF800
#define LCD_GREEN        0x07E0
#define LCD_CYAN         0x07FF
#define LCD_MAGENTA      0xF81F
#define LCD_YELLOW       0xFFE0  
#define LCD_WHITE        0xFFFF

//--- объявления функций -------------------------------------------------------
void LCD_SendCommand(uint8_t command);
void LCD_SendData(uint8_t data);
void LCD_Init();
void LCD_FillBackground(uint16_t color);
uint8_t SPI_Data(uint8_t data);
void LCD_SetWindow(uint32_t columns, uint32_t rows);
void LCD_SetAtPos(uint8_t X, uint8_t Y, uint8_t size);
void LCD_ShowChar(uint8_t* pdata, uint8_t XBytes, uint8_t YLines, uint16_t fcolor, uint16_t bgcolor);

//--- объявления переменных ----------------------------------------------------
struct {
  volatile int32_t DelayCounter; //для SysTick
  volatile int8_t seconds;
  volatile int8_t minutes;
} v;

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
  __enable_irq();
  
  //--- перевести MCU на тактовую частоту 48МГц ---
  RCC->CFGR =RCC->CFGR & (~RCC_CFGR_PLLMUL) | (RCC_CFGR_PLLMUL12); //задать множитель PLL x12 (48MHz)
  RCC->CR |=RCC_CR_PLLON; //включить PLL
  while((RCC->CR &RCC_CR_PLLRDY) == 0); //дождаться включения PLL
  
  RCC->CFGR |= (uint32_t) (RCC_CFGR_SW_PLL); //выбрать PLL источником SYSCLK
  while ((RCC->CFGR &RCC_CFGR_SWS) !=RCC_CFGR_SWS_PLL); //дожаться пока PLL не включится как источник SYSCLK
  
  //--- включить тактирование периферии 
  SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOAEN); //включить тактирование GPIO PORT A
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //включить тактирование SPI1
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; //включить тактирование TIM3
  //RCC->BDCR |= RCC_BDCR_RTCSEL_LSI | RCC_BDCR_RTCEN; //LSI source, RTC on
  
  //--- настраиваем пины 
  GPIOA->MODER |= (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1 |
                   GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0); //PA5,6,7-альтернативная функция AF0(SPI) + PA9,10-output
  GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR5 | GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR7 |
                     GPIO_OSPEEDR_OSPEEDR9 | GPIO_OSPEEDR_OSPEEDR10); //all - high speed
  GPIOA->PUPDR |= GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR10_0; //PA9,10-pull up
  GPIOA->OTYPER |= GPIO_OTYPER_OT_0; //As MISO pins of the slaves are connected together, all slaves must have the GPIO configuration
                                     //of their MISO pin set as alternate function open-drain
  
  //--- конфигурируем SPI1  
  SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_BR | SPI_CR1_SSM | SPI_CR1_SSI /* | SPI_CR1_BR_0*/; //Master mode, Fpclk/2 = 48/2
  SPI1->CR2 = SPI_CR2_FRXTH | SPI_CR2_DS_2 |SPI_CR2_DS_1 |SPI_CR2_DS_0; //8bit FIFO buffer, no interrupts
  SPI1->CR1 |=SPI_CR1_SPE; //enable SPI  

  //--- настройка PA4: выход, push-pull, high speed, push-up
  SET_BIT(GPIOA->MODER, GPIO_MODER_MODER4_0);
  CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT_4);
  SET_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPDR4_0);
  SET_BIT(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEEDR4);  
  LCD_CS_1;

  //--- настройка SysTick (1милисек)
  SysTick->LOAD = (48000UL & SysTick_LOAD_RELOAD_Msk) - 1;
  SysTick->VAL = 0; //необходимо вызвать для обнуления
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | //источник тактирования SYSCLK
                   SysTick_CTRL_TICKINT_Msk   | //запрашивать прерывание по достижению нуля
                   SysTick_CTRL_ENABLE_Msk;     //включить счетчик 
  
  //--- настройка таймера TIM3
  TIM3->PSC = 47999; //1ms
  TIM3->ARR = 1000; //1s
  NVIC_EnableIRQ(TIM3_IRQn_IRQn);
  TIM3->DIER |= TIM_DIER_UIE;
  v.seconds = v.minutes = 0;
  
  //--- инициализации
  LCD_Init();
  
  LCD_SetWindow(0x0000013F, 0x000000EF);
  LCD_FillBackground((uint16_t)LCD_BLACK);
  LCD_SetAtPos(3, 4, 24);
  LCD_ShowChar((uint8_t*)&D24[0*72], 3, 24, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);
  LCD_SetAtPos(4, 4, 24);
  LCD_ShowChar((uint8_t*)&D24[0*72], 3, 24, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);    
  
  LCD_SetAtPos(6, 4, 24);
  LCD_ShowChar((uint8_t*)&D24[0*72], 3, 24, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK); 
  LCD_SetAtPos(7, 4, 24);
  LCD_ShowChar((uint8_t*)&D24[0*72], 3, 24, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);   
  
  LCD_SetAtPos(9, 4, 24);
  LCD_ShowChar((uint8_t*)&D24[0*72], 3, 24, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK); 
  LCD_SetAtPos(10, 4, 24);
  LCD_ShowChar((uint8_t*)&D24[0*72], 3, 24, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);

  TIM3->CR1 |= TIM_CR1_CEN;
  
  while (1)
  {

    
//    LCD_SetAtPos(3, 1, 24);
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
  if (v.seconds == 59)
  {
    v.seconds = 0;
    v.minutes++;
    LCD_SetAtPos(6, 4, 24);
    LCD_ShowChar((uint8_t*)&D24[(uint8_t)(v.minutes/10)*72], 3, 24, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);   
    LCD_SetAtPos(7, 4, 24);
    LCD_ShowChar((uint8_t*)&D24[(v.minutes % 10)*72], 3, 24, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);         
  }
  else
  {
      v.seconds++;
  }
  
  //вывести секунды
  LCD_SetAtPos(9, 4, 24);
  LCD_ShowChar((uint8_t*)&D24[(uint8_t)(v.seconds/10)*72], 3, 24, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);   
  LCD_SetAtPos(10, 4, 24);
  LCD_ShowChar((uint8_t*)&D24[(v.seconds % 10)*72], 3, 24, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);  
  
  TIM3->SR &=~ TIM_SR_UIF; //сбрасываем флаг прерывания
}

//=== функции ==================================================================
void Delay(uint32_t msTime)
{	
      v.DelayCounter = msTime;
      while (v.DelayCounter);
}

//------------------------------------------------------------------------------
void LCD_SendCommand(uint8_t command)
{
  GPIOA->BSRR = GPIO_BSRR_BR_9; //DCX = 0
  SPI_Data(command);
}

//------------------------------------------------------------------------------
void LCD_SendData(uint8_t data)
{
  GPIOA->BSRR = GPIO_BSRR_BS_9; //DCX = 1
  SPI_Data(data);
}

//------------------------------------------------------------------------------
void LCD_Init()
{
  //hardware reset
  LCD_CS_1;
  LCD_RST_1;
  Delay(5);
  LCD_RST_0; //активный - низкий
  Delay(20);
  LCD_RST_1;
  Delay(150); 
  LCD_CS_0;
  
  //set up driver parameters
  uint32_t i,j,len, pos=0;
  for (i=0; i<19; i++) //строки массива
  {
    LCD_SendCommand(LcdInitData[pos++]);
    len = LcdInitData[pos++];
    for (j=0; j<len; j++)
    {
      LCD_SendData(LcdInitData[pos++]);
    }
  }
  
  Delay(120);
  LCD_SendCommand(0x29); //Display on
  LCD_SendCommand(0x2C); //Write registers
}
                   
//------------------------------------------------------------------------------
void LCD_FillBackground(uint16_t color)
{
  uint32_t t;
  for (t=0; t<(240*320); t++)
  {
    LCD_SendData((uint8_t)(color>>8));
    LCD_SendData((uint8_t)(color));
  }
}

//------------------------------------------------------------------------------
void LCD_SetWindow(uint32_t columns, uint32_t rows)
{
  //установить границы окна рисования
  LCD_SendCommand(0x2A);
  LCD_SendData((uint8_t)(columns>>24));
  LCD_SendData((uint8_t)(columns>>16)); //SC = column start
  LCD_SendData((uint8_t)(columns>>8));
  LCD_SendData((uint8_t)(columns)); //EC = column end  
  LCD_SendCommand(0x2B);
  LCD_SendData((uint8_t)(rows>>24));
  LCD_SendData((uint8_t)(rows>>16));
  LCD_SendData((uint8_t)(rows>>8));
  LCD_SendData((uint8_t)(rows));

  LCD_SendCommand(0x2C); //Write registers
}

//------------------------------------------------------------------------------
void LCD_SetAtPos(uint8_t X, uint8_t Y, uint8_t size)
{
  LCD_SetWindow( (uint32_t)(((X*size)<<16) | ((X+1)*size-1)), (uint32_t)((Y*size)<<16) | ((Y+1)*size-1) );
}

//------------------------------------------------------------------------------
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
          LCD_SendData((uint8_t)(fcolor>>8));
          LCD_SendData((uint8_t)(fcolor));
        }
        else
        {
          LCD_SendData((uint8_t)(bgcolor>>8));
          LCD_SendData((uint8_t)(bgcolor));
        }
      } 
    }
  }
}

//------------------------------------------------------------------------------
uint8_t SPI_Data(uint8_t data)
{
  while ((SPI1->SR & SPI_SR_TXE) != SPI_SR_TXE); //ждем пока можно будет отправлять
  *(uint8_t *)&(SPI1->DR) = data; //отправляем
  while ((SPI1->SR & SPI_SR_RXNE) != SPI_SR_RXNE); //ждем пока цикл передачи не будет закончен
  return (uint8_t)SPI1->DR; //читаем буфер
}