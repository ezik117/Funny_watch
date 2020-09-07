/*##############################################################################
#
# ОПИСАНИЕ: ОБЪЯВЛЕНИЕ ПЕРЕМЕННЫХ И МАКРОСОВ ПРОГРАММЫ
# ФАЙЛ: variables.h
#
##############################################################################*/

//--- макросы ------------------------------------------------------------------

#define TRUE (1U)
#define FALSE (0U)

// определение для 32х битов
#define BIT0                      (0x00000001U)                          
#define BIT1                      (0x00000002U)                          
#define BIT2                      (0x00000004U)                          
#define BIT3                      (0x00000008U)                          
#define BIT4                      (0x00000010U)                          
#define BIT5                      (0x00000020U)                          
#define BIT6                      (0x00000040U)                          
#define BIT7                      (0x00000080U)                          
#define BIT8                      (0x00000100U)                          
#define BIT9                      (0x00000200U)                          
#define BIT10                     (0x00000400U)                          
#define BIT11                     (0x00000800U)                          
#define BIT12                     (0x00001000U)                          
#define BIT13                     (0x00002000U)                          
#define BIT14                     (0x00004000U)                          
#define BIT15                     (0x00008000U)  
#define BIT16                     (0x00010000U)                          
#define BIT17                     (0x00020000U)                          
#define BIT18                     (0x00040000U)                          
#define BIT19                     (0x00080000U)                          
#define BIT20                     (0x00100000U)                          
#define BIT21                     (0x00200000U)                          
#define BIT22                     (0x00400000U)                          
#define BIT23                     (0x00800000U)                          
#define BIT24                     (0x01000000U)                          
#define BIT25                     (0x02000000U)                          
#define BIT26                     (0x04000000U)                          
#define BIT27                     (0x08000000U)                          
#define BIT28                     (0x10000000U)                          
#define BIT29                     (0x20000000U)                          
#define BIT30                     (0x40000000U)                          
#define BIT31                     (0x80000000U) 

// определения для выбора функции регистра AFR 
#define GPIO_AFR_AF0              (0x0000)
#define GPIO_AFR_AF1              (0x0001)
#define GPIO_AFR_AF2              (0x0010)
#define GPIO_AFR_AF3              (0x0011)
#define GPIO_AFR_AF4              (0x0100)
#define GPIO_AFR_AF5              (0x0101)
#define GPIO_AFR_AF6              (0x0110)
#define GPIO_AFR_AF7              (0x0111)


#define bool uint8_t

#define TOGGLE_BIT(REG, BIT) ((REG) ^= (BIT))

//--- объявления переменных ----------------------------------------------------

// Глобальный контейнер для хранения переменных (создается в main.c)
struct Variables_TypeDef
{
  volatile int32_t DelayCounter;
};
extern struct Variables_TypeDef v;

