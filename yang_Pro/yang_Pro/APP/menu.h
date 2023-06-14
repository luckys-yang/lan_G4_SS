#ifndef __MENU_H
#define __MENU_H
#include "allhead.h"

typedef enum
{
    DIS_title = (uint8_t)1,
    DIS_rtc = (uint8_t)3,
    DIS_fre = (uint8_t)5,
    DIS_duty = (uint8_t)7,
    DIS_adc = (uint8_t)9,
}Menu_Display_Line_t;

typedef struct
{
    uint8_t LCD_Display_Buffer[10][20]; // LCD显示数组
    void (*Menu_Display)(void); // 主页面显示
    void (*padString)(char *, int); // 补全空格
}Menu_t;

extern Menu_t Menu;

#endif
