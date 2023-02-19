#ifndef __MY_H
#define __MY_H
#include "main.h"
#include "gpio.h"
#include "i2c_hal.h"
#include "adc.h"
#include "tim.h"
#include "lcd.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"

#define KEY_TIME 20
#define ADC_TIME 50


#define KEY1 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)
#define KEY2 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)
#define KEY3 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)
#define KEY4 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)

#define BT_State 0
#define C1_State 2
#define C2_State 4


typedef struct
{
    uint16_t KEY_TIMER_COUNT;
    uint16_t ADC_TIMER_COUNT;
} TASK_TIMERTypeDef;

typedef struct
{
    bool KEY1_DOWN_Flag;	//按键1
    bool KEY2_DOWN_Flag;	//按键2
    bool KEY3_DOWN_Flag;	//按键3
    bool KEY4_DOWN_Flag;	//按键4
    bool MODE_Flag;	//模式 0：自动 1：手动
    uint8_t MENU_Flag;	//页面1:0 页面2:1
    bool SWITCH_Flag;	//切换频率标志位
} DATA_TypeDef;

typedef struct
{
    uint16_t PA6_duty;	//手动模式下PA6占空比
    uint16_t PA7_duty;	//手动模式下PA7占空比
    uint16_t auto_duty1;	//自动模式下的占空比1
    uint16_t auto_duty2;	//自动模式下的占空比2
    float ADC_value;	//ADC最终值
} PWM_DUTY_TypeDef;


void PWM_OUT(void);
void Flag_Run_fuction(void);
void LCD_Dis1(void);
void LCD_Dis2(void);
void LCD_function(void);
void Get_ADC(void);
uint8_t KEY_Pro(void);
void KEY_function(void);
void TASK_Init(void);
void LED_DIS(uint8_t num, uint8_t swch);

#endif
