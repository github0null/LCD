#ifndef _H_LCD
#define _H_LCD

#ifdef __cplusplus
extern "C" {
#endif

//=================== LCD interface ==================

#include <lcd_conf.h>
#include <stdint.h>

#ifndef LCD_PIN_CS_HIGH
#error "LCD_PIN_CS_HIGH() must implement"
#endif

#ifndef LCD_PIN_CS_LOW
#error "LCD_PIN_CS_LOW() must implement"
#endif

#ifndef LCD_PIN_RST_HIGH
#error "LCD_PIN_RST_HIGH() must implement"
#endif

#ifndef LCD_PIN_RST_LOW
#error "LCD_PIN_RST_LOW() must implement"
#endif

#ifndef LCD_PIN_DC_HIGH
#error "LCD_PIN_DC_HIGH() must implement"
#endif

#ifndef LCD_PIN_DC_LOW
#error "LCD_PIN_DC_LOW() must implement"
#endif

#ifndef LCD_PIN_BLK_HIGH
#error "LCD_PIN_BLK_HIGH() must implement"
#endif

#ifndef LCD_PIN_BLK_LOW
#error "LCD_PIN_BLK_LOW() must implement"
#endif

#ifndef __STATIC_INLINE
#define __STATIC_INLINE
#endif

//定义LCD的尺寸
#define LCD_W 240U
#define LCD_H 135U

//画笔颜色

#define LCD_COLOR_FROM_RGB(_rgb) ((((_rgb) >> 8U) & 0xF800U) | (((_rgb) >> 5U) & 0x07E0U) | (((_rgb) >> 3U) & 0x001FU))

#define LCD_COLOR_WHITE 0xFFFF
#define LCD_COLOR_BLACK 0x0000
#define LCD_COLOR_BLUE 0x001F
#define LCD_COLOR_BRED 0XF81F
#define LCD_COLOR_GRED 0XFFE0
#define LCD_COLOR_GBLUE 0X07FF
#define LCD_COLOR_RED 0xF800
#define LCD_COLOR_MAGENTA 0xF81F
#define LCD_COLOR_GREEN 0x07E0
#define LCD_COLOR_CYAN 0x7FFF
#define LCD_COLOR_YELLOW 0xFFE0
#define LCD_COLOR_BROWN 0XBC40 //棕色
#define LCD_COLOR_BRRED 0XFC07 //棕红色
#define LCD_COLOR_GRAY 0X8430  //灰色

#define LCD_COLOR_DARKBLUE 0X01CF  //深蓝色
#define LCD_COLOR_LIGHTBLUE 0X7D7C //浅蓝色
#define LCD_COLOR_GRAYBLUE 0X5458  //灰蓝色

#define LCD_COLOR_LIGHTGREEN 0X841F //浅绿色
#define LCD_COLOR_LIGHTGRAY 0XEF5B  //浅灰色(PANNEL)
#define LCD_COLOR_LGRAY 0XC618      //浅灰色(PANNEL),窗体背景色

#define LCD_COLOR_LGRAYBLUE 0XA651 //浅灰蓝色(中间层颜色)
#define LCD_COLOR_LBBLUE 0X2B12    //浅棕蓝色(选择条目的反色)

// 定义液晶屏顺时针旋转方向
typedef enum {
    LCD_SCREEN_HORIZONTAL = 2,
    LCD_SCREEN_ROTATE_90 = 1,
    LCD_SCREEN_ROTATE_180 = 3,
    LCD_SCREEN_ROTATE_270 = 0
} LCD_ScreenDirection;

//-------------

// LCD 信息
typedef struct
{
    uint16_t id;       //LCD ID
    uint16_t width;    //LCD 宽度
    uint16_t height;   //LCD 高度
    LCD_ScreenDirection direction;
} LCD_Info;

typedef void (*LCD_WriteByteCallbk)(uint8_t dat);
typedef void (*LCD_DelayCallBk)(uint16_t ms);

typedef uint8_t Char_8x16[16];
typedef uint8_t Char_16x16[2][16];

typedef struct 
{
    uint8_t index[2];
    Char_16x16 _data;
} LCD_GBK_CharInfo;

typedef struct
{
    // --- LCD options

    uint16_t defForegroundColor;
    
    uint16_t defBackgroundColor;

    uint8_t brightness; // 0x00-0xff

    // --- interface implement

    LCD_DelayCallBk delay;

    LCD_WriteByteCallbk writeDataCallbk;

} LCD_InitTypedef;

//------

void LCD_Init(LCD_InitTypedef *);

void LCD_Enable(uint8_t isEnable);

LCD_Info *LCD_GetInfo(void);

void LCD_SetWindow(uint16_t x, uint16_t y, uint16_t xEnd, uint16_t yEnd);

void LCD_SetDirection(LCD_ScreenDirection direction);

void LCD_SetChineseCharTable(LCD_GBK_CharInfo *_table, uint32_t len);

uint16_t LCD_GetDefaultForeColor(void);

void LCD_SetDefaultForeColor(uint16_t foreColor);

uint16_t LCD_GetDefaultBkColor(void);

void LCD_SetDefaultBkColor(uint16_t bkColor);

void LCD_ClearAll(void);

void LCD_WriteData(uint16_t colorData);

void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);

void LCD_DrawLine(uint16_t x, uint16_t y, uint16_t xEnd, uint16_t yEnd, uint16_t color);

void LCD_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

void LCD_DrawChar8x16(uint16_t x, uint16_t y, uint16_t color, Char_8x16 *Char);

void LCD_DrawChar16x16(uint16_t x, uint16_t y, uint16_t color, Char_16x16 *arr);

#ifdef USE_ASCII_CHAR_8x16

void LCD_DrawString8x16(uint16_t x, uint16_t y, uint16_t color, char *str);

void LCD_DrawAsciiChar8x16(uint16_t x, uint16_t y, uint16_t color, char _char);

#endif

#ifdef __cplusplus
}
#endif

#endif
