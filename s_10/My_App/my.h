#ifndef __MY_H
#define __MY_H

#include "main.h"
#include "adc.h"
#include "tim.h"
#include "gpio.h"
#include "i2c_hal.h"
#include "lcd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

//对应行数

#define BT_State 2
#define C1_State 3
#define C2_State 4
#define C3_State 5
#define C4_State 6

//按键电平读取
#define KEY1 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)
#define KEY2 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)
#define KEY3 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)
#define KEY4 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)

#define DATA_ADDR 0x08	//第8页开始存储参数

#define KEY_TIME 20	//20ms扫描一次
#define ADC_TIME 100	//100ms读取一次

//参数结构体
typedef struct
{
    uint8_t UP_LED;	//超出上限指示灯(1~8)
    uint8_t DOWN_LED;	//超出下限指示灯(1~8)
    float V_UP;	//电压上限(保留一位小数)
    float V_DOWN;	//电压下限(保留一位小数)
    float Now_V;	//当前电压
} PARAMETER_TypeDef;

//时间扫描结构体
typedef struct
{
    uint16_t KEY_TIME_COUNT;	//按键扫描
    uint16_t ADC_TIME_COUNT;	//ADC扫描
} TASK_TypeDef;

//数据结构体
typedef struct
{
    bool KEY1_DOWN_Flag;	//按键1按下标志位
    bool KEY2_DOWN_Flag;	//按键2按下标志位
    bool KEY3_DOWN_Flag;	//按键3按下标志位
    bool KEY4_DOWN_Flag;	//按键4按下标志位
    bool MENU_State;	//页面1,2切换标志位
    bool One_Flag;	//是否第一次上电标志位
    uint8_t High_Line_Flag;	//高亮选择
    bool UP_LED_Flag;	//上限指示灯标志位
    bool DOWN_LED_Flag;	//下限指示灯标志位
    bool LED_SCAN_Flag;	//LED闪烁标志位
} DATA_TypeDef;



void LED_DIS(uint8_t num, uint8_t swch);	//LED显示
void TASK_Init(void);	//初始化
void LCD_function(void);	//LCD检测刷新
void KEY_function(void);	//按键检测
uint8_t KEY_Pro(void);	//按键读取
void EEPROM_Page_Write(uint8_t addr, uint8_t *date, uint8_t len);	//EEPROM页写
void EEPROM_Page_Read(uint8_t addr, uint8_t *date, uint8_t len);	//EEPROM页读
void Parameter_Read(PARAMETER_TypeDef *para);	//读取EEPROM参数
void Parameter_Write(PARAMETER_TypeDef *para);	//写入EEPROM参数
void LCD_Dis1(void);	//主页面
void LCD_Dis2(void);	//参数页面
void Run_function(void);	//标志位执行功能函数
void GetAdc(void);	//获取ADC
void Add_V(PARAMETER_TypeDef *para, uint8_t dir);	//增加电压
void Add_LED(PARAMETER_TypeDef *para, uint8_t dir);	//增加LED
void Arr_KG(uint8_t arr[][20], uint8_t NUM);	//添加空格
void Reduce_LED(PARAMETER_TypeDef *para, uint8_t dir);	//减小电压
void Reduce_V(PARAMETER_TypeDef *para, uint8_t dir);	//减小LED
void LED_Togg(uint8_t num);	//LED翻转
void Judge_function(void);	//判断是否超出限制函数
void LED_Flicker(void);	//LED闪烁执行

#endif
