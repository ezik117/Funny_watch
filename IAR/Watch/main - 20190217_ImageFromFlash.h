//макросы управления пинами микросхемы памяти
#define MEM_HOLD(x) (x == 0 ? (GPIOA->BSRR = GPIO_BSRR_BR_0) : (GPIOA->BSRR = GPIO_BSRR_BS_0))
#define MEM_CS(x) (x == 0 ? (GPIOA->BSRR = GPIO_BSRR_BR_1) : (GPIOA->BSRR = GPIO_BSRR_BS_1))
#define MEM_PAUSE MEM_HOLD(0)
#define MEM_RESUME MEM_HOLD(1)

//макросы управления пинами экрана
#define LCD_CS(x) (x == 0 ? (GPIOA->BSRR = GPIO_BSRR_BR_4) : (GPIOA->BSRR = GPIO_BSRR_BS_4))
#define LCD_DC(x) (x == 0 ? (GPIOA->BSRR = GPIO_BSRR_BR_9) : (GPIOA->BSRR = GPIO_BSRR_BS_9))
#define LCD_RESET(x) (x == 0 ? (GPIOA->BSRR = GPIO_BSRR_BR_10) : (GPIOA->BSRR = GPIO_BSRR_BS_10))

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

//---- ПРОТОТИПЫ ФУНКЦИЙ -------------------------------------------------------
void Delay(uint32_t msTime);
void LCD_Init();
void SPI_Send(uint8_t data);
void LCD_FillBackground(uint16_t color);
void LCD_SetWindow(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1);

void MemCopy(uint8_t* from, uint8_t* to, uint16_t len);