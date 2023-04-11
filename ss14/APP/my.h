#ifndef __MY_H
#define __MY_H
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "lcd.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define KEY1 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)
#define KEY2 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)
#define KEY3 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)
#define KEY4 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)
#define TEST 0	//打印测试

typedef struct
{
    uint8_t HightLight_Flag;	//高亮标志位默认R
    float Adc_Value;	//ADC值
    bool LCD_OVER_FLAG;	//LCD刷新标志位
    uint8_t Key_Down_State[8];	//按键状态
    bool Adc_Over_Flag;	//ADC接收完成标志位
    uint8_t H_or_L_State;	//高低频状态
    bool Fre_5s_Flag;	//频率5s标志位
    uint8_t Lock_State;	//是否锁标志位
    uint8_t Lcd_Dis_Choose_State;	//界面显示状态
} MYDATA_TypeDef;

typedef struct
{
    uint16_t PA1_Fre;	//PA1输出频率
    uint8_t Data_P;	//实时占空比
    float Data_V;	//实时速度
    uint8_t Para_R;	//参数R
    uint8_t Para_K;	//参数K
    uint16_t Recd_N;	//输出模式切换次数
    float Recd_MH;	//输出模式下高频模式速度最大值
    float Recd_ML;	//输出模式下低频模式速度最小值

} PARAMETER_TypeDef;

typedef struct
{
    bool IC_OVER_FLAG;	//计算速度完成标志位
    uint32_t One_Value;	//第1次
    uint32_t Two_Value;	//第2次
    uint32_t Three_Value;	//第3次
    uint32_t Four_Value;	//第4次
    uint16_t Fre_Value;	//频率
    float Duty_Value;	//占空比
    uint8_t IC_RUN_STATE;	//运行状态
} PWMDATA_TypeDef;


extern MYDATA_TypeDef MyData;
extern PARAMETER_TypeDef ParameterData;
extern PWMDATA_TypeDef PwmData;

void vLed_Control2(uint8_t swch, uint8_t num);
void vAdc_Get_function(void);
void vRun_Flag_function(void);
uint8_t ucKey_Scan(void);
void vKey_function(void);
void vHardware_Init(void);
void vLed_Control(uint8_t led_num);
void vLcd_Display_function(void);
#endif