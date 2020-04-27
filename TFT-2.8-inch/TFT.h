#ifndef _H_TFT
#define _H_TFT

#include "stdint.h"

//LCD重要参数集
typedef struct
{
    uint16_t id;       //LCD ID
    uint16_t width;    //LCD 宽度
    uint16_t height;   //LCD 高度
    uint8_t direction; //横屏还是竖屏控制：0，竖屏；1，横屏。
} TFT_Info;

#define TFT_SCREEN_HORIZONTAL 0 //定义液晶屏顺时针旋转方向 	0-0度旋转，1-90度旋转，2-180度旋转，3-270度旋转
#define TFT_SCREEN_ROTATE_90 1
#define TFT_SCREEN_ROTATE_180 2
#define TFT_SCREEN_ROTATE_270 3

//定义LCD的尺寸
#define TFT_W 240U
#define TFT_H 320U

//-----------------------Pin-define-------------

#include "BITBAND.h"

#define TFT_Pin_RST(val) PAout(0) = (val)
#define TFT_Pin_DC(val) PAout(1) = (val)
#define TFT_BKLight_CTRL(val) PAout(2) = (val)
#define TFT_Pin_CS(val) PAout(3) = (val) 

//画笔颜色

#define TFT_COLOR_FROM_RGB(_rgb) ((((_rgb) >> 8U) & 0xF800U) | (((_rgb) >> 5U) & 0x07E0U) | (((_rgb) >> 3U) & 0x001FU))

#define TFT_COLOR_WHITE 0xFFFF
#define TFT_COLOR_BLACK 0x0000
#define TFT_COLOR_BLUE 0x001F
#define TFT_COLOR_BRED 0XF81F
#define TFT_COLOR_GRED 0XFFE0
#define TFT_COLOR_GBLUE 0X07FF
#define TFT_COLOR_RED 0xF800
#define TFT_COLOR_MAGENTA 0xF81F
#define TFT_COLOR_GREEN 0x07E0
#define TFT_COLOR_CYAN 0x7FFF
#define TFT_COLOR_YELLOW 0xFFE0
#define TFT_COLOR_BROWN 0XBC40 //棕色
#define TFT_COLOR_BRRED 0XFC07 //棕红色
#define TFT_COLOR_GRAY 0X8430  //灰色

#define TFT_COLOR_DARKBLUE 0X01CF  //深蓝色
#define TFT_COLOR_LIGHTBLUE 0X7D7C //浅蓝色
#define TFT_COLOR_GRAYBLUE 0X5458  //灰蓝色

#define TFT_COLOR_LIGHTGREEN 0X841F //浅绿色
#define TFT_COLOR_LIGHTGRAY 0XEF5B  //浅灰色(PANNEL)
#define TFT_COLOR_LGRAY 0XC618      //浅灰色(PANNEL),窗体背景色

#define TFT_COLOR_LGRAYBLUE 0XA651 //浅灰蓝色(中间层颜色)
#define TFT_COLOR_LBBLUE 0X2B12    //浅棕蓝色(选择条目的反色)

#define TFT_CMD 0
#define TFT_DATA 1

typedef void (*WriteByteCallBk)(uint8_t _byte);
typedef void (*DelayCallBk)(uint32_t ms);

typedef uint8_t Char_8x16[16];
typedef uint8_t Char_16x16[2][16];

typedef struct 
{
    uint8_t index[2];
    Char_16x16 _data;
} TFT_CharInfo;

//------

void TFT_Init(DelayCallBk delayCallBk, WriteByteCallBk writeByteCallBk);

TFT_Info *TFT_GetInfo(void);

void TFT_SetWindow(uint16_t x, uint16_t y, uint16_t xEnd, uint16_t yEnd);

void TFT_SetDirection(uint8_t direction);

void TFT_SetChineseCharTable(TFT_CharInfo *_table, uint32_t len);

uint16_t TFT_GetDefaultForeColor(void);

void TFT_SetDefaultForeColor(uint16_t foreColor);

uint16_t TFT_GetDefaultBkColor(void);

void TFT_SetDefaultBkColor(uint16_t bkColor);

void TFT_ClearAll(void);

void TFT_DrawPoint(uint16_t x, uint16_t y, uint16_t color);

void TFT_DrawLine(uint16_t x, uint16_t y, uint16_t xEnd, uint16_t yEnd, uint16_t color);

void TFT_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

void TFT_DrawChar8x16(uint16_t x, uint16_t y, uint16_t color, Char_8x16 *Char);

void TFT_DrawChar16x16(uint16_t x, uint16_t y, uint16_t color, Char_16x16 *arr);

#ifdef USE_ASCII_CHAR_8x16

void TFT_DrawString8x16(uint16_t x, uint16_t y, uint16_t color, char *str);

void TFT_DrawAsciiChar8x16(uint16_t x, uint16_t y, uint16_t color, char _char);

#endif

#endif
