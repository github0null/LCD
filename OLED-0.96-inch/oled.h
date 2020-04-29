#ifndef _H_OLED_1306
#define _H_OLED_1306

#include <Drivers_config.h>

typedef uint8_t OLED_Data8x16[2][8];
typedef uint8_t OLED_Data16x16[2][16];

typedef void (*WriteWordCallBack)(uint16_t);
typedef OLED_Data8x16 *(*GetAsciiCharCallBk)(char);

#define OLED_DEVICE_ADDR 0x78

#define OLED_MODE_SCREEN_NORMAL 0xA6
#define OLED_MODE_SCREEN_INVERSE 0xA7

#define OLED_MODE_DISPLAY_RESUME_RAM 0xA4
#define OLED_MODE_DISPLAY_IGNORE_RAM 0xA5

#define OLED_MODE_NORMAL_ROW 0xC0
#define OLED_MODE_INVERSE_ROW 0xC8

#define OLED_MODE_NORMAL_COLUMN 0xA0
#define OLED_MODE_INVERSE_COLUMN 0xA1

#define OLED_SET_RAM_ADDR_MODE 0x20
#define OLED_RAM_ADDR_MODE_HORIZONTAL 0x00
#define OLED_RAM_ADDR_MODE_VERTICAL 0x01
#define OLED_RAM_ADDR_MODE_PAGE 0x02

#define OLED_SET_DISPLAY_CONTRAST 0x81

typedef struct
{
    WriteWordCallBack writeWord;
    GetAsciiCharCallBk getAsciiChar;
} OLED_InitTypeDef;

void OLED_Init(OLED_InitTypeDef *initDef);

void OLED_SetColumn(uint8_t x);
void OLED_SetPage(uint8_t y);
void OLED_SetColumnAndPage(uint8_t x, uint8_t page);

void OLED_WriteData(uint8_t _data, uint8_t x, uint8_t page);
void OLED_WriteCommand(uint8_t _cmd);

void OLED_WriteDataArray(uint8_t *arr, uint8_t len);
void OLED_WriteData_8x16(OLED_Data8x16 arr);
void OLED_WriteData_16x16(OLED_Data16x16 arr);

void OLED_ClearEndOfPage(uint8_t page);
void OLED_ClearPage(uint8_t page);
void OLED_ClearAll(void);

uint8_t OLED_GetColumn(void);
uint8_t OLED_GetPage(void);

char OLED_putchar(char _char);

#endif
