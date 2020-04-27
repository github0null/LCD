//--------pin-define----------
#define DB
#define RS 
#define RW
#define EN
//------------------------

#include "stdint.h"

#define bool uint8_t

typedef void (*DelayUsFunc)(unsigned int us);

#define LCD1602_DIRECTION_LEFT 0x00
#define LCD1602_DIRECTION_RIGHT 0x04

//---------------

#define LCD1602_MODE_CURSOR_MOV_RIGHT 0x02
#define LCD1602_MODE_CURSOR_MOV_LEFT 0x00

#define LCD1602_MODE_SCREEN_MOV_RIGHT 0x01
#define LCD1602_MODE_SCREEN_NOT_MOV 0x00

//-------------------

#define LCD1602_DISP_DISABLE 0x00
#define LCD1602_DISP_ENABLE 0x04

#define LCD1602_DISP_NO_CURSOR 0x00
#define LCD1602_DISP_USE_CURSOR 0x02

#define LCD1602_DISP_FLICKER_CURSOR 0x00
#define LCD1602_DISP_NOT_FLICKER_CURSOR 0x01

//--------------------------

#define LCD1602_FUNC_MODE_ONE_LINE 0x00
#define LCD1602_FUNC_MODE_TWO_LINE 0x08

#define LCD1602_FUNC_MODE_5x7_CHAR 0x00
#define LCD1602_FUNC_MODE_5x10_CHAR 0x04

//--------------------------

void Lcd1602_Init(DelayUsFunc _delayUs);

void Lcd1602_Write(bool _isData, uint8_t _data);

void Lcd1602_SetMode(uint8_t _mode_macro);

void Lcd1602_SetDisplayMode(uint8_t _disp_macro);

void Lcd1602_MoveCursor(uint8_t offset, uint8_t _direction);

void Lcd1602_MoveChars(uint8_t offset, uint8_t _direction);

void Lcd1602_SetFunctionMode(uint8_t _func_macro);

void Lcd1602_Display(uint8_t x, uint8_t y, uint8_t _char);

void Lcd1602_DisplayArray(uint8_t x, uint8_t y, uint8_t str[], uint8_t len);