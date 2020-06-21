#include "oled.h"

#define OLED_OFF 0xAE
#define OLED_ON 0xAF

#define OLED_SET_DISPLAY_ROW_OFFSET 0xD3
#define OLED_SET_RAM_ADDR_MODE 0x20
#define OLED_SET_DISPLAY_CONTRAST 0x81

#define OLED_CALC_START_ROW(row) (uint8_t)(0x40U | (row & 0x3FU))
#define OLED_CALC_START_PAGE(page) (uint8_t)(0xB0U | (page & 0x07U))

#define OLED_CALC_START_COLUMN_HIGH(col) (uint8_t)(((col & 0xF0U) >> 4) | 0x10)
#define OLED_CALC_START_COLUMN_LOW(col) (uint8_t)(col & 0x0F)

#define _WriteWord(word) _OLED_WriteWord(word)
#define _GetAsciiCode(c) _OLED_GetAsciiCode(c)

uint8_t _x, _page;

void _AddPosition(uint8_t n)
{
    uint8_t i = 0;
    while (i < n)
    {
        if (++_x > 127U)
            _x = 0;
        i++;
    }
}

#define AddPage() (_page = _page >= 7 ? 0 : (_page + 1))

#define _WriteData(_data)                   \
    _WriteWord(0x4000 | ((uint16_t)_data)); \
    _AddPosition(1)

#define _WriteCmd(_cmd) _WriteWord(0x00FF & ((uint16_t)_cmd))

//======================================================================

void OLED_Init(OLED_InitTypeDef *oledDef)
{
    _WriteCmd(OLED_OFF);

    _WriteCmd(0x8D);
    _WriteCmd(0x14);

    _WriteCmd(OLED_SET_RAM_ADDR_MODE);
    _WriteCmd(oledDef->addrMode);

    _WriteCmd(OLED_CALC_START_PAGE(0));

    _WriteCmd(oledDef->columnMode);
    _WriteCmd(oledDef->rowMode);

    _WriteCmd(OLED_SET_DISPLAY_CONTRAST);
    _WriteCmd(oledDef->light);

    _WriteCmd(oledDef->displayMode);
    _WriteCmd(oledDef->screenMode);

    _WriteCmd(OLED_ON);
}

void OLED_SetColumn(uint8_t col)
{
    _WriteCmd(OLED_CALC_START_COLUMN_LOW(col));
    _WriteCmd(OLED_CALC_START_COLUMN_HIGH(col));
    _x = col;
}

void OLED_SetPage(uint8_t p)
{
    _WriteCmd(OLED_CALC_START_PAGE(p));
    _page = p;
}

uint8_t OLED_GetColumn()
{
    return _x;
}

uint8_t OLED_GetPage()
{
    return _page;
}

void OLED_SetColumnAndPage(uint8_t col, uint8_t p)
{
    OLED_SetColumn(col);
    OLED_SetPage(p);
}

void OLED_WriteData(uint8_t _data, uint8_t x, uint8_t page)
{
    OLED_SetColumnAndPage(x, page);
    _WriteData(_data);
}

void OLED_WriteCommand(uint8_t _cmd)
{
    _WriteCmd(_cmd);
}

void OLED_WriteData_8x16(OLED_Data8x16 arr)
{
    uint8_t x = _x, page = _page;
    OLED_WriteDataArray(arr[0], 8);
    OLED_SetColumnAndPage(x, page + 1);
    OLED_WriteDataArray(arr[1], 8);
    OLED_SetPage(page);
}

void OLED_WriteData_16x16(OLED_Data16x16 arr)
{
    uint8_t x = _x, page = _page;
    OLED_WriteDataArray(arr[0], 16);
    OLED_SetColumnAndPage(x, page + 1);
    OLED_WriteDataArray(arr[1], 16);
    OLED_SetPage(page);
}

void OLED_WriteDataArray(uint8_t *arr, uint8_t len)
{
    uint8_t i = 0;
    while (i < len)
    {
        _WriteWord(0x4000 | (uint16_t)arr[i++]);
    }
    _AddPosition(len);
}

void OLED_ClearEndOfPage(uint8_t page)
{
    uint8_t x = _x;

    OLED_SetPage(page);

    while (_x > 0)
    {
        _WriteData(0x00);
    }

    OLED_SetColumn(x);
}

void OLED_ClearPage(uint8_t page)
{
    uint8_t i, prevCol = _x, prevPage = _page;
    
    OLED_SetColumnAndPage(0, page);

    for (i = 0; i < 128; i++)
    {
        _WriteData(0x00);
    }

    OLED_SetColumnAndPage(prevCol, prevPage);
}

void OLED_ClearAll(void)
{
    uint8_t i = 0;

    for (i = 0; i < 8; i++)
    {
        OLED_ClearPage(i);
    }
}

#ifdef OLED_USE_GBK_CHAR
#define _GetGbkCode(c1, c2) _OLED_GetGbkCode(c1, c2)
uint8_t _firstByte;
#endif

void OLED_putchar(char _char)
{
    if (_char == '\n')
    {
        AddPage();
        AddPage();
        OLED_SetColumn(0);
        return;
    }

#ifdef OLED_USE_GBK_CHAR
    if ((uint8_t)_char > 0x80 && _firstByte == 0)
    {
        _firstByte = (uint8_t)_char;
    }
    else if (_firstByte != 0)
    {
        OLED_WriteData_16x16(*_GetGbkCode(_firstByte, (uint8_t)_char));
        _firstByte = 0;
    }
    else
    {
        OLED_WriteData_8x16(*_GetAsciiCode(_char));
    }
#else
    OLED_WriteData_8x16(*_GetAsciiCode(_char));
#endif
}
