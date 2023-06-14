/***************************************************************************
 * File: my_pwm.c
 * Author: Luckys.
 * Date: 2023/06/13
 * description: 描述
 -----------------------------------
注意：PA3输出频率和占空比，用示波器测量即可
 -----------------------------------
****************************************************************************/
#include "allhead.h"

/*====================================static function declaration area BEGIN====================================*/

static void PWM_Write_For_EEPROM(void); // 写入频率占空比
static void PWM_Read_For_EEPROM(void);  // 上电读取频率占空比

/*====================================static function declaration area   END====================================*/
myPWM_t myPWM = 
{
    2500,
    10,
    PWM_Write_For_EEPROM,
    PWM_Read_For_EEPROM,
};

/*
* @function: PWM_Write_For_EEPROM
* @param: None
* @retval: None
* @brief: 写入频率占空比
*/
static void PWM_Write_For_EEPROM(void)
{
    uint8_t Send_Arr[3] = {0};

    Send_Arr[0] = myPWM.PA3_Fre & 0xFF;  // 频率低8位
    Send_Arr[1] = (myPWM.PA3_Fre >> 8) & 0xFF;  // 频率高8位
    Send_Arr[2] = myPWM.PA3_Duty;    // 占空比

    EEPROM.EEPROM_Write(STORAGE_ADDR, (uint8_t*)Send_Arr,3);
}

/*
* @function: PWM_Read_For_EEPROM
* @param: None
* @retval: None
* @brief: 上电读取频率占空比
*/
static void PWM_Read_For_EEPROM(void)
{
    uint8_t Rec_Arr[3] = {0};

    EEPROM.EEPROM_Read(STORAGE_ADDR,(uint8_t*)Rec_Arr,3);
    myPWM.PA3_Fre = (uint16_t)(Rec_Arr[1] << 8) | Rec_Arr[0];   // 合并
    myPWM.PA3_Duty = Rec_Arr[2];
    // 更新频率占空比
    TIM2->ARR = (1000000.0f / myPWM.PA3_Fre) - 1;
    TIM2->CCR4 = (myPWM.PA3_Duty / 100.0f) * (TIM2->ARR);    
}