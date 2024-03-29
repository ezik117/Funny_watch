#pragma pack(push)
#pragma pack(1) //1-byte align
typedef struct
{
  uint16_t width; //2b
  uint16_t height; //2b
  uint32_t length; //4b
} TBmpHeader16; //len: 8 bytes
#pragma pack(pop)

TBmpHeader16* BmpHeader16;

static const uint8_t digits8[] = { //7x7
  	0x3E, 0x41, 0x41, 0x41, 0x41, 0x41, 0x3E, //0
        0x0C, 0x18, 0x28, 0x08, 0x08, 0x08, 0x3E, //1
        0x3E, 0x41, 0x01, 0x0E, 0x10, 0x20, 0x7F, //2
        0x3E, 0x41, 0x01, 0x0E, 0x01, 0x41, 0x3E, //3
        0x41, 0x41, 0x41, 0x3F, 0x01, 0x01, 0x01, //4
        0x7F, 0x40, 0x40, 0x7E, 0x01, 0x01, 0x7E, //5
        0x3E, 0x41, 0x40, 0x7E, 0x41, 0x41, 0x3E, //6
        0x7F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08, //7
        0x3E, 0x41, 0x41, 0x3E, 0x41, 0x41, 0x3E, //8
        0x3E, 0x41, 0x41, 0x3F, 0x01, 0x41, 0x3E //9
};

//-------------------------------------------------------------------------------------------------
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

//--- некоторые константы цвета ---
#define LCD_BLACK        0x0000
#define LCD_BLUE         0x001F
#define LCD_RED          0xF800
#define LCD_GREEN        0x07E0
#define LCD_CYAN         0x07FF
#define LCD_MAGENTA      0xF81F
#define LCD_YELLOW       0xFFE0  
#define LCD_WHITE        0xFFFF
#define LCD_GREY         0x8410

//-------------------------------------------------------------------------------------------------
static const uint8_t L24X27[] = //24x27, "."=8x27, "А" = 0xC0, bytes per symbol = 81
{
//цифры (10 шт)
/*[00] 0*/ 0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,
/*[01] 1*/ 0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x0E,0xE0,0x00,0x0E,0xE0,0x00,0x0E,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x0E,0xEE,0x00,0x0E,0xEE,0x00,0x0E,0xEE,0x00,
/*[02] 2*/ 0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,
/*[03] 3*/ 0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0xFE,0x00,0x00,0xFE,0x00,0x00,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,
/*[04] 4*/ 0x03,0x80,0xE0,0x03,0x80,0xE0,0x03,0x80,0xE0,0x00,0x00,0x00,0x1C,0x00,0xE0,0x1C,0x00,0xE0,0x1C,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,
/*[05] 5*/ 0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,
/*[06] 6*/ 0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,
/*[07] 7*/ 0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x00,0x03,0x80,0x00,0x03,0x80,0x00,0x03,0x80,0x00,0x00,0x00,0x00,0x0E,0x00,0x00,0x0E,0x00,0x00,0x0E,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x00,0x38,0x00,0x00,0x38,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x03,0x80,0x00,0x03,0x80,0x00,0x03,0x80,0x00,
/*[08] 8*/ 0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,
/*[09] 9*/ 0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x0F,0xFF,0xE0,0x0F,0xFF,0xE0,0x0F,0xFF,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,

//буквы (32 шт)
/*[10] А*/ 0x0F,0xFF,0xE0,0x0F,0xFF,0xE0,0x0F,0xFF,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xEF,0xFF,0xE0,0xEF,0xFF,0xE0,0xEF,0xFF,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,
/*[11] Б*/ 0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,
/*[12] В*/ 0xFF,0xFE,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,
/*[13] Г*/ 0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,
/*[14] Д*/ 0x03,0xF8,0x00,0x03,0xF8,0x00,0x03,0xF8,0x00,0x00,0x00,0x00,0x38,0x03,0x80,0x38,0x03,0x80,0x38,0x03,0x80,0x00,0x00,0x00,0x38,0x03,0x80,0x38,0x03,0x80,0x38,0x03,0x80,0x00,0x00,0x00,0x38,0x03,0x80,0x38,0x03,0x80,0x38,0x03,0x80,0x00,0x00,0x00,0x38,0x03,0x80,0x38,0x03,0x80,0x38,0x03,0x80,0x00,0x00,0x00,0x38,0x03,0x80,0x38,0x03,0x80,0x38,0x03,0x80,0x00,0x00,0x00,0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,
/*[15] Е*/ 0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,
/*[16] И*/ 0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x03,0xE0,0xE0,0x03,0xE0,0xE0,0x03,0xE0,0x00,0x00,0x00,0xE0,0x0F,0xE0,0xE0,0x0F,0xE0,0xE0,0x0F,0xE0,0x00,0x00,0x00,0xE1,0xF0,0xE0,0xE1,0xF0,0xE0,0xE1,0xF0,0xE0,0x00,0x00,0x00,0xFE,0x00,0xE0,0xFE,0x00,0xE0,0xFE,0x00,0xE0,0x00,0x00,0x00,0xF8,0x00,0xE0,0xF8,0x00,0xE0,0xF8,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,
/*[17] К*/ 0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x0E,0x00,0xE0,0x0E,0x00,0xE0,0x0E,0x00,0x00,0x00,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0x00,0x00,0x00,0xFE,0x00,0x00,0xFE,0x00,0x00,0xFE,0x00,0x00,0x00,0x00,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0x00,0x00,0x00,0xE0,0x0E,0x00,0xE0,0x0E,0x00,0xE0,0x0E,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,
/*[18] Л*/ 0x0F,0xFF,0xE0,0x0F,0xFF,0xE0,0x0F,0xFF,0xE0,0x00,0x00,0x00,0x38,0x00,0xE0,0x38,0x00,0xE0,0x38,0x00,0xE0,0x00,0x00,0x00,0x38,0x00,0xE0,0x38,0x00,0xE0,0x38,0x00,0xE0,0x00,0x00,0x00,0x38,0x00,0xE0,0x38,0x00,0xE0,0x38,0x00,0xE0,0x00,0x00,0x00,0x38,0x00,0xE0,0x38,0x00,0xE0,0x38,0x00,0xE0,0x00,0x00,0x00,0x38,0x00,0xE0,0x38,0x00,0xE0,0x38,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,
/*[19] М*/ 0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xF8,0x03,0xE0,0xF8,0x03,0xE0,0xF8,0x03,0xE0,0x00,0x00,0x00,0xFE,0x0F,0xE0,0xFE,0x0F,0xE0,0xFE,0x0F,0xE0,0x00,0x00,0x00,0xE1,0xF0,0xE0,0xE1,0xF0,0xE0,0xE1,0xF0,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,
/*[20] Н*/ 0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,
/*[21] О*/ 0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,
/*[22] П*/ 0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,
/*[23] Р*/ 0xFF,0xFE,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,
/*[24] С*/ 0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,
/*[25] Т*/ 0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0xFF,0xFF,0xE0,0x00,0x00,0x00,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,
/*[26] У*/ 0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x0F,0xFF,0xE0,0x0F,0xFF,0xE0,0x0F,0xFF,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,
/*[27] Ф*/ 0x7F,0xFF,0xC0,0x7F,0xFF,0xC0,0x7F,0xFF,0xC0,0x00,0x00,0x00,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x00,0x00,0x00,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x00,0x00,0x00,0x7F,0xFF,0xC0,0x7F,0xFF,0xC0,0x7F,0xFF,0xC0,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,
/*[28] Ц*/ 0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x3F,0xFF,0xF8,0x3F,0xFF,0xF8,0x3F,0xFF,0xF8,
/*[29] Ч*/ 0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x7F,0xFF,0xE0,0x7F,0xFF,0xE0,0x7F,0xFF,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,
/*[30] Ь*/ 0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0xFF,0xFE,0x00,
/*[31] Ю*/ 0xE0,0xFF,0xE0,0xE0,0xFF,0xE0,0xE0,0xFF,0xE0,0x00,0x00,0x00,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x00,0x00,0x00,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x00,0x00,0x00,0xFF,0xE0,0xE0,0xFF,0xE0,0xE0,0xFF,0xE0,0xE0,0x00,0x00,0x00,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x00,0x00,0x00,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x00,0x00,0x00,0xE0,0xFF,0xE0,0xE0,0xFF,0xE0,0xE0,0xFF,0xE0,
/*[32] Я*/ 0x0F,0xFF,0xE0,0x0F,0xFF,0xE0,0x0F,0xFF,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0xE0,0x00,0x00,0x00,0x0F,0xFF,0xE0,0x0F,0xFF,0xE0,0x0F,0xFF,0xE0,0x00,0x00,0x00,0x00,0xF0,0xE0,0x00,0xF0,0xE0,0x00,0xF0,0xE0,0x00,0x00,0x00,0x0F,0x00,0xE0,0x0F,0x00,0xE0,0x0F,0x00,0xE0,0x00,0x00,0x00,0xF0,0x00,0xE0,0xF0,0x00,0xE0,0xF0,0x00,0xE0,

//символы
/*[33] v*/ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x06,0x00,0x00,0x0E,0x00,0x00,0x1C,0x00,0x00,0x3C,0x00,0x00,0x78,0x00,0x00,0xF8,0x00,0x01,0xF0,0x00,0x03,0xF0,0x60,0x07,0xE0,0x7C,0x0F,0xE0,0x3F,0x9F,0xC0,0x1F,0xFF,0xC0,0x0F,0xFF,0x80,0x07,0xFF,0x80,0x03,0xFF,0x00,0x01,0xFF,0x00,0x00,0xFE,0x00,0x00,0x7E,0x00,0x00,0x3C,0x00,0x00,0x1C,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00

}; 

static const uint8_t L24X27FUN[] =
{
/*[00] 0*/0x26, 0xAA,   0x00,0x00,0x00,0x00,0xFF,0x00,0x03,0xFF,0xC0,0x07,0xFF,0xE0,0x07,0xFF,0xF0,0x0F,0xFF,0xF8,0x1F,0xFF,0xF8,0x1F,0xFF,0xFC,0x1F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xFF,0xFE,0x3F,0xC1,0xFE,0x3F,0x81,0xFE,0x3F,0x80,0xFE,0x3F,0x80,0xFC,0x1F,0x80,0xFC,0x1F,0x80,0xFC,0x1F,0x81,0xF8,0x1F,0xC1,0xF8,0x0F,0xC1,0xF0,0x07,0xE3,0xF0,0x07,0xFF,0xE0,0x03,0xFF,0xC0,0x00,0xFF,0x80,0x00,0x7E,0x00,0x00,0x00,0x00,
/*[01] 1*/0xD2, 0xBD,   0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x03,0xF0,0x00,0x1F,0xE0,0x00,0xFF,0xE0,0x07,0xFF,0xE0,0x0F,0xFF,0xE0,0x07,0xFF,0xE0,0x07,0xFF,0xE0,0x07,0xFF,0xE0,0x00,0x7F,0xE0,0x00,0x7F,0xE0,0x00,0x7F,0xE0,0x00,0x7F,0xE0,0x00,0x7F,0xE0,0x00,0x7F,0xE0,0x00,0x7F,0xE0,0x00,0x7F,0xE0,0x00,0x7F,0xE0,0x00,0x3F,0xE0,0x00,0x3F,0xE0,0x00,0x3F,0xE0,0x00,0x3F,0xE0,0x00,0x3F,0xE0,0x00,0x3F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,
/*[02] 2*/0x1F, 0x3A,   0x00,0x00,0x00,0x00,0xFE,0x00,0x03,0xFF,0xC0,0x07,0xFF,0xE0,0x07,0xFF,0xF0,0x0F,0xFF,0xF0,0x0F,0xFF,0xF8,0x0F,0xFF,0xF8,0x0F,0xFF,0xF8,0x07,0xFF,0xF8,0x00,0x3F,0xF8,0x00,0x1F,0xF0,0x00,0x1F,0xF0,0x00,0x1F,0xE0,0x00,0x1F,0xE0,0x00,0x1F,0xC0,0x00,0x1F,0x80,0x00,0x3F,0x00,0x00,0x3E,0x00,0x00,0x7E,0x00,0x00,0x7C,0x00,0x00,0xF8,0x00,0x00,0xFF,0xE0,0x01,0xFF,0xE0,0x03,0xFF,0xE0,0x03,0xFF,0xE0,0x00,0x00,0x00,
/*[03] 3*/0xAD, 0x3E,   0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFF,0xC0,0x0F,0xFF,0xE0,0x0F,0xFF,0xF0,0x1F,0xFF,0xF8,0x1F,0xFF,0xF8,0x1F,0xFF,0xF8,0x0F,0xFF,0xF8,0x00,0x3F,0xF8,0x00,0x1F,0xF8,0x00,0x0F,0xF8,0x00,0x1F,0xF0,0x00,0x1F,0xE0,0x00,0x7F,0x80,0x00,0xFE,0x00,0x00,0xFF,0x00,0x00,0x0F,0xC0,0x00,0x07,0xE0,0x00,0x07,0xE0,0x00,0x07,0xE0,0x00,0x07,0xE0,0x03,0xFF,0xE0,0x03,0xFF,0xC0,0x03,0xFF,0x80,0x01,0xFE,0x00,0x00,0x00,0x00,
/*[04] 4*/0x6C, 0x7F,   0x00,0x00,0x00,0x00,0x1F,0xE0,0x00,0x1F,0xE0,0x00,0x3F,0xE0,0x00,0x7F,0xE0,0x00,0x7F,0xE0,0x00,0xFF,0xE0,0x01,0xFF,0xE0,0x01,0xFF,0xE0,0x01,0xFF,0xC0,0x01,0xFF,0xC0,0x01,0xFF,0xC0,0x01,0xFF,0xC0,0x01,0xCF,0xC0,0x03,0xCF,0xC0,0x03,0xCF,0xC0,0x03,0x8F,0xC0,0x07,0x8F,0xC0,0x07,0x0F,0xC0,0x0F,0xFF,0xF8,0x0F,0xFF,0xF8,0x1F,0xFF,0xF8,0x00,0x0F,0xC0,0x00,0x0F,0xC0,0x00,0x0F,0xC0,0x00,0x0F,0xC0,0x00,0x00,0x00,
/*[05] 5*/0xFB, 0x87,   0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFF,0xF0,0x03,0xFF,0xF0,0x03,0xFF,0xF0,0x07,0xFF,0xF0,0x07,0xFF,0xE0,0x07,0xFF,0xE0,0x07,0xFF,0xC0,0x07,0x80,0x00,0x0F,0x80,0x00,0x0F,0x00,0x00,0x0F,0xFF,0xC0,0x1F,0xFF,0xE0,0x1F,0xFF,0xF0,0x1F,0xFF,0xF8,0x3F,0xFF,0xF8,0x1F,0xFF,0xFC,0x00,0x07,0xFC,0x00,0x03,0xF8,0x00,0x03,0xF8,0x00,0x07,0xF8,0x08,0x0F,0xF0,0x0F,0xFF,0xE0,0x0F,0xFF,0x80,0x07,0xFC,0x00,0x00,0x00,0x00,
/*[06] 6*/0xFC, 0xE2,   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x80,0x00,0x7F,0xC0,0x00,0xFF,0x80,0x01,0xFE,0x00,0x03,0xFC,0x00,0x03,0xF8,0x00,0x07,0xF0,0x00,0x0F,0xE0,0x00,0x0F,0xE0,0x00,0x1F,0xFF,0xC0,0x1F,0xFF,0xF0,0x1F,0xFF,0xF0,0x3F,0xFF,0xF8,0x3F,0xFF,0xF8,0x3F,0xC3,0xFC,0x3F,0x81,0xFC,0x3F,0x81,0xFC,0x3F,0xC1,0xF8,0x1F,0xC1,0xF8,0x1F,0xC3,0xF8,0x0F,0xFF,0xF0,0x07,0xFF,0xE0,0x03,0xFF,0xC0,0x00,0x7F,0x00,0x00,0x00,0x00,
/*[07] 7*/0x5E, 0xFB,   0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xFF,0xFC,0x1F,0xFF,0xFC,0x1F,0xFF,0xF8,0x1F,0xFF,0xF8,0x1F,0xFF,0xF0,0x1F,0xFF,0xF0,0x0F,0xFF,0xE0,0x00,0x1F,0xC0,0x00,0x1F,0xC0,0x00,0x1F,0x80,0x00,0x3F,0x80,0x00,0x3F,0x00,0x00,0x3F,0x00,0x00,0x7F,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0xFC,0x00,0x00,0xFC,0x00,0x00,0xFC,0x00,0x00,0xF8,0x00,0x01,0xF8,0x00,0x01,0xF8,0x00,0x01,0xF0,0x00,0x00,0x00,0x00,
/*[08] 8*/0xD9, 0xDD,   0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFF,0x80,0x0F,0xFF,0xE0,0x0F,0xFF,0xF0,0x1F,0xFF,0xF8,0x3F,0xFF,0xF8,0x3F,0xFF,0xFC,0x7F,0xFF,0xFC,0x7F,0xC3,0xFC,0x3F,0x81,0xFC,0x3F,0x81,0xFC,0x3F,0x81,0xF8,0x1F,0xC3,0xF8,0x0F,0xFF,0xE0,0x03,0xFF,0x80,0x07,0xFF,0xC0,0x0F,0xFF,0xE0,0x0F,0xC7,0xF0,0x1F,0xC3,0xF0,0x1F,0x83,0xF0,0x1F,0x83,0xF0,0x0F,0xC7,0xE0,0x07,0xFF,0xC0,0x01,0xFF,0x80,0x00,0x7C,0x00,0x00,0x00,0x00,
/*[09] 9*/0xD9, 0xDD,   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x80,0x03,0xFF,0xE0,0x07,0xFF,0xF0,0x0F,0xFF,0xF8,0x1F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xFF,0xFE,0x3F,0x81,0xFE,0x3F,0x81,0xFC,0x3F,0x81,0xFC,0x1F,0x81,0xFC,0x1F,0xC3,0xF8,0x0F,0xFF,0xF8,0x07,0xFF,0xF0,0x01,0xFF,0xF0,0x00,0x0F,0xE0,0x00,0x0F,0xC0,0x00,0x1F,0xC0,0x00,0x3F,0x80,0x00,0x3F,0x00,0x00,0x7E,0x00,0x00,0xFC,0x00,0x00,0x30,0x00,0x00,0x00,0x00,

/*[10] А*/0xD9, 0x32,   0x00,0x00,0x00,0x00,0x1E,0x00,0x00,0x3F,0x00,0x00,0x7F,0x00,0x00,0x7F,0x80,0x00,0xFF,0x80,0x00,0xFF,0xC0,0x01,0xFF,0xC0,0x01,0xFF,0xE0,0x03,0xF3,0xE0,0x03,0xE1,0xE0,0x07,0xC1,0xF0,0x07,0xC1,0xF0,0x0F,0xC1,0xF0,0x0F,0xC1,0xF8,0x1F,0xFF,0xF8,0x1F,0xFF,0xF8,0x1F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0x81,0xFC,0x7F,0x01,0xFC,0x7F,0x00,0xFC,0x7F,0x00,0xFC,0x3E,0x00,0xF0,0x3E,0x00,0x00,0x00,0x00,0x00,
/*[11] Б*/0xD2, 0xBD,   0x00,0x00,0x00,0x00,0x01,0xC0,0x00,0x07,0xE0,0x00,0x3F,0xF0,0x01,0xFF,0xF0,0x07,0xFF,0xF0,0x3F,0xFF,0xE0,0x3F,0xFF,0xE0,0x3F,0xF8,0x00,0x1F,0xC0,0x00,0x1F,0x80,0x00,0x1F,0x83,0xF8,0x1F,0x8F,0xFC,0x0F,0xFF,0xFC,0x0F,0xFF,0xFE,0x0F,0xFF,0xFE,0x07,0xFC,0x3E,0x07,0xE0,0x3E,0x07,0xE0,0x3E,0x07,0xE0,0x3E,0x03,0xF0,0x7E,0x03,0xFF,0xFC,0x03,0xFF,0xF8,0x01,0xFF,0xF0,0x01,0xFF,0xC0,0x01,0xF0,0x00,0x00,0x00,0x00,
/*[12] В*/0xD9, 0xDD,   0x00,0x00,0x00,0x00,0xFF,0x00,0x0F,0xFF,0xE0,0x3F,0xFF,0xF0,0x3F,0xFF,0xF8,0x3E,0x03,0xF8,0x3E,0x03,0xF8,0x3E,0x03,0xF8,0x3E,0x03,0xF0,0x3E,0x07,0xF0,0x1E,0x07,0xE0,0x1F,0xFF,0xC0,0x1F,0xFF,0xF0,0x1F,0xFF,0xF8,0x1F,0xFF,0xFC,0x1F,0xE3,0xFE,0x1F,0x80,0xFE,0x1F,0x80,0x7F,0x1F,0x00,0x7F,0x1F,0x00,0x7F,0x1F,0x80,0xFF,0x1F,0xC0,0xFF,0x1F,0xF1,0xFE,0x1F,0xFF,0xFC,0x1F,0xFF,0xF8,0x00,0xFF,0xC0,0x00,0x00,0x00,
/*[13] Г*/0xAD, 0x3E,   0x00,0x00,0x00,0x00,0x00,0x0C,0x1F,0xFF,0xFC,0x3F,0xFF,0xFE,0x3F,0xFF,0xFE,0x3F,0xFF,0xFE,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0x00,0x00,0x3F,0x00,0x00,0x3F,0x00,0x00,0x3F,0x00,0x00,0x3F,0x00,0x00,0x3F,0x00,0x00,0x3E,0x00,0x00,0x3E,0x00,0x00,0x3E,0x00,0x00,0x3F,0x00,0x00,0x3F,0x00,0x00,0x3F,0x00,0x00,0x7F,0x00,0x00,0x7F,0x00,0x00,0x7F,0x80,0x00,0x7F,0xC0,0x00,0x7F,0xC0,0x00,0x0F,0x00,0x00,0x00,0x00,0x00,
/*[14] Д*/0x6C, 0x7F,   0x00,0x00,0x00,0x00,0x3E,0x00,0x00,0x3E,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0x7F,0x00,0x00,0xE7,0x00,0x00,0xE7,0x00,0x00,0xE7,0x00,0x00,0xE3,0x00,0x01,0xE3,0x80,0x01,0xC3,0x80,0x01,0xC3,0x80,0x01,0xC3,0x80,0x03,0xFF,0x80,0x03,0xFF,0xF8,0x3F,0xFF,0xFE,0x7F,0xFF,0xFE,0x7F,0xFF,0xFE,0xFF,0xFF,0xFE,0xFF,0x00,0xFE,0xFF,0x00,0x7E,0xFE,0x00,0x7E,0x7E,0x00,0x3F,0x3C,0x00,0x1C,0x18,0x00,0x00,
/*[15] Е*/0xFB, 0x87,   0x00,0x00,0x00,0x00,0x0F,0xF0,0x00,0x7F,0xFE,0x03,0xFF,0xFE,0x0F,0xFF,0xFE,0x1F,0xFF,0xFE,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xEF,0xF0,0x3F,0xC0,0x00,0x3F,0xC0,0x00,0x7F,0xFF,0xC0,0x7F,0xFF,0xE0,0x7F,0xFF,0xE0,0x7F,0xFF,0xE0,0x7F,0xFF,0xC0,0x7F,0xFF,0xC0,0x7F,0xE0,0x00,0x3F,0xF8,0x38,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x1F,0xFF,0xFC,0x1F,0xFF,0xFC,0x07,0xFF,0xF8,0x03,0xFF,0xE0,0x00,0x00,0x00,
/*[16] И*/0x1F, 0x3A,   0x00,0x00,0x00,0x7F,0x87,0xFC,0x7F,0xCF,0xFE,0x7F,0xC7,0xFE,0x7F,0xC7,0xFE,0x7F,0xC7,0xFE,0x7F,0xC7,0xFE,0x7F,0xC7,0xFE,0x7F,0xC7,0xFE,0x7F,0xCF,0xFE,0x7F,0xCF,0xFE,0x7F,0xDF,0xFE,0x3F,0xFF,0xFE,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xF7,0xFC,0x3F,0xF7,0xFC,0x3F,0xE3,0xFC,0x3F,0xE3,0xFC,0x1F,0xC1,0xF8,0x0F,0x80,0x00,
/*[17] К*/0x26, 0xAA,   0x3E,0x00,0x00,0x7F,0x00,0x00,0x7F,0x00,0x00,0x7F,0x01,0xC0,0x7F,0x03,0xF0,0x7F,0x07,0xFC,0x7F,0x07,0xFE,0x7F,0x8F,0xFC,0x7F,0x9F,0xF8,0x3F,0x9F,0xF0,0x3F,0xBF,0xE0,0x3F,0xFF,0xC0,0x3F,0xFF,0x00,0x3F,0xFF,0x00,0x3F,0xFF,0x00,0x3F,0xFF,0x80,0x3F,0xFF,0xC0,0x3F,0xFF,0xE0,0x3F,0xFF,0xF0,0x3F,0xFF,0xFC,0x3F,0xCF,0xFE,0x3F,0xCF,0xFC,0x3F,0xE7,0xF8,0x3F,0xE3,0xF0,0x3F,0xE1,0xC0,0x3F,0x00,0x80,0x00,0x00,0x00,
/*[18] Л*/0xFC, 0x0A,   0x00,0x00,0x00,0x00,0x1C,0x00,0x00,0xFE,0x00,0x03,0xFE,0x00,0x03,0xFF,0x00,0x03,0xFF,0x00,0x07,0xFF,0x80,0x07,0xFF,0x80,0x07,0xFF,0x80,0x07,0xFF,0xC0,0x0F,0xFF,0xC0,0x0F,0xFF,0xE0,0x0F,0xFF,0xE0,0x0F,0xEF,0xF0,0x1F,0xE7,0xF0,0x1F,0xC7,0xF0,0x1F,0xC7,0xF8,0x1F,0xC7,0xF8,0x3F,0xC3,0xFC,0x3F,0xC3,0xFC,0x3F,0x83,0xFC,0x3F,0x83,0xFE,0x7F,0x81,0xFE,0x7F,0x81,0xFE,0x7F,0x81,0xF0,0x7F,0x00,0x00,0x00,0x00,0x00,
/*[19] М*/0xA6, 0x7D,   0x00,0x00,0x00,0x07,0xC0,0x00,0x07,0xC7,0xC0,0x07,0xC7,0xE0,0x07,0xEF,0xE0,0x07,0xEF,0xF0,0x07,0xFF,0xF0,0x0F,0xFF,0xF0,0x0F,0xFF,0xF0,0x0F,0xFF,0xF0,0x0F,0xFF,0xF8,0x0F,0xFF,0xF8,0x1F,0xFF,0xF8,0x1F,0xFF,0xF8,0x1F,0xFF,0xF8,0x1F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xBD,0xFC,0x7F,0xB9,0xFE,0x7F,0x19,0xFE,0x7F,0x18,0xFE,0xFF,0x00,0xFE,0x7F,0x00,0xFC,0x1E,0x00,0xF8,0x06,0x00,0x60,0x00,0x00,0x00,
/*[20] Н*/0xEC, 0x3F,   0x00,0x00,0x00,0x7F,0x87,0xFE,0x7F,0x8F,0xFE,0x7F,0x8F,0xFE,0x7F,0x8F,0xFE,0x7F,0x87,0xFE,0x7F,0x87,0xFE,0x7F,0x87,0xFC,0x7F,0x87,0xFC,0x7F,0xFF,0xFC,0x7F,0xFF,0xFC,0x7F,0xFF,0xFC,0x7F,0xFF,0xFC,0x7F,0xFF,0xFC,0x7F,0xFF,0xFC,0x7F,0xFF,0xFC,0x7F,0xFF,0xFC,0x7F,0xFF,0xFC,0x3F,0x83,0xFC,0x3F,0x83,0xF8,0x3F,0x83,0xF8,0x3F,0x83,0xF8,0x3F,0xC7,0xF8,0x3F,0xC7,0xF8,0x3F,0xC3,0xF8,0x3F,0xC0,0x00,0x00,0x00,0x00,
/*[21] О*/0xFA, 0xB5,   0x00,0x00,0x00,0x00,0x7F,0x00,0x01,0xFF,0xC0,0x03,0xFF,0xE0,0x07,0xFF,0xF0,0x0F,0xFF,0xF8,0x1F,0xFF,0xF8,0x1F,0x80,0xFC,0x3F,0x00,0x7C,0x3F,0x00,0x7E,0x7E,0x00,0x3E,0x7E,0x00,0x3E,0x7E,0x00,0x3E,0x7E,0x00,0x3E,0x7E,0x00,0x3E,0x7F,0x00,0x7E,0x7F,0x00,0x7C,0x7F,0x80,0xFC,0x3F,0xFF,0xFC,0x3F,0xFF,0xF8,0x1F,0xFF,0xF8,0x1F,0xFF,0xF0,0x0F,0xFF,0xE0,0x07,0xFF,0xC0,0x03,0xFF,0x80,0x01,0xFE,0x00,0x00,0x00,0x00,
/*[22] П*/0xF9, 0x86,   0x00,0x7C,0x00,0x1F,0xFF,0xF0,0x7F,0xFF,0xF8,0x7F,0xFF,0xFC,0x7F,0xFF,0xFC,0x7F,0xFF,0xFC,0x7F,0xFF,0xFC,0x7F,0x83,0xFC,0x7F,0x83,0xFC,0x7F,0x83,0xFC,0x7F,0x83,0xFC,0x7F,0x83,0xFC,0x7F,0x83,0xFC,0x7F,0x83,0xFE,0x7F,0x83,0xFE,0x7F,0x83,0xFC,0x7F,0x83,0xFC,0x7F,0x83,0xFC,0x7F,0x83,0xFC,0x7F,0x83,0xFC,0x7F,0x83,0xFC,0x7F,0x83,0xFC,0x7F,0x83,0xFC,0x7F,0x83,0xFC,0x7F,0x81,0xFC,0x3F,0x81,0xF8,0x3F,0x00,0x00,
/*[23] Р*/0x85, 0x3F,   0x00,0x00,0x00,0x00,0x7F,0x80,0x03,0xFF,0xE0,0x1F,0xFF,0xF8,0x7F,0xFF,0xF8,0x7F,0xFF,0xFC,0x7F,0xE1,0xFC,0x7F,0x01,0xFC,0x7F,0x00,0xFC,0x7F,0x00,0xFC,0x7F,0x01,0xFC,0x3F,0x87,0xFC,0x3F,0x9F,0xF8,0x3F,0xFF,0xF0,0x3F,0xFF,0xE0,0x3F,0xFF,0x00,0x3F,0xF8,0x00,0x3F,0xE0,0x00,0x3F,0xE0,0x00,0x1F,0xF0,0x00,0x1F,0xF0,0x00,0x1F,0xF0,0x00,0x1F,0xF8,0x00,0x1F,0xF8,0x00,0x1F,0xF8,0x00,0x1F,0xF8,0x00,0x0F,0xE0,0x00,
/*[24] С*/0xFB, 0x08,   0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0xFF,0xC0,0x03,0xFF,0xF0,0x07,0xFF,0xF8,0x0F,0xFF,0xFC,0x1F,0xFF,0xFE,0x3F,0xFF,0xFE,0x3F,0xFF,0xFC,0x3F,0xC0,0xF0,0x7F,0x80,0x60,0x7F,0x00,0x00,0x7F,0x00,0x00,0x7F,0x00,0x00,0x7F,0x00,0x00,0x7F,0x00,0x00,0x7F,0x00,0x00,0x7F,0x80,0x70,0x3F,0xC1,0xF8,0x3F,0xFF,0xF8,0x1F,0xFF,0xFC,0x1F,0xFF,0xFC,0x0F,0xFF,0xFC,0x03,0xFF,0xF0,0x01,0xFF,0xC0,0x00,0x7E,0x00,0x00,0x00,0x00,
/*[25] Т*/0xEC, 0xD1,   0x00,0x00,0x00,0x00,0x00,0x1E,0x00,0x0F,0xFE,0x07,0xFF,0xFE,0x7F,0xFF,0xFE,0x7F,0xFF,0xFE,0x7F,0xFF,0xFE,0x7F,0xFF,0xFC,0x7F,0xFF,0xFC,0x3F,0xFF,0x00,0x38,0xFE,0x00,0x00,0xFE,0x00,0x00,0xFE,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0x7F,0x00,0x00,0x7F,0x00,0x00,0x7F,0x00,0x00,0x7F,0x80,0x00,0x7F,0x80,0x00,0x7F,0x80,0x00,0x7F,0x80,0x00,0x00,0x00,
/*[26] У*/0xBF, 0x4A,   0x00,0x00,0x00,0x3E,0x00,0xFE,0x7F,0x01,0xFE,0x7F,0x01,0xFC,0x3F,0x83,0xFC,0x1F,0xC3,0xF8,0x1F,0xE7,0xF8,0x0F,0xE7,0xF0,0x07,0xFF,0xF0,0x07,0xFF,0xE0,0x03,0xFF,0xE0,0x01,0xFF,0xC0,0x01,0xFF,0xC0,0x00,0xFF,0x80,0x00,0xFF,0x00,0x00,0xFF,0x00,0x01,0xFE,0x00,0x01,0xFC,0x00,0x03,0xF8,0x00,0x03,0xF0,0x00,0x07,0xE0,0x00,0x0F,0xE0,0x00,0x1F,0x80,0x00,0x3F,0x00,0x00,0x3E,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,
/*[27] Ф*/0xF9, 0x5A,   0x00,0x00,0x00,0x0F,0x00,0x00,0x3F,0x87,0xF8,0x3F,0xFF,0xFC,0x7F,0xFF,0xFC,0x7C,0x3C,0x3C,0x7C,0x3C,0x3C,0x7C,0x3C,0x3C,0x7C,0x3C,0x3C,0x7C,0x3C,0x3C,0x7C,0x3C,0x3C,0x3F,0xFE,0x7C,0x3F,0xFF,0xF8,0x3F,0xFF,0xF8,0x1F,0xFF,0xF0,0x00,0x3C,0x00,0x00,0x3C,0x00,0x00,0x3C,0x00,0x00,0x7C,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0xFE,0x00,0x00,0xFE,0x00,0x00,0x7C,0x00,0x00,0x00,0x00,
/*[28] Ц*/0xFB, 0x08,   0x00,0x00,0x00,0x1F,0x87,0xE0,0x7F,0x87,0xF0,0x7F,0x87,0xF0,0x7F,0x87,0xE0,0x7F,0x87,0xE0,0x7F,0x87,0xE0,0x7F,0x87,0xE0,0x7F,0x87,0xE0,0x7F,0x87,0xC0,0x7F,0x87,0xC0,0x7F,0x87,0xC0,0x7F,0x87,0xE0,0x7F,0x87,0xE0,0x7F,0x87,0xE0,0x7F,0xCF,0xE0,0x3F,0xFF,0xFE,0x3F,0xFF,0xFE,0x3F,0xFF,0xFE,0x3F,0xFF,0xFE,0x3F,0xFF,0xFE,0x3F,0xFF,0xFE,0x3F,0xFF,0xFE,0x00,0x00,0x3E,0x00,0x00,0x3E,0x00,0x00,0x3E,0x00,0x00,0x00,
/*[29] Ч*/0xC0, 0x1F,   0x00,0x00,0x00,0x7F,0xE3,0xFE,0x7F,0xE3,0xFE,0x7F,0xE3,0xFE,0x7F,0xE3,0xFE,0x7F,0xC3,0xFE,0x3F,0xC3,0xFE,0x3F,0xC3,0xFE,0x3F,0xC3,0xFE,0x3F,0xC3,0xFE,0x3F,0xFF,0xFC,0x1F,0xFF,0xFC,0x1F,0xFF,0xFC,0x1F,0xFF,0xFC,0x1F,0xFF,0xFC,0x1F,0xFF,0xFC,0x0F,0xFF,0xFC,0x00,0xFF,0xFC,0x00,0x03,0xFC,0x00,0x03,0xFC,0x00,0x03,0xFC,0x00,0x03,0xFC,0x00,0x03,0xFC,0x00,0x03,0xFC,0x00,0x03,0xFC,0x00,0x01,0xFC,0x00,0x00,0x00,
/*[30] Ь*/0xFD, 0x69,   0x00,0x00,0x00,0x07,0xF8,0x00,0x07,0xF8,0x00,0x07,0xF8,0x00,0x07,0xF0,0x00,0x0F,0xF0,0x00,0x0F,0xF0,0x00,0x0F,0xF0,0x00,0x0F,0xF0,0x00,0x0F,0xF0,0x00,0x0F,0xF0,0x00,0x0F,0xFF,0x00,0x0F,0xFF,0xC0,0x0F,0xFF,0xE0,0x0F,0xFF,0xF0,0x0F,0xFE,0xF0,0x0F,0xF0,0x70,0x0F,0xF0,0x78,0x0F,0xF0,0x78,0x07,0xF0,0xF0,0x07,0xFF,0xF0,0x07,0xFF,0xF0,0x03,0xFF,0xE0,0x03,0xFF,0xC0,0x01,0xFF,0x80,0x00,0x7F,0x00,0x00,0x00,0x00,
/*[31] Ю*/0x7D, 0xDF,   0x00,0x00,0x00,0x38,0x00,0x00,0x7C,0x0F,0xC0,0x7C,0x1F,0xF0,0x7C,0x3F,0xF0,0x7C,0x3F,0xF8,0x3C,0x7F,0xF8,0x3C,0x7F,0xFC,0x3C,0xFC,0x7C,0x3C,0xF8,0x1C,0x3C,0xF0,0x1E,0x3C,0xF0,0x0E,0x3F,0xE0,0x0E,0x3F,0xE0,0x0E,0x3C,0xE0,0x0E,0x3C,0xE0,0x0E,0x3C,0xF0,0x0E,0x3C,0xF0,0x1C,0x3C,0xF8,0x3C,0x3C,0x7C,0x7C,0x3C,0x7F,0xF8,0x3C,0x3F,0xF8,0x3C,0x1F,0xF0,0x3E,0x0F,0xE0,0x7E,0x07,0xC0,0x3C,0x00,0x00,0x00,0x00,0x00,
/*[32] Я*/0x1D, 0xBF,   0x00,0x00,0x00,0x00,0xFF,0x80,0x03,0xFF,0xF0,0x07,0xFF,0xF8,0x1F,0xFF,0xFC,0x1F,0xFF,0xFE,0x3F,0xC1,0xFE,0x3F,0x81,0xFE,0x3F,0x81,0xFE,0x1F,0xC1,0xFE,0x1F,0xE1,0xFE,0x0F,0xFF,0xFE,0x07,0xFF,0xFE,0x01,0xFF,0xFE,0x01,0xFF,0xFE,0x03,0xFF,0xFE,0x07,0xFF,0xFE,0x0F,0xF7,0xFE,0x1F,0xE7,0xFE,0x3F,0xE7,0xFE,0x7F,0xC7,0xFE,0x7F,0x87,0xFE,0x7F,0x87,0xFE,0x3F,0x07,0xFC,0x0F,0x07,0xFC,0x06,0x03,0xF8,0x00,0x00,0x00
};

//соответствие кода буквы коду в таблице знакогенератора
static const uint8_t ABC[] =
{
/*00 А*/ 10,
/*01 Б*/ 11,
/*02 В*/ 12,
/*03 Г*/ 13,
/*04 Д*/ 14,
/*05 Е*/ 15,
/*06 Ж*/ 0, //1
/*07 З*/ 0, //2
/*08 И*/ 16,
/*09 Й*/ 0, //3
/*10 К*/ 17,
/*11 Л*/ 18,
/*12 М*/ 19,
/*13 Н*/ 20,
/*14 О*/ 21,
/*15 П*/ 22,
/*16 Р*/ 23,
/*17 С*/ 24,
/*18 Т*/ 25,
/*19 У*/ 26,
/*20 Ф*/ 27,
/*21 Х*/ 0, //4
/*22 Ц*/ 28,
/*23 Ч*/ 29,
/*24 Ш*/ 0, //5
/*25 Щ*/ 0, //6
/*26 Ъ*/ 0, //7
/*27 Ы*/ 0, //8
/*28 Ь*/ 30,
/*29 Э*/ 0, //9
/*30 Ю*/ 31,
/*31 Я*/ 32
};

static const uint8_t TDAYS[] =
{
  "\xCF\xCE\xCD\xC5\xC4\xC5\xCB\xDC\xCD\xC8\xCA\0" /* ПОНЕДЕЛЬНИК */ \
  "\xC2\xD2\xCE\xD0\xCD\xC8\xCA\0" /* ВТОРНИК */ \
  "\xD1\xD0\xC5\xC4\xC0\0" /* СРЕДА */ \
  "\xD7\xC5\xD2\xC2\xC5\xD0\xC3\0" /* ЧЕТВЕРГ */ \
  "\xCF\xDF\xD2\xCD\xC8\xD6\xC0\0" /* ПЯТНИЦА */ \
  "\xD1\xD3\xC1\xC1\xCE\xD2\xC0\0" /* СУББОТА */ \
  "\xC2\xCE\xD1\xCA\xD0\xC5\xD1\xC5\xCD\xDC\xC5\0" /* ВОСКРЕСЕНЬЕ */
};

static const uint8_t TDaysIDx[] = {0, 0, 12, 20, 26, 34, 42, 50};

static const uint8_t TShortDAYS[] = {"\xCF\xCD\0\xC2\xD2\0\xD1\xD0\0\xD7\xD2\0\xCF\xD2\0\xD1\xC1\0\xC2\xD1\0\xD1\xC1\xD0\xCE\xD1\0"};

static const uint8_t TMONTHS[] =
{
  "\xDF\xCD\xC2\xC0\xD0\xDF\0" /* ЯНВАРЯ */ \
  "\xD4\xC5\xC2\xD0\xC0\xCB\xDF\0" /* ФЕВРАЛЯ */ \
  "\xCC\xC0\xD0\xD2\xC0\0" /* МАРТА */ \
  "\xC0\xCF\xD0\xC5\xCB\xDF\0" /* АПРЕЛЯ */ \
  "\xCC\xC0\xDF\0" /* МАЯ */ \
  "\xC8\xDE\xCD\xDF\0" /* ИЮНЯ */ \
  "\xC8\xDE\xCB\xDF\0" /* ИЮЛЯ */ \
  "\xC0\xC2\xC3\xD3\xD1\xD2\xC0\0" /* АВГУСТА */ \
  "\xD1\xC5\xCD\xD2\xDF\xC1\xD0\xDF\0" /* СЕНТЯБРЯ */ \
  "\xCE\xCA\xD2\xDF\xC1\xD0\xDF\0" /* ОКТЯБРЯ */ \
  "\xCD\xCE\xDF\xC1\xD0\xDF\0" /* НОЯБРЯ */ \
  "\xC4\xC5\xCA\xC0\xC1\xD0\xDF\0" /* ДЕКАБРЯ */
};

static const uint8_t TMonths_IDx[] = {0, 0, 7, 15, 21, 28, 32, 37, 42, 50, 59, 67, 74};

static const uint8_t TMessages[] =
{
  /*000*/ "\xD3\xD1\xD2\xC0\xCD\xCE\xC2\xCA\xC0\0" /* УСТАНОВКА */ \
  /*010*/ "\xD7\xC0\xD1\xCE\xC2\0" /* ЧАСОВ */ \
  /*016*/ "\xCC\xC8\xCD\xD3\xD2\0" /* МИНУТ */ \
  /*022*/ "\xC3\xCE\xC4\xC0\0" /* ГОДА */ \
  /*027*/ "\xCC\xC5\xD1\xDF\xD6\xC0\0" /* МЕСЯЦА */ \
  /*034*/ "\xC4\xCD\xDF\0" /* ДНЯ */ \
  /*038*/ "\xC1\xD3\xC4\xC8\xCB\xDC\xCD\xC8\xCA\xC0\0" /* БУДИЛЬНИКА */  
};