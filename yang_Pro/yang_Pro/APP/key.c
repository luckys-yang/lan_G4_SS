/***************************************************************************
 * File: key.c
 * Author: Luckys.
 * Date: 2023/06/13
 * description: 按键
****************************************************************************/
#include "allhead.h"

/*====================================variable definition declaration area BEGIN===================================*/
// 按键键值、抬起一瞬间、按下一瞬间
static uint8_t ucKey_Value,ucKey_Up,ucKey_Down;

/*====================================variable definition declaration area   END===================================*/

/*====================================static function declaration area BEGIN====================================*/

static uint8_t Key_Return_Value(void); // 返回键值
static void Key_Scan(void);   // 按键三行消抖---按键扫描
static void Key_Handler(void);    // 按键处理
/*====================================static function declaration area   END====================================*/

Key_t Key = 
{
    0,
    {FALSE},
    Key_Scan,
    Key_Handler,
};


/*
* @function: Key_Return_Value
* @param: None
* @retval: None
* @brief: 返回键值
*/
static uint8_t Key_Return_Value(void)
{
    if ((!READ_KEY1) || (!READ_KEY2) || (!READ_KEY3) || (!READ_KEY4))
    {
        if (!READ_KEY1)
        {
            return KEY1_DOWN;
        }
        else if (!READ_KEY2)
        {
            return KEY2_DOWN;
        }
        else if (!READ_KEY3)
        {
            return KEY3_DOWN;
        }   
        else if (!READ_KEY4)
        {
            return KEY4_DOWN;
        }                     
    }

    return KEY_NULL;
}

/*
* @function: Key_Scan
* @param: None
* @retval: None
* @brief: 按键三行消抖---按键扫描
*/
static void Key_Scan(void)
{
    static uint8_t uckey_old;

    ucKey_Value = Key_Return_Value();                      // 读取按键的键值
    ucKey_Up = ~ucKey_Value & (uckey_old ^ ucKey_Value); // 按键的上升沿检测 只在按键抬起的瞬间有效 其他时刻都为零无效
    ucKey_Down = ucKey_Value & (uckey_old ^ ucKey_Value); // 按键的下降沿检测 只在按键按下的瞬间有效 其他时刻都为零无效
    uckey_old = ucKey_Value;                          // 记录上一次按键按下后的键值

    if (ucKey_Down) // 当有按键按下时
    {
        Key.vusKey_Timer_Count = 0; // 将计时器清零 从零开始计时 此处使用了基础定时器用于计时
    }

    if (Key.vusKey_Timer_Count < 10) // 如果计时时间小于1s 短按
    {
        switch (ucKey_Up) // 判断按键是否抬起 选择键值执行短按的相应程序
        {
            case KEY1_DOWN:Key.vucKey_Flag_Arr[0] = TRUE;break;
            case KEY2_DOWN:Key.vucKey_Flag_Arr[1] = TRUE;break;
            case KEY3_DOWN:Key.vucKey_Flag_Arr[2] = TRUE;break;
            case KEY4_DOWN:Key.vucKey_Flag_Arr[3] = TRUE;break;
            default:break;
        }
    }
    else // 长按 计时时间超过1s
    {
        // 自己添加
    }
}

/*
* @function: Key_Handler
* @param: None
* @retval: None
* @brief: 按键处理
*/
static void Key_Handler(void)
{
    if (Key.vucKey_Flag_Arr[0])
    {
        myPWM.PA3_Duty += 5;
        if (myPWM.PA3_Duty > 45)
        {
            myPWM.PA3_Duty = 5;
        }
        // 修改占空比
        TIM2->CCR4 = (myPWM.PA3_Duty / 100.f) * (TIM2->ARR);
        // 刷新
        sprintf((char*)Menu.LCD_Display_Buffer[DIS_duty], "PA3_Duty:%.0f%%",myPWM.PA3_Duty);
        Menu.padString((char*)Menu.LCD_Display_Buffer[DIS_duty],20);  
        myPWM.PWM_Write_For_EEPROM();   // 写入EEPROM      
        Key.vucKey_Flag_Arr[0] = FALSE;
    }
    else if (Key.vucKey_Flag_Arr[1])
    {
        myPWM.PA3_Fre += 500;
        if (myPWM.PA3_Fre > 4500)
        {
            myPWM.PA3_Fre = 500;
        }
        // 修改频率占空比
        TIM2->ARR = (1000000.0f / myPWM.PA3_Fre) - 1;
        TIM2->CCR4 = (myPWM.PA3_Duty / 100.0f) * (TIM2->ARR);
        // 刷新
        sprintf((char*)Menu.LCD_Display_Buffer[DIS_fre], "PA3_Fre:%dHz",myPWM.PA3_Fre);
        Menu.padString((char*)Menu.LCD_Display_Buffer[DIS_fre],20);        
        myPWM.PWM_Write_For_EEPROM();   // 写入EEPROM      
        Key.vucKey_Flag_Arr[1] = FALSE;
    }
    else if (Key.vucKey_Flag_Arr[2])
    {
        Led.Led_K4_Flag = FALSE;
        Led.Led_K3_Flag = TRUE;
        Key.vucKey_Flag_Arr[2] = FALSE;
    }
    else if (Key.vucKey_Flag_Arr[3])
    {
        Led.Led_K3_Flag = FALSE;
        Led.Led_K4_Flag = TRUE;
        Key.vucKey_Flag_Arr[3] = FALSE;
    }    
}