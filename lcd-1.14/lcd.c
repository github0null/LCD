#include "lcd.h"

#define LCD_CMD 0
#define LCD_DATA 1

#define _SetXCmd 0x2A
#define _SetYCmd 0x2B
#define _wRamCmd 0x2C

#undef TRUE
#define TRUE 1

#undef FALSE
#define FALSE 0

// LCD参数
LCD_Info _lcdInfo;      //管理LCD重要参数
uint16_t _defForeColor; //前景 默认白色
uint16_t _defBkColor;   //背景 默认为黑色

// LCD interface
LCD_DelayCallBk lcd_delay;
LCD_WriteByteCallbk lcd_writeByte;

LCD_GBK_CharInfo *_chineseCharTable;
uint32_t _tableLen;

#define LCD_WriteByte(_dat) lcd_writeByte(_dat)

//------------------------------------------

_LCD_INLINE void _WriteData(uint8_t isData, uint8_t _dat)
{
    LCD_PIN_CS_LOW();

    if (isData)
        LCD_PIN_DC_HIGH();
    else
        LCD_PIN_DC_LOW();

    LCD_WriteByte(_dat);

    LCD_PIN_CS_HIGH();
}

_LCD_INLINE void _WriteRegister(uint8_t addr, uint8_t _dat)
{
    LCD_PIN_CS_LOW();
    LCD_PIN_DC_LOW(); // LCD_CMD
    LCD_WriteByte(addr);
    LCD_PIN_CS_HIGH();

    LCD_PIN_CS_LOW();
    LCD_PIN_DC_HIGH(); // LCD_DATA
    LCD_WriteByte(_dat);
    LCD_PIN_CS_HIGH();
}

_LCD_INLINE void _WriteColor(uint16_t color)
{
    LCD_PIN_CS_LOW();
    LCD_PIN_DC_HIGH(); // LCD_DATA
    LCD_WriteByte((uint8_t)(color >> 8));
    LCD_WriteByte((uint8_t)(color & 0xFF));
    LCD_PIN_CS_HIGH();
}

int32_t _FindCharIndex(char *_wChar)
{
    uint32_t i = 0;

    while (i < _tableLen)
    {
        if (_chineseCharTable[i].index[0] == (uint8_t)_wChar[0] &&
            _chineseCharTable[i].index[1] == (uint8_t)_wChar[1])
        {
            return i;
        }
        i++;
    }
    return -1;
}

#define _PrepareWrite() _WriteData(LCD_CMD, _wRamCmd)

//----------------------------------------

void LCD_Enable(uint8_t isEnable)
{
    if (isEnable)
        LCD_PIN_BLK_HIGH();
    else
        LCD_PIN_BLK_LOW();
}

void LCD_Init(LCD_InitTypedef *initDef)
{
    lcd_delay = initDef->delay;
    lcd_writeByte = initDef->writeDataCallbk;

    LCD_PIN_RST_LOW();
    lcd_delay(50);
    LCD_PIN_RST_HIGH();
    lcd_delay(100);

    LCD_SetDirection(LCD_SCREEN_HORIZONTAL);

    _WriteData(LCD_CMD, 0x3A);
    _WriteData(LCD_DATA, 0x05);

    _WriteData(LCD_CMD, 0xB2);
    _WriteData(LCD_DATA, 0x0C);
    _WriteData(LCD_DATA, 0x0C);
    _WriteData(LCD_DATA, 0x00);
    _WriteData(LCD_DATA, 0x33);
    _WriteData(LCD_DATA, 0x33);

    _WriteData(LCD_CMD, 0xB7);
    _WriteData(LCD_DATA, 0x35);

    _WriteData(LCD_CMD, 0xBB);
    _WriteData(LCD_DATA, 0x19);

    _WriteData(LCD_CMD, 0xC0);
    _WriteData(LCD_DATA, 0x2C);

    _WriteData(LCD_CMD, 0xC2);
    _WriteData(LCD_DATA, 0x01);

    _WriteData(LCD_CMD, 0xC3);
    _WriteData(LCD_DATA, 0x12);

    _WriteData(LCD_CMD, 0xC4);
    _WriteData(LCD_DATA, 0x20);

    _WriteData(LCD_CMD, 0xC6);
    _WriteData(LCD_DATA, 0x0F);

    _WriteData(LCD_CMD, 0xD0);
    _WriteData(LCD_DATA, 0xA4);
    _WriteData(LCD_DATA, 0xA1);

    _WriteData(LCD_CMD, 0xE0);
    _WriteData(LCD_DATA, 0xD0);
    _WriteData(LCD_DATA, 0x04);
    _WriteData(LCD_DATA, 0x0D);
    _WriteData(LCD_DATA, 0x11);
    _WriteData(LCD_DATA, 0x13);
    _WriteData(LCD_DATA, 0x2B);
    _WriteData(LCD_DATA, 0x3F);
    _WriteData(LCD_DATA, 0x54);
    _WriteData(LCD_DATA, 0x4C);
    _WriteData(LCD_DATA, 0x18);
    _WriteData(LCD_DATA, 0x0D);
    _WriteData(LCD_DATA, 0x0B);
    _WriteData(LCD_DATA, 0x1F);
    _WriteData(LCD_DATA, 0x23);

    _WriteData(LCD_CMD, 0xE1);
    _WriteData(LCD_DATA, 0xD0);
    _WriteData(LCD_DATA, 0x04);
    _WriteData(LCD_DATA, 0x0C);
    _WriteData(LCD_DATA, 0x11);
    _WriteData(LCD_DATA, 0x13);
    _WriteData(LCD_DATA, 0x2C);
    _WriteData(LCD_DATA, 0x3F);
    _WriteData(LCD_DATA, 0x44);
    _WriteData(LCD_DATA, 0x51);
    _WriteData(LCD_DATA, 0x2F);
    _WriteData(LCD_DATA, 0x1F);
    _WriteData(LCD_DATA, 0x1F);
    _WriteData(LCD_DATA, 0x20);
    _WriteData(LCD_DATA, 0x23);

    _WriteData(LCD_CMD, 0x51); // set brightness
    _WriteData(LCD_DATA, initDef->brightness);

    _WriteData(LCD_CMD, 0x21); // display Inversion on
    _WriteData(LCD_CMD, 0x11); // Exit Sleep
    _WriteData(LCD_CMD, 0x29); // display on

    LCD_Enable(TRUE); // enable lcd display

    LCD_SetChineseCharTable((LCD_GBK_CharInfo *)0x00, 0); // init table as NULL
    LCD_SetDefaultForeColor(initDef->defForegroundColor);
    LCD_SetDefaultBkColor(initDef->defBackgroundColor);
}

LCD_Info *LCD_GetInfo(void)
{
    return &_lcdInfo;
}

void LCD_SetChineseCharTable(LCD_GBK_CharInfo *_table, uint32_t len)
{
    _tableLen = len;
    _chineseCharTable = _table;
}

void LCD_SetWindow(uint16_t x, uint16_t y, uint16_t xEnd, uint16_t yEnd)
{
    switch (_lcdInfo.direction)
    {
    case LCD_SCREEN_HORIZONTAL:
        x += 40;
        xEnd += 40;
        y += 53;
        yEnd += 53;
        break;
    case LCD_SCREEN_ROTATE_90:
        x += 52;
        xEnd += 52;
        y += 40;
        yEnd += 40;
        break;
    case LCD_SCREEN_ROTATE_180:
        x += 40;
        xEnd += 40;
        y += 52;
        yEnd += 52;
        break;
    case LCD_SCREEN_ROTATE_270:
        x += 53;
        xEnd += 53;
        y += 40;
        yEnd += 40;
        break;
    default:
        break;
    }
    
    LCD_Lock();

    _WriteData(LCD_CMD, _SetXCmd);
    _WriteData(LCD_DATA, x >> 8);
    _WriteData(LCD_DATA, 0x00FF & x);
    _WriteData(LCD_DATA, xEnd >> 8);
    _WriteData(LCD_DATA, 0x00FF & xEnd);

    _WriteData(LCD_CMD, _SetYCmd);
    _WriteData(LCD_DATA, y >> 8);
    _WriteData(LCD_DATA, 0x00FF & y);
    _WriteData(LCD_DATA, yEnd >> 8);
    _WriteData(LCD_DATA, 0x00FF & yEnd);

    _PrepareWrite(); //开始写入GRAM
    
    LCD_UnLock();
}

void LCD_SetDirection(LCD_ScreenDirection direction)
{
    LCD_Lock();

    _lcdInfo.direction = direction; // set direction

    switch (direction)
    {
    case LCD_SCREEN_HORIZONTAL:
        _lcdInfo.width = LCD_W;
        _lcdInfo.height = LCD_H;
        _WriteRegister(0x36, 0x70);
        break;
    case LCD_SCREEN_ROTATE_90:
        _lcdInfo.width = LCD_H;
        _lcdInfo.height = LCD_W;
        _WriteRegister(0x36, 0x00);
        break;
    case LCD_SCREEN_ROTATE_180:
        _lcdInfo.width = LCD_W;
        _lcdInfo.height = LCD_H;
        _WriteRegister(0x36, 0xA0);
        break;
    case LCD_SCREEN_ROTATE_270:
        _lcdInfo.width = LCD_H;
        _lcdInfo.height = LCD_W;
        _WriteRegister(0x36, 0xC0);
        break;
    default:
        break;
    }
    
    LCD_UnLock();
}

uint16_t LCD_GetDefaultForeColor(void)
{
    return _defForeColor;
}

void LCD_SetDefaultForeColor(uint16_t foreColor)
{
    _defForeColor = foreColor;
}

uint16_t LCD_GetDefaultBkColor(void)
{
    return _defBkColor;
}

void LCD_SetDefaultBkColor(uint16_t bkColor)
{
    _defBkColor = bkColor;
}

void LCD_WriteData(uint16_t colorData)
{
    LCD_Lock();
    _WriteColor(colorData);
    LCD_UnLock();
}

void LCD_ClearAll(void)
{
    LCD_DrawRect(0, 0, _lcdInfo.width, _lcdInfo.height, _defBkColor);
}

void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_Lock();
    LCD_SetWindow(x, y, x, y);
    _WriteColor(color);
    LCD_UnLock();
}

void LCD_DrawLine(uint16_t x, uint16_t y, uint16_t xEnd, uint16_t yEnd, uint16_t color)
{
    int16_t xLen = (int16_t)xEnd - x, yLen = (int16_t)yEnd - y, offsetX, offsetY;
    float slope = (float)yLen / xLen, temp;

    xLen = xLen >= 0 ? (xLen + 1) : (xLen - 1);
    yLen = yLen >= 0 ? (yLen + 1) : (yLen - 1);

    if (slope == 0)
    {
        if (x <= xEnd)
        {
            LCD_DrawRect(x, y, xLen, 1, color);
        }
        else
        {
            LCD_DrawRect(xEnd, y, -xLen, 1, color);
        }
        return;
    }

    offsetY = 0;
    if (yLen >= 0)
    {
        while (offsetY < yLen)
        {
            temp = offsetY / slope;
            offsetX = temp >= 0.0f ? (temp + 0.55f) : (temp - 0.55f);
            LCD_DrawPoint(offsetX + x, offsetY + y, color);
            offsetY++;
        }
    }
    else
    {
        while (offsetY > yLen)
        {
            temp = offsetY / slope;
            offsetX = temp >= 0.0f ? (temp + 0.55f) : (temp - 0.55f);
            LCD_DrawPoint(offsetX + x, offsetY + y, color);
            offsetY--;
        }
    }
}

void LCD_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    uint32_t i = 0, size = width * height;

    LCD_Lock();

    LCD_SetWindow(x, y, width + x - 1, height + y - 1);
    while (i++ < size)
        _WriteColor(color);
        
    LCD_UnLock();
}

void LCD_DrawChar8x16(uint16_t x, uint16_t y, uint16_t color, Char_8x16 *Char)
{
    uint8_t cChar;
    uint16_t line = 0, column;
    
    LCD_Lock();

    LCD_SetWindow(x, y, x + 7, y + 15);

    while (line < 16)
    {
        cChar = (*Char)[line];
        column = 0;
        while (column < 8)
        {
            _WriteColor((cChar & 0x01) ? color : _defBkColor);
            cChar >>= 1;
            column++;
        }
        line++;
    }
    
    LCD_UnLock();
}

void LCD_DrawChar16x16(uint16_t x, uint16_t y, uint16_t color, Char_16x16 *CharArr)
{
    uint16_t cChar;
    uint16_t line = 0, column;
    
    LCD_Lock();

    LCD_SetWindow(x, y, x + 15, y + 15);

    while (line < 16)
    {
        cChar = (*CharArr)[0][line] | ((*CharArr)[1][line] << 8);
        column = 0;
        while (column < 16)
        {
            _WriteColor((cChar & 0x01) ? color : _defBkColor);
            cChar >>= 1;
            column++;
        }
        line++;
    }
    
    LCD_UnLock();
}

#ifdef USE_ASCII_CHAR_8x16

const uint8_t _char_8x16[][16] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*" ",0*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x10, 0x00, 0x10, 0x18, 0x00, 0x00, 0x00}, /*"!",1*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x2C, 0x24, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*""",2*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x68, 0x24, 0xFE, 0x24, 0x24, 0x24, 0x7E, 0x24, 0x24, 0x00, 0x00, 0x00}, /*"#",3*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x10, 0x7C, 0x16, 0x12, 0x16, 0x38, 0x68, 0x48, 0x48, 0x3E, 0x08, 0x08, 0x00}, /*"$",4*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x86, 0x4B, 0x69, 0x2E, 0x10, 0x08, 0x68, 0x94, 0x92, 0x63, 0x00, 0x00, 0x00}, /*"%",5*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x3C, 0x26, 0x26, 0x1C, 0x4E, 0x52, 0x73, 0x62, 0xFE, 0x00, 0x00, 0x00}, /*"&",6*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*"'",7*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x20, 0x10, 0x18, 0x08, 0x0C, 0x0C, 0x0C, 0x0C, 0x08, 0x08, 0x18, 0x30, 0x20}, /*"(",8*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x04, 0x08, 0x10, 0x10, 0x30, 0x20, 0x20, 0x20, 0x30, 0x10, 0x18, 0x08, 0x04}, /*")",9*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x10, 0x50, 0x2C, 0x38, 0x56, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*"*",10*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0xFE, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00}, /*"+",11*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x10, 0x0C, 0x00}, /*",",12*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*"-",13*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00}, /*".",14*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x40, 0x60, 0x20, 0x30, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04, 0x02, 0x00, 0x00}, /*"/",15*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x3C, 0x66, 0x42, 0xE2, 0xDA, 0xCE, 0x42, 0x66, 0x3C, 0x00, 0x00, 0x00}, /*"0",16*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x18, 0x1E, 0x12, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7E, 0x00, 0x00, 0x00}, /*"1",17*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x3C, 0x62, 0x60, 0x60, 0x20, 0x10, 0x08, 0x04, 0x7E, 0x00, 0x00, 0x00}, /*"2",18*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x3E, 0x60, 0x60, 0x20, 0x3C, 0x40, 0x40, 0x60, 0x3E, 0x00, 0x00, 0x00}, /*"3",19*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x30, 0x38, 0x28, 0x24, 0x26, 0x22, 0xFF, 0x20, 0x20, 0x00, 0x00, 0x00}, /*"4",20*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x7E, 0x06, 0x06, 0x06, 0x7E, 0x40, 0x40, 0x60, 0x3E, 0x00, 0x00, 0x00}, /*"5",21*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x78, 0x0C, 0x06, 0x12, 0x6E, 0x42, 0x42, 0x46, 0x3C, 0x00, 0x00, 0x00}, /*"6",22*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x7E, 0x40, 0x60, 0x20, 0x30, 0x10, 0x18, 0x08, 0x0C, 0x00, 0x00, 0x00}, /*"7",23*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x3C, 0x46, 0x42, 0x6C, 0x38, 0x66, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00}, /*"8",24*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x3C, 0x66, 0x42, 0x42, 0x66, 0x58, 0x40, 0x20, 0x1E, 0x00, 0x00, 0x00}, /*"9",25*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00}, /*":",26*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x10, 0x0C, 0x00}, /*";",27*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x18, 0x0C, 0x06, 0x08, 0x30, 0x60, 0x00, 0x00, 0x00}, /*"<",28*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00}, /*"=",29*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x18, 0x20, 0x60, 0x30, 0x08, 0x04, 0x00, 0x00, 0x00}, /*">",30*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x0C, 0x38, 0x60, 0x60, 0x60, 0x18, 0x08, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00}, /*"?",31*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x38, 0x44, 0x82, 0x82, 0xBB, 0xAD, 0xA5, 0xA5, 0xF5, 0x29, 0x03, 0x02, 0x3C}, /*"@",32*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0x2C, 0x24, 0x64, 0x46, 0x7E, 0xC2, 0x83, 0x00, 0x00, 0x00}, /*"A",33*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x3E, 0x62, 0x42, 0x62, 0x3E, 0x42, 0x42, 0x42, 0x3E, 0x00, 0x00, 0x00}, /*"B",34*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x78, 0x04, 0x02, 0x02, 0x02, 0x02, 0x02, 0x06, 0x7C, 0x00, 0x00, 0x00}, /*"C",35*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x3E, 0x62, 0x42, 0xC2, 0xC2, 0xC2, 0x42, 0x62, 0x1E, 0x00, 0x00, 0x00}, /*"D",36*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x7E, 0x06, 0x06, 0x06, 0x7E, 0x06, 0x06, 0x06, 0x7E, 0x00, 0x00, 0x00}, /*"E",37*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x7E, 0x06, 0x06, 0x06, 0x7E, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00, 0x00}, /*"F",38*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x78, 0x06, 0x02, 0x02, 0x73, 0x42, 0x42, 0x46, 0x7C, 0x00, 0x00, 0x00}, /*"G",39*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00}, /*"H",40*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00, 0x00, 0x00}, /*"I",41*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x3E, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x1E, 0x00, 0x00, 0x00}, /*"J",42*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x42, 0x22, 0x12, 0x0A, 0x0E, 0x1A, 0x32, 0x22, 0x42, 0x00, 0x00, 0x00}, /*"K",43*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x7C, 0x00, 0x00, 0x00}, /*"L",44*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x46, 0x66, 0xE6, 0xFA, 0xDA, 0xDA, 0x83, 0x83, 0x83, 0x00, 0x00, 0x00}, /*"M",45*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x46, 0x46, 0x4E, 0x4A, 0x5A, 0x52, 0x72, 0x62, 0x62, 0x00, 0x00, 0x00}, /*"N",46*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x3C, 0x46, 0xC2, 0xC3, 0xC3, 0xC3, 0xC2, 0x46, 0x3C, 0x00, 0x00, 0x00}, /*"O",47*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x3E, 0x62, 0x42, 0x42, 0x62, 0x1E, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00}, /*"P",48*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x3C, 0x46, 0xC2, 0xC3, 0xC3, 0xC3, 0xC2, 0x46, 0x3C, 0x18, 0xF0, 0x00}, /*"Q",49*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x3E, 0x66, 0x46, 0x66, 0x3E, 0x36, 0x26, 0x66, 0x46, 0x00, 0x00, 0x00}, /*"R",50*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x7C, 0x06, 0x02, 0x06, 0x38, 0x60, 0x40, 0x40, 0x3E, 0x00, 0x00, 0x00}, /*"S",51*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0xFE, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00}, /*"T",52*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x46, 0x3C, 0x00, 0x00, 0x00}, /*"U",53*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x83, 0xC2, 0x42, 0x46, 0x64, 0x24, 0x2C, 0x38, 0x18, 0x00, 0x00, 0x00}, /*"V",54*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x83, 0x83, 0x82, 0x92, 0xDA, 0xDA, 0x6E, 0x66, 0x66, 0x00, 0x00, 0x00}, /*"W",55*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0xC2, 0x66, 0x2C, 0x18, 0x18, 0x38, 0x24, 0x66, 0xC3, 0x00, 0x00, 0x00}, /*"X",56*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x83, 0x42, 0x66, 0x2C, 0x38, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00}, /*"Y",57*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x7E, 0x40, 0x20, 0x30, 0x18, 0x08, 0x04, 0x06, 0x7E, 0x00, 0x00, 0x00}, /*"Z",58*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x38}, /*"[",59*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x02, 0x04, 0x04, 0x08, 0x08, 0x10, 0x10, 0x30, 0x20, 0x60, 0x40, 0x00, 0x00}, /*"\",60*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x3C, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3C}, /*"]",61*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0x24, 0x46, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*"^",62*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF}, /*"_",63*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*"`",64*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x60, 0x40, 0x7C, 0x42, 0x62, 0x5E, 0x00, 0x00, 0x00}, /*"a",65*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x3A, 0x46, 0x42, 0x42, 0x42, 0x42, 0x3E, 0x00, 0x00, 0x00}, /*"b",66*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x04, 0x06, 0x02, 0x06, 0x04, 0x7C, 0x00, 0x00, 0x00}, /*"c",67*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x7C, 0x46, 0x42, 0x42, 0x42, 0x66, 0x5C, 0x00, 0x00, 0x00}, /*"d",68*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x46, 0x42, 0x7E, 0x02, 0x06, 0x7C, 0x00, 0x00, 0x00}, /*"e",69*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0xF0, 0x18, 0x08, 0x08, 0x7E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00}, /*"f",70*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x66, 0x42, 0x66, 0x1A, 0x02, 0x7C, 0xC2, 0x42, 0x3C}, /*"g",71*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x3A, 0x46, 0x42, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00}, /*"h",72*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x1E, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7E, 0x00, 0x00, 0x00}, /*"i",73*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x20, 0x30, 0x00, 0x3E, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x30, 0x1E}, /*"j",74*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x06, 0x06, 0x06, 0x46, 0x36, 0x1E, 0x0E, 0x16, 0x26, 0x46, 0x00, 0x00, 0x00}, /*"k",75*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x1E, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7E, 0x00, 0x00, 0x00}, /*"l",76*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xD2, 0xD2, 0xD2, 0xD2, 0xD2, 0xD2, 0x00, 0x00, 0x00}, /*"m",77*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3A, 0x46, 0x42, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00}, /*"n",78*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x46, 0x42, 0xC2, 0x42, 0x46, 0x3C, 0x00, 0x00, 0x00}, /*"o",79*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3A, 0x46, 0x42, 0x42, 0x42, 0x42, 0x3E, 0x02, 0x02, 0x02}, /*"p",80*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x46, 0x42, 0x42, 0x42, 0x66, 0x5C, 0x40, 0x40, 0x40}, /*"q",81*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x4E, 0xC6, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00, 0x00}, /*"r",82*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x04, 0x04, 0x3C, 0x60, 0x40, 0x3E, 0x00, 0x00, 0x00}, /*"s",83*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x7F, 0x08, 0x08, 0x08, 0x08, 0x08, 0x78, 0x00, 0x00, 0x00}, /*"t",84*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x66, 0x5C, 0x00, 0x00, 0x00}, /*"u",85*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC2, 0x42, 0x66, 0x24, 0x2C, 0x18, 0x18, 0x00, 0x00, 0x00}, /*"v",86*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x83, 0x83, 0xDA, 0x5A, 0x7A, 0x66, 0x66, 0x00, 0x00, 0x00}, /*"w",87*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x64, 0x38, 0x18, 0x38, 0x64, 0x46, 0x00, 0x00, 0x00}, /*"x",88*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC2, 0x42, 0x66, 0x24, 0x2C, 0x38, 0x18, 0x18, 0x0C, 0x07}, /*"y",89*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x60, 0x30, 0x18, 0x08, 0x04, 0x7E, 0x00, 0x00, 0x00}, /*"z",90*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x70, 0x18, 0x08, 0x08, 0x08, 0x0C, 0x0E, 0x08, 0x08, 0x08, 0x08, 0x18, 0x70}, /*"{",91*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10}, /*"|",92*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x0C, 0x18, 0x10, 0x10, 0x10, 0x30, 0x70, 0x10, 0x10, 0x10, 0x10, 0x18, 0x0C}, /*"}",93*/
    /* (8 X 16 , Consolas )*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8E, 0xD2, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00} /*"~",94*/
    /* (8 X 16 , Consolas )*/
};

#define _Char8x16(_char) ((Char_8x16 *)(&_char_8x16[(_char)-32]))

_LCD_INLINE char _ToDisplayableChar(char c)
{
    if (c == '\t')
    {
        return ' ';
    }
    else if (c == '\r' || c == '\n')
    {
        return 0;
    }
    else if (c < 32 || c > 126)
    {
        return '?';
    }
    return c;
}

void LCD_DrawString8x16(uint16_t x, uint16_t y, uint16_t color, char *str)
{
    char _char;
    int32_t _index;
    uint32_t i = 0;

    if (_chineseCharTable != 0x00 && _tableLen > 0)
    {
        while (str[i] != 0x00)
        {
            if ((uint8_t)str[i] >= 128)
            {
                _index = _FindCharIndex(str + i);
                if (_index != -1)
                {
                    LCD_DrawChar16x16(x, y, color, &_chineseCharTable[_index]._data);
                }
                else
                {
                    LCD_DrawChar8x16(x, y, color, _Char8x16('?'));
                    LCD_DrawChar8x16(x + 8, y, color, _Char8x16('?'));
                }
                x += 16;
                i += 2;
            }
            else
            {
                _char = _ToDisplayableChar(str[i]);
                if (_char != 0)
                {
                    LCD_DrawChar8x16(x, y, color, _Char8x16(_char));
                    x += 8;
                }
                i++;
            }
        }
    }
    else
    {
        while (str[i] != 0x00)
        {
            _char = _ToDisplayableChar(str[i]);
            if (_char != 0)
            {
                LCD_DrawChar8x16(x, y, color, _Char8x16(_char));
                x += 8;
            }
            i++;
        }
    }
}

void LCD_DrawAsciiChar8x16(uint16_t x, uint16_t y, uint16_t color, char c)
{
    char _char = _ToDisplayableChar(c);
    if (_char != 0)
    {
        LCD_DrawChar8x16(x, y, color, _Char8x16(_char));
    }
}

#endif
