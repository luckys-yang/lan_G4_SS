#ifndef __MY_H
#define __MY_H
#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "lcd.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"
#include "i2c_hal.h"
#include "stdlib.h"

#define KEY1 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)
#define KEY2 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)
#define KEY3 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)
#define KEY4 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)

//每页的首地址 = （页数*每页多少字节） --->结果是十进制需要转换为十六进制
#define WZ1_ADDR	0x00	//0*8 = 0
#define WZ2_ADDR	0x08	//1*8 = 8
#define WZ3_ADDR	0x10	//2*8 = 16
#define WZ4_ADDR	0x18	//3*8 = 24
#define WZ5_ADDR	0x20	//4*8 = 32

//LCD Dis
#define WZ_NUM	2	//位置
#define TIME_NUM	5	//时间
#define ZT_NUM	8	//状态

//State
#define STANDBY 1	//停止
#define SETTING 2	//设置
#define RUNNING 3	//运行
#define PAUSE 4	//暂停
#define NULLER 0	//无

//时 分 秒 无 索引
#define H_INDEX	1
#define M_INDEX	2
#define S_INDEX	3
#define NULL_INDEX	0

//时间
typedef struct
{
    uint8_t Hour;
    uint8_t Min;
    uint8_t Sec;
} TIMER_TypeDef;

typedef struct
{
    uint8_t WZ_State;	//当前位置(1~5)
    uint8_t One_Read_State;	//上电读取标志位
    bool KEY1_Short_Flag;	//按键1短按标志位
    bool KEY2_Short_Flag;	//按键2短按标志位
    bool KEY2_Long_Flag;	//按键2长按标志位
    bool KEY3_Short_Flag;	//按键3短按标志位
    bool KEY3_Long_Flag;	//按键3长按标志位
    bool KEY4_Short_Flag;	//按键4短按标志位
    bool KEY4_Long_Flag;	//按键4长按标志位
    uint8_t Now_State;	//当前状态(STANDBY  SETTING  RUNNING  PAUSE  NULLER)
    uint8_t LCD_HIGH_State;	//高亮
    uint16_t TIMERT_1S_Count;	//秒计数
} DATA_TypeDef;

extern DATA_TypeDef Data;




void Flag_function(void);
void LCD_function(void);
void TASK_Init(void);
void LED_DIS(uint8_t num, uint8_t swch);
void EEPROM_Read(uint8_t addr, uint8_t *data, uint8_t len);
void EEPROM_Write(uint8_t addr, uint8_t *Data, uint8_t len);
void Read_Time(TIMER_TypeDef *tim, uint8_t addr);
void Write_Time(TIMER_TypeDef *tim, uint8_t addr);
uint8_t KEY_pro(void);
void KEY_function(void);
void KEY1_Short_function(void);
void KEY2_Short_function(void);
void KEY3_Short_function(void);
void Timer_CountDown(TIMER_TypeDef *timx);
void KEY4_Long_function(TIMER_TypeDef *timx);
#endif


