#ifndef __MY_H
#define __MY_H
#include "main.h"
#include "gpio.h"
#include "rtc.h"
#include "tim.h"
#include "my.h"
#include "lcd.h"

#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
/***************************宏定义*****************************/

//LCD Display
#define LCD_SPACE0	0
#define LCD_SPACE1	1
#define LCD_TEST	2
#define LCD_SPACE3	3
#define LCD_SPACE4	4
#define LCD_NUMBER	5
#define LCD_SPACE6	6
#define LCD_SPACE7	7
#define LCD_RTC	8
#define LCD_SPACE9	9

//时间片
#define KEY_TIMEST	10	//10ms扫描一次
#define RTC_TIMEST  200	//200ms扫描一次

//按键读取电平状态
#define KEY1 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)	//PB0
#define KEY2 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)	//PB1
#define KEY3 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)	//PB2
#define KEY4 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)	//PA0

//有误差故加多点
#define KEY_OVER_TIME	1000	//按键按完需要等待的时间
#define SJJ_OC_OVER_TIME	4000	//升降机开关门需要的时间
#define SJJ_UD_OVER_TIME	6000	//升降机上下需要的时间
#define SJJ_WAIT_OVER_TIME	2000	//升降机停留需要的时间



/***************************变量*****************************/

//时间片结构体
typedef struct
{
    uint8_t KEY_time;
    uint8_t RTC_TIME;
} TASK_TIMESTypeDef;

//系统数据
typedef struct
{
    uint8_t Platform;	//当前平台(1~4)
    bool LED1_State;	//LED1状态(代表4个平台)
    bool LED2_State;	//LED2状态
    bool LED3_State;	//LED3状态
    bool LED4_State;	//LED4状态

    bool KEY_Timer_Start;	//按键按下 开始计时标志位
    uint16_t KEY_Timer_Count;	//按键按下后计时时间
    bool KEY_Timer_Over;	//按键按下计时 完成标志位
    bool KEY_InFore_Flag;	//按键是否有效(电机运行期间无效 1:有效 0:无效)

    bool SJJ_Close_Start;	//升降机开始关门 标志位
    bool SJJ_Close_Over;	//升降机关门结束 标志位
    bool SJJ_Open_Start;	//升降机开始开门 标志位
    bool SJJ_Open_Over;	//升降机开门结束 标志位
    uint16_t SJJ_OpenClose_Timer_Count;	//升降机开门/关门计时时间

    uint16_t SJJ_Wait_Timer_Count;	//升降机停留计时时间
    bool SJJ_Wait_Start;	//开始停留
    bool SJJ_Wait_Over;	//停留结束

    bool SJJ_UP_Start;	//升降机开始上升 标志位
    bool SJJ_UP_Over;	//升降机上升完成 标志位
    bool SJJ_DOWN_Start;	//升降机开始下降 标志位
    bool SJJ_DOWN_Over;	//升降机下降完成 标志位
    uint16_t SJJ_UpDown_Timer_Count;	//升降机上下计时时间

    bool Platform_Over;	//到达目标平台标志位
    bool Platform_NumberBlink_Flag;	//数字闪烁
    bool Platform_UPorDOWN;	//选择 1:上-->下/0:下-->上扫描 标志位
    uint8_t PWM_Out_UpDown_Flag;	//上下电机pwm输出切换标志位
    uint8_t PWM_Out_OpenClose_Flag;	//开关电机pwm输出切换标志位
} DATA_TypeDef;


extern DATA_TypeDef Data;

extern TASK_TIMESTypeDef TASK_TIMES;

extern uint8_t LCD_Line_BUFF[10][20];	//LCD显示数组



/***************************函数*****************************/

void LCD_function(void);
void LCD_DrawMonoPict1(uint16_t Xpos, uint16_t Ypos, uint32_t BackColor, uint32_t TextColor, uint8_t *Pict);
void TASK_Init(void);
void RTC_function(void);
void PWM_SET_PA6(uint32_t freq, uint16_t duty);
void PWM_SET_PA7(uint32_t freq, uint16_t duty);
void KEY_SCAN(void);
uint8_t KEY_pro(void);
void LED_DIS(uint8_t num, uint8_t swch);
void Flag_Run(void);
void SJJ_UP(void);
uint8_t Check(void);
void SJJ_DOWN(void);
uint8_t Check_DOWN(void);
uint8_t Check_UP(void);
#endif
