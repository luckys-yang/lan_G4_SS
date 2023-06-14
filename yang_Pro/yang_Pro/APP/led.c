/***************************************************************************
 * File: led.c
 * Author: Luckys.
 * Date: 2023/06/13
 * description: LED
****************************************************************************/
#include "allhead.h"

/*====================================static function declaration area BEGIN====================================*/

static void Led_ON(Led_Num_t);   // 打开
static void Led_OFF(Led_Num_t);  // 关闭
static void Led_Flip(Led_Num_t); // 翻转

/*====================================static function declaration area   END====================================*/

Led_t Led = 
{
    FALSE,
    FALSE,
    FALSE,
    Led_ON,
    Led_OFF,
    Led_Flip,
};

/*
* @function: Led_ON
* @param: LEDx -> x：1~8 
* @retval: None
* @brief: 打开
*/
static void Led_ON(Led_Num_t LEDx)
{
    HAL_GPIO_WritePin(GPIOC, (uint16_t)LEDx << 8, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

/*
* @function: Led_OFF
* @param: LEDx -> x：1~8
* @retval: None
* @brief: 关闭
*/
static void Led_OFF(Led_Num_t LEDx)
{
    HAL_GPIO_WritePin(GPIOC, (uint16_t)LEDx << 8, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);    
}

/*
* @function: Led_Flip
* @param: LEDx -> x：1~8
* @retval: None
* @brief: 翻转
*/
static void Led_Flip(Led_Num_t LEDx)
{
    HAL_GPIO_TogglePin(GPIOC, (uint16_t)LEDx << 8);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);    
}