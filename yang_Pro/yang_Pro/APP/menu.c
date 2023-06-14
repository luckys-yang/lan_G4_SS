/***************************************************************************
 * File: menu.c
 * Author: Luckys.
 * Date: 2023/06/13
 * description: 页面
****************************************************************************/
#include "allhead.h"

/*====================================static function declaration area BEGIN====================================*/

static void Menu_Display(void); // 主页面显示
static void padString(char *, int); // 补全空格
/*====================================static function declaration area   END====================================*/
Menu_t Menu = 
{
    {" "},
    Menu_Display,
    padString,
};


/*
* @function: Menu_Display
* @param: None
* @retval: None
* @brief: 主页面显示
*/
static void Menu_Display(void)
{
    LCD_DisplayStringLine(Line1,(uint8_t*)"        Main        ");

    LCD_DisplayStringLine(Line3,(uint8_t*)Menu.LCD_Display_Buffer[DIS_rtc]);

    LCD_DisplayStringLine(Line5,(uint8_t*)Menu.LCD_Display_Buffer[DIS_fre]);

    LCD_DisplayStringLine(Line7,(uint8_t*)Menu.LCD_Display_Buffer[DIS_duty]);

    LCD_DisplayStringLine(Line9,(uint8_t*)Menu.LCD_Display_Buffer[DIS_adc]);
}

/*
* @function: Menu_Display
* @param: str -> 字串符 size -> 最大不能超过多少
* @retval: None
* @brief: 补全空格
*/
static void padString(char *str, int size)
{
    int len = strlen(str);
    if (len >= size)
    {
        return; // 字符串已经够长了，不需要添加空格
    }
    for (int i = len; i < size; i++)
    {
        if (i < size - 1)
        {
            str[i] = ' '; // 在字符串末尾添加空格
        }
        else
        {
            str[i] = '\0'; // 添加字符串结束符
        }
    }
}