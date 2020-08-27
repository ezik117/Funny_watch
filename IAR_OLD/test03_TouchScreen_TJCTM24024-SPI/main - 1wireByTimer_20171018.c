#include "stm32f030x6.h"
#include "stm32f0xx.h"
#include "main.h"

#define true 1
#define false 0
#define bool uint8_t

/*##############################################################################
# ��������������: STM32F030F4P6
# ������ ������ ���������� ������� ��������� SPI ����������
# � ������������ � ��������� ������� TJCTM24024
##############################################################################*/
//SPI:
// LCD_CS       - PA4
// LCD_SPI_MISO - PA6
// LCD_SPI_MOSI - PA7
// LCD_SPI_SCK  - PA5
// LCD_RST      - PA10
// LCD_DCX      - PA9

//--- ������� ------------------------------------------------------------------
#define LCD_RST_0 (GPIOA->BSRR = GPIO_BSRR_BR_10)
#define LCD_RST_1 (GPIOA->BSRR = GPIO_BSRR_BS_10)
#define LCD_CS_0  (GPIOA->BSRR = GPIO_BSRR_BR_4) //select slave
#define LCD_CS_1  (GPIOA->BSRR = GPIO_BSRR_BS_4) //deselect slave

#define LCD_DATA (GPIOA->BSRR = GPIO_BSRR_BS_9) //DCX = 1
#define LCD_CMD (GPIOA->BSRR = GPIO_BSRR_BR_9) //DCX = 0
#define SPI_WAIT_TX_END while((SPI1->SR & SPI_SR_BSY) != 0) {} //���� ���� ��������� �� ����� ��������� ��������

#define LCD_BLACK        0x0000
#define LCD_BLUE         0x001F
#define LCD_RED          0xF800
#define LCD_GREEN        0x07E0
#define LCD_CYAN         0x07FF
#define LCD_MAGENTA      0xF81F
#define LCD_YELLOW       0xFFE0  
#define LCD_WHITE        0xFFFF

#define OW_Mode_Idle     0
#define OW_Mode_Presence 1
#define OW_Mode_Send     2
#define OW_Mode_Read     3

//--- ���������� ������� -------------------------------------------------------
void Delay(uint32_t msTime);
void LCD_Init();
void LCD_FillBackground(uint16_t color);
void SPI_DataStream(uint8_t data);
void LCD_SetWindow(uint32_t columns, uint32_t rows);
void LCD_SetAtPos(uint8_t X, uint8_t Y, uint8_t size);
void LCD_ShowChar(uint8_t* pdata, uint8_t XBytes, uint8_t YLines, uint16_t fcolor, uint16_t bgcolor);
void LCD_ShowText(uint8_t* text, uint8_t XPos, uint8_t YPos, int8_t YOffset, uint8_t WChar, uint8_t HChar, uint16_t fcolor, uint16_t bgcolor);
uint8_t* Calendar_FindDayNameByNumber(uint8_t NDay, uint8_t* array);

//--- ���������� ���������� ----------------------------------------------------
struct {
  volatile uint32_t DelayCounter; //��� SysTick
  volatile uint32_t RTC_TR_last;
  volatile uint32_t RTC_DR_last;
} v;

struct { //��������� ���������� 1wire 
  volatile uint8_t Start; //=1 - ��������� ���������, ��������� ������������� ����� ��� 0 ���������� � 0  
  volatile uint8_t RetVal; //��������� �������� �� �����, ���� ��������� �������� ���������� ������
  volatile uint8_t* Program; //��������� �� ���������
  volatile uint8_t _mode;
  volatile uint16_t _cycles;
  volatile uint16_t _bitsLo;
  volatile uint16_t _bitsHi;
} ow;

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
                                      0x36,1, MV | MX | MY | BGR, //Memory access control:������ ����, ����� �������, ��������������
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
  
  //--- ��������� MCU �� �������� ������� 48��� ---
  RCC->CFGR =RCC->CFGR & (~RCC_CFGR_PLLMUL) | (RCC_CFGR_PLLMUL12); //������ ��������� PLL x12 (48MHz)
  RCC->CR |=RCC_CR_PLLON; //�������� PLL
  while((RCC->CR &RCC_CR_PLLRDY) == 0); //��������� ��������� PLL
  
  RCC->CFGR |= (uint32_t) (RCC_CFGR_SW_PLL); //������� PLL ���������� SYSCLK
  while ((RCC->CFGR &RCC_CFGR_SWS) !=RCC_CFGR_SWS_PLL); //�������� ���� PLL �� ��������� ��� �������� SYSCLK
  
  //--- �������� ������������ ��������� 
  SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOAEN); //�������� ������������ GPIO PORT A
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //�������� ������������ SPI1
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM14EN; //�������� ������������ TIM3, TIM14
  //�������� RTC
  RCC->CSR |= RCC_CSR_LSION; //�������� ���������� �������� ��������� 40���
  while((RCC->CSR &RCC_CSR_LSIRDY) == 0); //��������� ��������� PLL
  RCC->APB1ENR |= RCC_APB1ENR_PWREN; //�������� ������������ ���������� power
  PWR->CR |= PWR_CR_DBP; //�������� ������ � �������� RCC->BDCR
  RCC->BDCR |= RCC_BDCR_RTCSEL_LSI | RCC_BDCR_RTCEN; //LSI source, RTC on
  while((RCC->BDCR &RCC_BDCR_RTCEN) == 0); //��������� ��������� RTC
  PWR->CR &= ~PWR_CR_DBP; //��������� ������ � �������� RCC->BDCR
  
  //--- ����������� ���� 
  GPIOA->MODER |= (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1 |
                   GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0); //PA5,6,7-�������������� ������� AF0(SPI) + PA9,10-output
  GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR5 | GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR7 |
                     GPIO_OSPEEDR_OSPEEDR9 | GPIO_OSPEEDR_OSPEEDR10); //all - high speed
  GPIOA->PUPDR |= GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR10_0; //PA9,10-pull up
  GPIOA->OTYPER |= GPIO_OTYPER_OT_0; //As MISO pins of the slaves are connected together, all slaves must have the GPIO configuration
                                     //of their MISO pin set as alternate function open-drain
  
  //�������� ��� PA3
  GPIOA->MODER |= GPIO_MODER_MODER3_0; //in
  GPIOA->OTYPER |= GPIO_OTYPER_OT_3; //open-drain
  GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR3; //hi speed
  GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR3_Msk; //no pull-up, pull-down
  GPIOA->BSRR = GPIO_BSRR_BS_3; //free line
  
//  volatile static uint32_t b;
//  GPIOA->BSRR = GPIO_BSRR_BS_3; //free
//  b = (GPIOA->IDR & GPIO_IDR_3) >> 3;
//  GPIOA->BSRR = GPIO_BSRR_BR_3; //to GND
//  b = (GPIOA->IDR & GPIO_IDR_3) >> 3;
  
  
  //--- ������������� SPI1  
  SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI /* | SPI_CR1_BR_0*/; //Master mode, Fpclk/2 = 48/2
  SPI1->CR2 = SPI_CR2_FRXTH | SPI_CR2_DS_2 |SPI_CR2_DS_1 |SPI_CR2_DS_0; //8bit FIFO buffer, no interrupts
  SPI1->CR1 |=SPI_CR1_SPE; //enable SPI  

  //--- ��������� PA4: �����, push-pull, high speed, push-up
  SET_BIT(GPIOA->MODER, GPIO_MODER_MODER4_0);
  CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT_4);
  SET_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPDR4_0);
  SET_BIT(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEEDR4);  
  LCD_CS_1;

  //--- ��������� SysTick (1�������)
  SysTick->LOAD = (48000UL & SysTick_LOAD_RELOAD_Msk) - 1;
  SysTick->VAL = 0; //���������� ������� ��� ���������
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | //�������� ������������ SYSCLK
                   SysTick_CTRL_TICKINT_Msk   | //����������� ���������� �� ���������� ����
                   SysTick_CTRL_ENABLE_Msk;     //�������� ������� 
  
  //--- ��������� ������� TIM3 (�������� ���������� ������ �������)
  TIM3->PSC = 47; //1ms
  TIM3->ARR = 1; //1s
  NVIC_EnableIRQ(TIM3_IRQn_IRQn);
  TIM3->DIER |= TIM_DIER_UIE;
  
   //--- ��������� ������� TIM14 (������ � 1-wire ����������)
  ow.Start = 0;
  TIM14->PSC = 47; //1us
  TIM14->ARR = 1; //1ms
  NVIC_EnableIRQ(TIM14_IRQn);
  TIM14->DIER |= TIM_DIER_UIE; 
  TIM14->CR1 |= TIM_CR1_CEN; //��������� ������
  
  //1wire. �������� ���������
  ow.Program = (uint8_t*)&OWPROG_PRESENCE[0];
  ow.Start = 1;
  while ((ow.Start & 0x01) == 1);
  volatile uint8_t bb = ow.RetVal;
  
  while (1);
  
  //--- �������������
  LCD_Init();
  
  LCD_SetWindow(0x0000013F, 0x000000EF);
  LCD_FillBackground((uint16_t)LCD_BLACK);
  
  //��������� ������� ������ ��������� �����
  //� ����������� �� ����� �������� �� ���������� �� ��� �� ������� ��� ��
  //�������� �� ����� �� �������� ���������� �� ������
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
  
  //���� ������
  LCD_ShowText(Calendar_FindDayNameByNumber(0, (uint8_t*)&TDAYS[0]), 0, 0, 0, 24, 27, (uint16_t)LCD_WHITE, (uint16_t)LCD_BLACK);
  
  //���������
  LCD_ShowText("2000", 0, 2, 0, 24, 27, (uint16_t)LCD_YELLOW, (uint16_t)LCD_BLACK);
  LCD_ShowText("01-���", 0, 3, 8, 24, 27, (uint16_t)LCD_YELLOW, (uint16_t)LCD_BLACK);
  
  //���������� ��������� �������� ���������, ��������������
  PWR->CR |= PWR_CR_DBP;
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;  
  RTC->ISR |=RTC_ISR_INIT;
  while ((RTC->ISR &RTC_ISR_INITF) !=RTC_ISR_INITF);
    RTC->PRER = 0x007F0137;
    RTC->TR = 1 << RTC_TR_HT_Pos | 8 << RTC_TR_HU_Pos | 0 << RTC_TR_MNT_Pos | 5 << RTC_TR_MNU_Pos;
    RTC->DR = 1 << RTC_DR_YT_Pos | 7 << RTC_DR_YU_Pos | 1 << RTC_DR_MT_Pos | 0 << RTC_DR_MU_Pos |
              1 << RTC_DR_DT_Pos | 4 << RTC_DR_DU_Pos |
              6 << RTC_DR_WDU_Pos;
  RTC->ISR &=~RTC_ISR_INIT;
  RTC->WPR = 0xFE;
  RTC->WPR = 0x64;
  
  TIM3->CR1 |= TIM_CR1_CEN;
  
  while (1)
  {

    
//    LCD_SetAtPos(3, 1, 24);
  }
  

}

//=== NVIC ������� =============================================================
void SysTick_Handler()
{
 if (v.DelayCounter != 0) v.DelayCounter--;
}

//------------------------------------------------------------------------------
void TIM3_IRQHandler()
{
 
  //CCo - sec
  LCD_SetWindow(C1X, C1Y);
  LCD_ShowChar((uint8_t*)&D56X93[651*((RTC->TR & RTC_TR_ST_Msk)>>RTC_TR_ST_Pos)], 7, 93, (uint16_t)LCD_GREEN, (uint16_t)LCD_BLACK);
  LCD_SetWindow(C2X, C2Y);
  LCD_ShowChar((uint8_t*)&D56X93[651*((RTC->TR & RTC_TR_SU_Msk))], 7, 93, (uint16_t)LCD_GREEN, (uint16_t)LCD_BLACK);  

  if (((v.RTC_TR_last ^ RTC->TR) & 0x0000007F) != 0) //���� ��������� ����� ��� ����� ������ ���������� - ������� �������
  {
    //������� ������
    LCD_SetWindow(M1X, M1Y);
    LCD_ShowChar((uint8_t*)&D56X93[651*((RTC->TR & RTC_TR_MNT_Msk)>>RTC_TR_MNT_Pos)], 7, 93, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);
    LCD_SetWindow(M2X, M2Y);
    LCD_ShowChar((uint8_t*)&D56X93[651*((RTC->TR & RTC_TR_MNU_Msk)>>RTC_TR_MNU_Pos)], 7, 93, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);  
    v.RTC_TR_last = RTC->TR;
    //� ����
    LCD_SetWindow(H1X, H1Y);
    LCD_ShowChar((uint8_t*)&D56X93[651*((RTC->TR & RTC_TR_HT_Msk)>>RTC_TR_HT_Pos)], 7, 93, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);
    LCD_SetWindow(H2X, H2Y);
    LCD_ShowChar((uint8_t*)&D56X93[651*((RTC->TR & RTC_TR_HU_Msk)>>RTC_TR_HU_Pos)], 7, 93, (uint16_t)LCD_CYAN, (uint16_t)LCD_BLACK);   
    v.RTC_TR_last = RTC->TR;
  }  
  
  if ((v.RTC_DR_last ^ RTC->DR) != 0) //���� ��������� ���� ���������� - ������� �������
  {
    //���
    LCD_ShowText((uint8_t*)&TDIGITS[2*((RTC->DR & RTC_DR_YT_Msk)>>RTC_DR_YT_Pos)], 2, 2, 0, 24, 27, (uint16_t)LCD_YELLOW, (uint16_t)LCD_BLACK);
    LCD_ShowText((uint8_t*)&TDIGITS[2*((RTC->DR & RTC_DR_YU_Msk)>>RTC_DR_YU_Pos)], 3, 2, 0, 24, 27, (uint16_t)LCD_YELLOW, (uint16_t)LCD_BLACK);
    //����� 
    LCD_ShowText((uint8_t*)&TMONTHS[4* (((0x0A*((RTC->DR & RTC_DR_MT_Msk)>>RTC_DR_MT_Pos)) +
                                               ((RTC->DR & RTC_DR_MU_Msk)>>RTC_DR_MU_Pos)) - 1)
                 ], 3, 3, 8, 24, 27, (uint16_t)LCD_YELLOW, (uint16_t)LCD_BLACK);    
    //����
    LCD_ShowText((uint8_t*)&TDIGITS[2*((RTC->DR & RTC_DR_DT_Msk)>>RTC_DR_DT_Pos)], 0, 3, 8, 24, 27, (uint16_t)LCD_YELLOW, (uint16_t)LCD_BLACK);
    LCD_ShowText((uint8_t*)&TDIGITS[2*((RTC->DR & RTC_DR_DU_Msk)>>RTC_DR_DU_Pos)], 1, 3, 8, 24, 27, (uint16_t)LCD_YELLOW, (uint16_t)LCD_BLACK);
    //���� ������
    LCD_ShowText((uint8_t*)&TDAYS[12*(((RTC->DR & RTC_DR_WDU_Msk)>>RTC_DR_WDU_Pos)-1)], 0, 0, 0, 24, 27, (uint16_t)LCD_WHITE, (uint16_t)LCD_BLACK);
    
    v.RTC_DR_last = RTC->DR;   
  }

  
  TIM3->SR &=~ TIM_SR_UIF; //���������� ���� ����������
}

//------------------------------------------------------------------------------
void TIM14_IRQHandler()
{
  if ((ow.Start & 0x01) == 1) //��������� ��������
  {
    if ((ow.Start & 0x02) == 0) //�������� ���������
    {
      ow._mode = *ow.Program;
      ow.Program++;
      ow._cycles = *ow.Program;
      ow._cycles <<= 8;
      ow.Program++;
      ow._cycles |= *ow.Program;
      ow.Program++;
      if (ow._mode == 3) ow._bitsLo = ow._bitsHi = 0;
      ow.Start |= 0x02; //�������� ���������
    }
    else
    {
      switch (ow._mode)
      {
        case 0: //����� �� ���������
          if (ow._bitsHi > ow._bitsLo)
          {
            ow.RetVal = 1;
          }
          else
          {
            ow.RetVal = 0;
          }
          ow.Start = 0;   
        break;
        
        case 1: //������� ����� � GND
          GPIOA->BSRR = GPIO_BSRR_BR_3;
        break;
        
        case 2: //��������� ����� � VCC
          GPIOA->BSRR = GPIO_BSRR_BS_3;
        break;      
        
        case 3: //��������� ����� � VCC � ������� ������
          GPIOA->BSRR = GPIO_BSRR_BS_3;
          if ((GPIOA->IDR & GPIO_IDR_3) == 0)
          {
            ow._bitsLo++;
          }
          else
          {
            ow._bitsHi++;
          }        
        break;        
      }
      ow._cycles--;
      if (ow._cycles == 0)
      {
        ow.Start &= ~0x02; //������� ��� �������� ���������
      }
    }
  }
  TIM14->SR &=~ TIM_SR_UIF; //���������� ���� ����������
}

//=== ������� ==================================================================
void Delay(uint32_t msTime)
{	
      v.DelayCounter = msTime;
      while (v.DelayCounter);
}

//------------------------------------------------------------------------------
void LCD_Init()
{
  SPI_WAIT_TX_END; //���� ���������� ����� ������� �� ������
  
  //hardware reset
  LCD_CS_1;
  LCD_RST_1;
  Delay(5);
  LCD_RST_0; //�������� - ������
  Delay(20);
  LCD_RST_1;
  Delay(150); 
  LCD_CS_0;
  
  //set up driver parameters
  uint32_t i,j,len, pos=0;
  for (i=0; i<19; i++) //������ �������
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
/* ��������� �������� ����� ������� ��������� ������ */
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
/* ������������� ���� ���������. ����� ������ ������� ����� ���������� ������ �����
   ��� ������������ DX �������.
   columns, rows - 32� ������ ����������, ��� ������ 16 ��� �������� ��������� ��������
   ������ 16 ��� �������� ��������. �������� 0x00010002 - �������� �� ������ �� ������ ������/������� */
void LCD_SetWindow(uint32_t columns, uint32_t rows)
{
  //���������� ������� ���� ���������
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
/* ������ ���� ��������� ��� ������� � ������������� ��������� A �� B, ��� �=B.
   X,Y - ���������� �������� � �������� ����������� ������, size - ������� �������
   (� ��� �) ������ */
void LCD_SetAtPos(uint8_t X, uint8_t Y, uint8_t size)
{
  LCD_SetWindow( (uint32_t)(((X*size)<<16) | ((X+1)*size-1)), (uint32_t)((Y*size)<<16) | ((Y+1)*size-1) );
}

//------------------------------------------------------------------------------
/* ������� ������ � �������� ����� �������.
   pdata - ��������� �� ������ � ������� �������
   XBytes - ���������� ������ ���������� �������������� ����� �������
   YLines - ���������� ������������ ����� ������� 
   (�������� XBytes=3, YLines = 24, ��� 3 ��� 8*3=24 ����� �� ����������� = YLines)
   fcolor - ���� �������
   bgcolor - ���� ������� ���� */
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
        if (idx == 0x20) idx = 43; //������
        if (idx == 0x2D) idx = 10; //�����
      }
    }
    LCD_ShowChar((uint8_t*)&L24X27[idx*3*27], (WChar / 8), HChar, fcolor, bgcolor);
    text++;
    XPos++;
  }
}

//------------------------------------------------------------------------------
/* ���� ��������� � ������� �������� � ����������� �� ����� ������� � 0 */
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
/* ���������� ���� ������ � ����� TXFIFO ���������� SPI, ���� ����� � ������ ���
   ������� ���� ��� �� �������� */
void SPI_DataStream(uint8_t data)
{
  while ((SPI1->SR & SPI_SR_TXE) == 0); 
  *(uint8_t *)&(SPI1->DR) = data; //���������� � �����
//  if ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE)
//    data = (uint8_t)SPI1->DR; //������ �����  
//  while ((SPI1->SR & SPI_SR_TXE) != SPI_SR_TXE); //���� ���� ����� ����� ����������
//  *(uint8_t *)&(SPI1->DR) = data; //����������
//  while ((SPI1->SR & SPI_SR_RXNE) != SPI_SR_RXNE); //���� ���� ���� �������� �� ����� ��������
//  data = (uint8_t)SPI1->DR; //������ �����  
}
