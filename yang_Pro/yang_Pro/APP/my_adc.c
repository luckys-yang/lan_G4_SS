/***************************************************************************
 * File: my_adc.c
 * Author: Luckys.
 * Date: 2023/06/13
 * description: ADC
****************************************************************************/
#include "allhead.h"

/*====================================static function declaration area BEGIN====================================*/

static void ADC_Convert(void);  // ADC转换+滤波

/*====================================static function declaration area   END====================================*/
myADC_t myADC = 
{
    {0},
    0.0,
    ADC_Convert,
};

/*
* @function: ADC_Convert
* @param: None
* @retval: None
* @brief: ADC转换+滤波
*/
static void ADC_Convert(void)
{
    uint32_t adc = 0, temp;
    uint8_t count;

    HAL_ADC_Stop_DMA(&hadc2);          // 停止ADC
    for (uint8_t i = 0; i < 100 - 1; i++)
    {
        count = 0;
        for (uint8_t j = 0; j < 100 - 1 - i; j++)
        {
            if (myADC.ADC_BUFF[j] > myADC.ADC_BUFF[j + 1])
            {
                temp = myADC.ADC_BUFF[j];
                myADC.ADC_BUFF[j] = myADC.ADC_BUFF[j + 1];
                myADC.ADC_BUFF[j + 1] = temp;
                count++;
            }
        }
        if (0 == count)
        {
            break;
        }
    }

    for (uint8_t k = 1; k < 100 - 1; k++)
    {
        adc += myADC.ADC_BUFF[k];
    }
    HAL_ADC_Start_DMA(&hadc2, (uint32_t *)&myADC.ADC_BUFF, 100);
    myADC.Now_ADC_Value = (adc / 98) * 3.3f / 4096;
    // 刷新屏幕
    sprintf((char*)Menu.LCD_Display_Buffer[DIS_adc], "ADC:%.1fV", myADC.Now_ADC_Value);
    Menu.padString((char*)Menu.LCD_Display_Buffer[DIS_adc],20);
}

