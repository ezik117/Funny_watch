#define bool uint8_t
#define true 1
#define false 0

//макросы управления бипером
#define BEEP(x) (x == 0 ? (GPIOB->BSRR = GPIO_BSRR_BR_1) : (GPIOB->BSRR = GPIO_BSRR_BS_1))

//макросы управления пинами микросхемы памяти
#define MEM_HOLD(x) (x == 0 ? (GPIOA->BSRR = GPIO_BSRR_BR_0) : (GPIOA->BSRR = GPIO_BSRR_BS_0))
#define MEM_CS(x) (x == 0 ? (GPIOA->BSRR = GPIO_BSRR_BR_1) : (GPIOA->BSRR = GPIO_BSRR_BS_1))
#define MEM_PAUSE MEM_HOLD(0)
#define MEM_RESUME MEM_HOLD(1)

//макросы управления пинами экрана
#define LCD_CS(x) (x == 0 ? (GPIOA->BSRR = GPIO_BSRR_BR_4) : (GPIOA->BSRR = GPIO_BSRR_BS_4))
#define LCD_DC(x) (x == 0 ? (GPIOA->BSRR = GPIO_BSRR_BR_9) : (GPIOA->BSRR = GPIO_BSRR_BS_9))
#define LCD_RESET(x) (x == 0 ? (GPIOA->BSRR = GPIO_BSRR_BR_10) : (GPIOA->BSRR = GPIO_BSRR_BS_10))

//макросы управления контроллером сенсорного экрана
#define TOUCH_CS(x) (x == 0 ? (GPIOA->BSRR = GPIO_BSRR_BR_2) : (GPIOA->BSRR = GPIO_BSRR_BS_2))
#define TOUCH_PRESSED (!(GPIOA->IDR & GPIO_IDR_3)) //0-touched, 1-free

//константы управления сигналом /CS
#define CS_ENABLE 0
#define CS_DISABLE 1

//константы управления сигналом /RESTART
#define RST_LO 0
#define RST_HI 1

//константы управления пинами микросхемы памяти
#define MEM_HOLD_ON 0
#define MEM_HOLD_OFF 1

//константы управления пинами экрана
#define LCD_CMD 0
#define LCD_DATA 1

#define SPI_WAIT_TX_READY while ((SPI1->SR & SPI_SR_TXE) != SPI_SR_TXE);
#define SPI_WAIT_FOR_COMPLETION while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY);

#define BackGroundColor 0x6B4D
#define base_digits_x 33
#define base_digits_y 66
#define base_fdigits_x 17


static const uint32_t memMap[] =
  {0x00000000, 0x000035F0, 0x00006BE0, 0x0000A1D0, 0x0000D7C0, //0[0], 1[1], 2[2], 3[3], 4[4]
   0x00010DB0, 0x000143A0, 0x00017990, 0x0001AF80, 0x0001E570, //5[5], 6[6], 7[7], 8[8], 9[9]
   0x00021B60, 0x00022270, 0x00029F78, //10[:], 11[adjustment panel], 12[service menu]
   0x00031C80, 0x00035270, 0x00038860, 0x0003BE50, 0x0003F440, //13[0], 14[1], 15[2], 16[3], 17[4]
   0x00042A30, 0x00046020, 0x00049610, 0x0004CC00, 0x000501F0, //18[5], 19[6], 20[7], 21[8], 22[9]
   0x000537E0, 0x00054D02, 0x0007A50A //23[sunny], 24[about], 25[alarm]
  };

//---- ПРОТОТИПЫ ФУНКЦИЙ -------------------------------------------------------
void Delay(uint32_t msTime);
void LCD_Init();
void SPI_Send(uint8_t data);
uint8_t SPI_Exchange(uint8_t data);

void LCD_FillBackground(uint16_t color);
void LCD_SetWindow(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1);
void LCD_FillRectangle(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1, uint16_t color);
void LCD_ShowImage16FromMem(uint16_t x, uint8_t y, uint32_t address);
void LCD_ShowImage2FromRom(uint16_t x, uint8_t y, uint8_t imgIdx, uint8_t font, uint16_t fcolor, uint16_t bcolor);
void LCD_ShowText(uint16_t x, uint8_t y, uint8_t* address, uint8_t font, uint16_t fcolor, uint16_t bcolor);
void LCD_ShowDigits(uint16_t x, uint8_t y, uint16_t value, uint8_t digits);

void Time_Show(bool updateAll, bool realTime, uint8_t HHMM);
#define TS_HH 0x01U //бит показывает что отобразим только разряды часов
#define TS_MM 0x02U //бит показывает что отобразим только разряды минут (допустимо комбинирование)
uint32_t Time_Add(uint32_t time, uint8_t HH, uint8_t MM);
uint32_t Time_Sub(uint32_t time, uint8_t HH, uint8_t MM);
void Time_SetCalendarTM(uint32_t newTime, uint32_t newDate);

void Date_Show();
uint8_t Date_GetDayOfWeek(uint8_t D, uint8_t M, uint16_t Y);

void Alarm_ShowDowCheckboxes();

uint16_t ConvertToBCD(uint16_t data);

void ProcessTouching();

uint8_t CharCount(uint8_t* address);

void SaveToRom();
void LoadFromRom();
void MemWriteEnable();
void MemWaitForWriteEnd();
void MemWriteSequence(uint8_t* data, uint8_t length);
void MemReadSequence(uint8_t* value, uint8_t length);
void MemEndOfCommand();