#include "LCD1602.h"

#define true 1
#define false 0

DelayUsFunc _DelayUs;

void Lcd1602_Init(DelayUsFunc _delayUs)
{
    _DelayUs = _delayUs;

    Lcd1602_SetMode(
        LCD1602_MODE_SCREEN_NOT_MOV |
        LCD1602_MODE_CURSOR_MOV_RIGHT);

    Lcd1602_SetDisplayMode(
        LCD1602_DISP_ENABLE |
        LCD1602_DISP_NO_CURSOR);

    Lcd1602_SetFunctionMode(
        LCD1602_FUNC_MODE_5x7_CHAR |
        LCD1602_FUNC_MODE_TWO_LINE);
}

void Lcd1602_Write(bool _isData, uint8_t _data)
{
    DB = _data;
    RS = _isData;
    RW = 0;
    EN = 1;
    _DelayUs(1);
    EN = 0;
    _DelayUs(50);
}

void Lcd1602_SetMode(uint8_t _mode_macro)
{
    Lcd1602_Write(false, 0x04 | _mode_macro);
}

void Lcd1602_SetDisplayMode(uint8_t _disp_macro)
{
    Lcd1602_Write(false, 0x08 | _disp_macro);
}

void Lcd1602_MoveCursor(uint8_t offset, uint8_t _direction)
{
    uint8_t i, _mode = 0x10 | _direction;
    for (i = 0; i < offset; i++)
    {
        Lcd1602_Write(false, _mode);
    }
}

void Lcd1602_MoveChars(uint8_t offset, uint8_t _direction)
{
    uint8_t i, _mode = 0x18 | _direction;
    for (i = 0; i < offset; i++)
    {
        Lcd1602_Write(false, _mode);
    }
}

void Lcd1602_SetFunctionMode(uint8_t _func_macro)
{
    Lcd1602_Write(false, 0x30 | _func_macro);
}

void Lcd1602_Display(uint8_t x, uint8_t y, uint8_t _char)
{
    int addr = (y == 1 ? 0xC0 : 0x80) + x;
    Lcd1602_Write(false, addr);
    Lcd1602_Write(true, _char);
}

void Lcd1602_DisplayArray(uint8_t x, uint8_t y, uint8_t str[], uint8_t len)
{
    uint8_t i = 0, baseAddr = (y == 1 ? 0xC0 : 0x80) + x;

    for (i = 0; i < len; i++)
    {
        Lcd1602_Write(false, i + baseAddr);
        Lcd1602_Write(true, str[i]);
    }
}