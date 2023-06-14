/***************************************************************************
 * File: my_rtc.c
 * Author: Luckys.
 * Date: 2023/06/13
 * description: RTC
****************************************************************************/
#include "allhead.h"

/*====================================variable definition declaration area BEGIN===================================*/

RTC_TimeTypeDef my_time = {0};
RTC_DateTypeDef my_date = {0};

/*====================================variable definition declaration area   END===================================*/

/*====================================static function declaration area BEGIN====================================*/

static void RTC_Get_Time(void);
/*====================================static function declaration area   END====================================*/
myRTC_t myRTC = 
{
    RTC_Get_Time,
};

/*
* @function: RTC_Get_Time
* @param: None
* @retval: None
* @brief: 获取时间
*/
static void RTC_Get_Time(void)
{
    static uint8_t rtc_old_Sec = 5; // 上一次的值
    rtc_old_Sec = my_time.Seconds;
    HAL_RTC_GetTime(&hrtc, &my_time, RTC_FORMAT_BIN);	//获取时间
    HAL_RTC_GetDate(&hrtc, &my_date, RTC_FORMAT_BIN);

    if (rtc_old_Sec != my_time.Seconds) // 刷新
    {
        sprintf((char*)Menu.LCD_Display_Buffer[DIS_rtc], "Time:%d:%d:%d", my_time.Hours, my_time.Minutes, my_time.Seconds);
        Menu.padString((char*)Menu.LCD_Display_Buffer[DIS_rtc],20);
    }
}


