#ifndef __MY_H
#define __MY_H

#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "lcd.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdbool.h"


#define KEY1 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)
#define KEY2 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)
#define KEY3 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)
#define KEY4 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)

#define BT_STATE 1
#define C1_STATE 3
#define C2_STATE 4
#define C3_STATE 5



typedef struct 
{
	bool KEY1_DOWN_Flag;	//1
	bool KEY2_DOWN_Flag;	//2
	bool KEY3_DOWN_Flag;	//3
	bool KEY4_DOWN_Flag;	//4
	uint8_t mm_1;	//密码第一位
	uint8_t mm_2;	//密码第一位
	uint8_t mm_3;	//密码第一位
	uint16_t freq;	//频率
	float duty;	//占空比
	bool MENU_Flag;	//默认0是密码页面 1是输出页面
	bool Run_Count_5s;	//5秒开始计数标志位
	uint16_t Run_Timer_Count;	//计数值
	bool LED2_Count_5s;	//LED2开始计数标志位
	uint16_t LED2_Timer_Count;	//计数值
	
	
}DATA_TypeDef;

void LCD_Dis2(void);
void LCD_Dis1(void);
void TASK_Init(void);
void LED_DIS(uint8_t num,uint8_t swch);
void KEY_function(void);
uint8_t KEY_Proc(void);
void LCD_function(void);
void Flag_Run_function(void);
void USART1_function(void);
#endif