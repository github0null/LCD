#include "oled.h"

#define OLED_OFF 0xAE
#define OLED_ON 0xAF

#define OLED_SET_DISPLAY_ROW_OFFSET 0xD3

#define OLED_CALC_START_ROW(row) (uint8_t)(0x40U | (row & 0x3FU))
#define OLED_CALC_START_PAGE(page) (uint8_t)(0xB0U | (page & 0x07U))

#define OLED_CALC_START_COLUMN_HIGH(col) (uint8_t)(((col & 0xF0U) >> 4) | 0x10)
#define OLED_CALC_START_COLUMN_LOW(col) (uint8_t)(col & 0x0F)

WriteWordCallBack _WriteWord;
GetAsciiCharCallBk _getChar8x16;
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

#define _WriteData(_data)                   \
    _WriteWord(0x4000 | ((uint16_t)_data)); \
    _AddPosition(1)

#define _WriteCmd(_cmd) _WriteWord(0x00FF & ((uint16_t)_cmd))

void OLED_Init(OLED_InitTypeDef *initDef)
{
    _WriteWord = initDef->writeWord;
    _getChar8x16 = initDef->getAsciiChar;

    _WriteCmd(OLED_OFF); //关闭显示

    _WriteCmd(0x8D); //电荷泵设置
    _WriteCmd(0x14);

    _WriteCmd(OLED_SET_RAM_ADDR_MODE);
    _WriteCmd(OLED_RAM_ADDR_MODE_PAGE);

    _WriteCmd(OLED_CALC_START_PAGE(0));

    _WriteCmd(OLED_MODE_INVERSE_COLUMN);
    _WriteCmd(OLED_MODE_INVERSE_ROW);

    _WriteCmd(OLED_SET_DISPLAY_CONTRAST);
    _WriteCmd(0xEF);

    _WriteCmd(OLED_MODE_DISPLAY_RESUME_RAM);
    _WriteCmd(OLED_MODE_SCREEN_NORMAL);

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
    int8_t x = _x;

    OLED_SetPage(page);

    while (_x > 0)
    {
        _WriteData(0x00);
    }

    OLED_SetColumn(x);
}

void OLED_ClearPage(uint8_t page)
{
    uint8_t i;

    OLED_SetColumnAndPage(0, page);

    for (i = 0; i < 128; i++)
    {
        _WriteData(0x00);
    }
}

void OLED_ClearAll(void)
{
    uint8_t i = 0;

    for (i = 0; i < 8; i++)
    {
        OLED_ClearPage(i);
    }
}

char OLED_putchar(char _char)
{
    OLED_Data8x16 *cDat = _getChar8x16(_char);
    OLED_WriteData_8x16(*cDat);
    return _char;
}

