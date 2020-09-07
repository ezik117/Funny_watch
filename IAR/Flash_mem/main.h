/*##############################################################################
#
# ОПИСАНИЕ: ВСПОМОГАТЕЛЬНЫЕ ЭЛЕМЕНТЫ МОДУЛЯ MAIN
# ФАЙЛ: main.h
#
##############################################################################*/

#define bool uint8_t
#define true 1
#define false 0

//константы управления пинами микросхемы памяти
#define MEM_HOLD_ON 0
#define MEM_HOLD_OFF 1

//константы управления сигналом /CS
#define CS_ENABLE 0
#define CS_DISABLE 1

//макросы управления пинами микросхемы памяти
#define MEM_HOLD(x) (x == 0 ? (GPIOA->BSRR = GPIO_BSRR_BR_0) : (GPIOA->BSRR = GPIO_BSRR_BS_0))
#define MEM_CS(x) (x == 0 ? (GPIOA->BSRR = GPIO_BSRR_BR_1) : (GPIOA->BSRR = GPIO_BSRR_BS_1))
#define MEM_PAUSE MEM_HOLD(0)
#define MEM_RESUME MEM_HOLD(1)


void usart_init();
void processUsart();
void SendUData(uint8_t data);
uint8_t SendSPI(uint8_t data);
