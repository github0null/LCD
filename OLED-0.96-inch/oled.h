#ifndef _H_OLED_1306
#define _H_OLED_1306

#ifdef __cplusplus
extern "C" {
#endif

//============= interface ==============

#include <stdint.h>

typedef uint8_t OLED_Data8x16[2][8];
typedef uint8_t OLED_Data16x16[2][16];

/* must implement this method */
extern void _OLED_WriteWord(uint16_t _word);

/* must implement this method */
extern OLED_Data8x16 *_OLED_GetAsciiCode(char c);

#ifdef OLED_USE_GBK_CHAR
/* implement this method to support chinese char */
extern OLED_Data16x16 *_OLED_GetGbkCode(uint8_t byte1, uint8_t byte2);
#endif

//======================================

#define OLED_DEVICE_ADDR 0x78

typedef enum {
    OLED_MODE_SCREEN_NORMAL = 0xA6,
    OLED_MODE_SCREEN_INVERSE = 0xA7
} OLED_ScreenMode;

typedef enum {
    OLED_MODE_DISPLAY_RESUME_RAM = 0xA4,
    OLED_MODE_DISPLAY_IGNORE_RAM = 0xA5
} OLED_DisplayMode;

typedef enum {
    OLED_MODE_NORMAL_ROW = 0xC0,
    OLED_MODE_INVERSE_ROW = 0xC8
} OLED_RowMode;

typedef enum {
    OLED_MODE_NORMAL_COLUMN = 0xA0,
    OLED_MODE_INVERSE_COLUMN = 0xA1
} OLED_ColumnMode;

typedef enum {
    OLED_RAM_ADDR_MODE_HORIZONTAL = 0x00,
    OLED_RAM_ADDR_MODE_VERTICAL = 0x01,
    OLED_RAM_ADDR_MODE_PAGE = 0x02
} OLED_AddressMode;

typedef struct
{
    OLED_ScreenMode screenMode;
    OLED_DisplayMode displayMode;
    OLED_RowMode rowMode;
    OLED_ColumnMode columnMode;
    OLED_AddressMode addrMode;
    uint8_t light; // default 0xEF
} OLED_InitTypeDef;

void OLED_Init(OLED_InitTypeDef *oledDef);

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

void OLED_putchar(char _char);

#ifdef __cplusplus
}
#endif

#endif
