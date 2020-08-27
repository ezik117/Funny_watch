#include "stm32f030x6.h"
#include "stm32f0xx.h"
#include "images.h"
#include "main.h"

#define BackGroundColor 0x6B4D

//------ ���������� ------------------------------------------------------------
volatile struct
{
  uint8_t TimeChanged : 1;
  uint8_t isTouched : 1;
} flags;

struct
{
  uint8_t i; //��������� ���������� ��� ������ � �.�.
  uint8_t j; //��������� ���������� ��� ������ � �.�.
  volatile uint32_t DelayCounter; //��� SysTick
    
  uint32_t mCalendar;
  uint32_t mLastCalendar;
  
  uint8_t tXX[5]; //������ ���������� ������� X. �������� tXX[1] �������� �������
  uint8_t tYY[5]; //������ ���������� ������� Y. �������� tYY[1] �������� �������
  uint8_t tIterations; //���-�� ���������� � ������ � ������ ������� ������
  uint8_t tCompareMatch;
  
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
  SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOAEN); //�������� ������������ GPIO PORT A
  RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN); //�������� ������������ SPI1
  RCC->APB1ENR |= RCC_APB1ENR_PWREN; //�������� ������������ ���������� power
  for (v.i=0; v.i<100; v.i++); //��������� ���� ��� �� �������� ������������
  //---  
  
  //--- �������� RTC
  PWR->CR |= PWR_CR_DBP; //�������� ������ � �������� RCC->BDCR
    RCC->CSR |= RCC_CSR_LSION; //�������� ���������� �������� ��������� 40���
    while((RCC->CSR &RCC_CSR_LSIRDY) == 0); //��������� ��������� LSI 
    RCC->BDCR |= RCC_BDCR_RTCSEL_LSI | RCC_BDCR_RTCEN; //LSI source, RTC on
  //PWR->CR &= ~PWR_CR_DBP; //��������� ������ � �������� RCC->BDCR
  //---

  //--- ����������� ����
  GPIOA->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | 
                   GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | 
                   GPIO_MODER_MODER7_1 | GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0);
  GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR0 | GPIO_OSPEEDR_OSPEEDR1 | GPIO_OSPEEDR_OSPEEDR2 |
                     GPIO_OSPEEDR_OSPEEDR3 | GPIO_OSPEEDR_OSPEEDR4 | GPIO_OSPEEDR_OSPEEDR5 |
                     GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR7 | GPIO_OSPEEDR_OSPEEDR9 |
                     GPIO_OSPEEDR_OSPEEDR10);
  //GPIOA->PUPDR |= GPIO_PUPDR_PUPDR3_0;
  
  //--- ������������� SPI
  SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE; //Master mode, Fpclk/2 = 48/2, SPI on
  SPI1->CR2 |= SPI_CR2_FRXTH; //8bit FIFO buffer, 8bit data size, NSS pin disabled, no interrupts
  
  //--- ��������� SysTick (1�������)
  SysTick->LOAD = (48000UL & SysTick_LOAD_RELOAD_Msk) - 1;
  SysTick->VAL = 0; //���������� ������� ��� ���������
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | //�������� ������������ SYSCLK
                   SysTick_CTRL_TICKINT_Msk   | //����������� ���������� �� ���������� ����
                   SysTick_CTRL_ENABLE_Msk;     //�������� ������� 
  
  //--- ��������� �������� � ���������� ---
  EXTI->IMR |= EXTI_IMR_IM17; //17�� ������� ���������� ��� Alarm A
  EXTI->RTSR |= EXTI_RTSR_RT17;
  NVIC_EnableIRQ(RTC_IRQn);
  NVIC_SetPriority(SysTick_IRQn, 0); //systick ����� ������ ���������
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
  
  //--- ��������� RTC ---
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
  
  
  //������ ���
  LCD_CS(CS_ENABLE);
  LCD_SetWindow(0,319, 0,239);
  LCD_FillBackground(BackGroundColor);//(0x4228);
   
  //����� ��������
  Time_Show(1);
  
  LCD_ShowImage16FromMem(0*80, 190, memMap[11], 0x00);
  LCD_ShowImage16FromMem(1*80, 190, memMap[12], 0x00);
  LCD_ShowImage16FromMem(2*80, 190, memMap[13], 0x00);
  LCD_ShowImage16FromMem(3*80, 190, memMap[14], 0x00);
 
  while (true)
  {
    Time_Show(0);  
    
    //�������� ��� �� ������� �� �����
    if (TOUCH_PRESSED && (!flags.isTouched))
    {
#define T_MAX 5
      //��������� ������ SPI Rx Tx
      while ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE) v.i = SPI1->DR;
      while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY);
      
      //������� ������� �� ���������� ������� ������ � TouchScr
      SPI1->CR1 |= SPI_CR1_BR_2; //set 1,5MHz [100(48/32=1,5MHz) 000(48/2=24MHz)]       
      TOUCH_CS(CS_ENABLE);
      
      //��������� ��������
      SPI_Exchange(0xD8); //read X
      v.tXX[3] =  (SPI_Exchange(0x00) << 1);
      v.tXX[3] |= ((SPI_Exchange(0x00) & 0x80) >> 7);
        SPI_Exchange(0x98); //read Y     
        v.tYY[3] =  (SPI_Exchange(0x00) << 1);
        v.tYY[3] |= ((SPI_Exchange(0x00) & 0x80) >> 7);
        
       //������� ������� SPI ������� � ����������� �� TouchScr
      SPI1->CR1 ^= SPI_CR1_BR_2; //set 24Mhz
      TOUCH_CS(CS_DISABLE); 
        
      //��������� ���� �� ������ �������
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
          //��������� ��������� ������
          v.tIterations++;
          v.tXX[0] = v.tXX[3];
          if (v.tXX[1] < v.tXX[0]) {v.tXX[3]=v.tXX[0]; v.tXX[0]=v.tXX[1]; v.tXX[1]=v.tXX[3];}
          if (v.tXX[2] < v.tXX[0]) {v.tXX[3]=v.tXX[0]; v.tXX[0]=v.tXX[2]; v.tXX[2]=v.tXX[3];}
          if (v.tXX[2] < v.tXX[1]) {v.tXX[3]=v.tXX[1]; v.tXX[1]=v.tXX[2]; v.tXX[2]=v.tXX[3];}
          v.tYY[0] = v.tYY[3];   
          if (v.tYY[1] < v.tYY[0]) {v.tYY[3]=v.tYY[0]; v.tYY[0]=v.tYY[1]; v.tYY[1]=v.tYY[3];}
          if (v.tYY[2] < v.tYY[0]) {v.tYY[3]=v.tYY[0]; v.tYY[0]=v.tYY[2]; v.tYY[2]=v.tYY[3];}
          if (v.tYY[2] < v.tYY[1]) {v.tYY[3]=v.tYY[1]; v.tYY[1]=v.tYY[2]; v.tYY[2]=v.tYY[3];}
          
          if (v.tIterations > 20) //��������� ��������� ���������� ��������� ��� �������
          {
            if (v.tYY[4] == 0) v.tYY[4] = v.tXX[1]; //���������� �� touch ��������
            if (v.tXX[4] == 0) v.tXX[4] = v.tYY[1];
            if ((v.tXX[1] == v.tYY[4]) && (v.tYY[1] == v.tXX[4])) v.tCompareMatch++;
            if (v.tCompareMatch == 40) //������������ �������
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
    
    //-- ������ ��������� ������� -----
    if (flags.isTouched)
    {
        //������ �� ����� ����������
        uint16_t bcd = ConvertToBCD(v.tXX[4]); //show X
        LCD_ShowImage2FromRom(0,0,  ((bcd>>8)&0x0F), LCD_WHITE);
        LCD_ShowImage2FromRom(8,0,  ((bcd>>4)&0x0F), LCD_WHITE);
        LCD_ShowImage2FromRom(16,0, ((bcd>>0)&0x0F), LCD_WHITE);

        bcd = ConvertToBCD(v.tYY[4]); //show Y
        LCD_ShowImage2FromRom(0,8,  ((bcd>>8)&0x0F), LCD_WHITE);
        LCD_ShowImage2FromRom(8,8,  ((bcd>>4)&0x0F), LCD_WHITE);
        LCD_ShowImage2FromRom(16,8, ((bcd>>0)&0x0F), LCD_WHITE); 
        
        //� ����� �����
        //y [70..0] - �������������� ����� ������ ����������
        //y [180..75] - �������������� ����� ����
        //x [70..0] - ������ "+"
        //x [123..71] - ������ "-"
        //� [178..124] - ������ "��"
        //x [255..179] - ������ "CANCEL"
        //-- ��������� ����� �����
        if (v.tYY[4] < 71)
        {
          //����� ������ ����������
          v.j = 1;
          if (v.tXX[4] < 71)
          {
            //������ "+"
            v.i = 4;
          }
          else
          {
            if (v.tXX[4] < 124)
            {
              //������ "-"
              v.i = 3;
            }
            else
            {
              if (v.tXX[4] < 179)
              {
                //������ "��"
                v.i = 2;
              }
              else
              {
                //������ "CANCEL"
                v.i = 1;
              }
            }
          }
        }
        else
        {
          if (v.tYY[4] < 181)
          {
            //����� ����
            v.j = 2;
          }
        }
        
        LCD_ShowImage2FromRom(00,16, v.j, LCD_YELLOW); 
        LCD_ShowImage2FromRom(16,16, v.i, LCD_YELLOW);
    }
    
    
  } //while (true)
}

//=== NVIC ������� =============================================================
/* ������� ��� ������� Delay */
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

//=== ������� ==================================================================
/* ����������� �������� � �� */
void Delay(uint32_t msTime)
{	
      v.DelayCounter = msTime;
      while (v.DelayCounter);
}

//------------------------------------------------------------------------------
/* ������������� ������� */
void LCD_Init()
{
  SPI_WAIT_TX_READY; //���� ���������� ����� ������� �� ������
  
  //hardware reset
  LCD_RESET(RST_HI);
  Delay(5);
  LCD_RESET(RST_LO); //�������� - ������
  Delay(20);
  LCD_RESET(RST_HI);
  Delay(150); 
  
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
void LCD_ShowImage16FromMem(uint16_t x, uint8_t y, uint32_t address, uint8_t mask)
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
  for (uint8_t i=0; i<4; i++)
  {
    c = SPI_Exchange(0xFF);
    *(&BmpHeader16.width + i)  = c;
  }
  
  //��������� ���� ������ LCD
  MEM_PAUSE;
  LCD_CS(CS_ENABLE);
  LCD_SetWindow(x, x+BmpHeader16.width-1, y, y+BmpHeader16.height-1);
  LCD_CS(CS_DISABLE);
  MEM_RESUME;
  
  while ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE) c = SPI1->DR; //�������� Rx �����
  while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY);
  
  //----- ���������� ����� -----
  c = SPI_Exchange(0x00) ^ mask; //������� 1 ����
  LCD_CS(CS_ENABLE);
  
  for (uint32_t count=0; count < BmpHeader16.length; count++)
  {
	  c = SPI_Exchange(c) ^ mask; //��������� ���������� � ������� ��������� ����	  
  }
  
  LCD_CS(CS_DISABLE);
  MEM_CS(CS_DISABLE);
}

//------------------------------------------------------------------------------
/* ����� �������� �� ROM */
void LCD_ShowImage2FromRom(uint16_t x, uint8_t y, uint8_t imgIdx, uint16_t color)
{
  uint8_t c;
  
  //��������� ����� ������ (1) � �������� ��� �������� (2)
  while ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE) c = SPI1->DR; //1
  while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY); //2
  
  //��������� ���� ������ LCD
  LCD_CS(CS_ENABLE);
  LCD_SetWindow(x, x+7, y, y+6);
  
  //����� �����������
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
/* ����������� �������, updateAll ������������� ��� ������� ��������� */
/* ����� ������ �� ����� ������������ �������, ��������� ���������� v.TimeStored */
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