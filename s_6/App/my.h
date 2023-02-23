#ifndef __MY_H
#define __MY_H
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "i2c_hal.h"
#include "lcd.h"


#define KEY1 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)
#define KEY2 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)
#define KEY3 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)
#define KEY4 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)

#define ADDR 0x00	//存储地址

#define C1_STATE 2	//页面2
#define C2_STATE 3
#define C3_STATE 4	//页面2
#define C4_STATE 5

#define KEY_TIME 10
#define ADC_TIME 100
#define LED_TIME 200

typedef struct
{
    uint32_t TASK_KEY;
    uint32_t TASK_ADC;
    uint32_t TASK_LED;
} TASK_TINESTypeDef;



typedef struct
{
    bool KEY1_DOWN_Flag;	//1
    bool KEY2_DOWN_Flag;	//2
    bool KEY3_DOWN_Flag;	//3
    bool KEY4_DOWN_Flag;	//4
    bool ADC_Over_Flag;	//ADC采集完成标志位
    bool MENU_Flag;	//页面标志位
    bool LED_RUN_Flag;	//报警LED开关
    uint8_t TIMER_CHOOSE_Flag;	//时间选择 （1 2 3）
    bool ALARM_Over_Flag;	//闹钟标志位
    bool Storage_Success_Flag;	//存储标志位
} DATA_TypeDef;

typedef struct
{
    float My_ADC;
    float K;	//默认0.1保存到EEPROM
    uint8_t LED_STATE;	//LED状态默认打开ON
    uint8_t temp_hour;	//定时时间缓存
    uint8_t temp_min;
    uint8_t temp_sec;
} PARAMETER_TypeDef;

void LED_function(void);
void LED_Write_all(uint8_t data);
void TASK_TIMES_init(void);
uint32_t get_time_task(uint32_t time);
uint32_t get_time(void);
uint8_t KEY_Proc(void);
void KEY_function(void);
void LCD_function(void);
void LCD_Dis1(void);
void LCD_Dis2(void);
void TASK_Init(void);
void ADC_function(void);
void GetTime_function(void);
void Flag_Run_function(void);
void Alarm_SetTime(uint8_t Hours, uint8_t Minutes, uint8_t Seconds);
void USART1_function(void);
void EEPROM_Read(uint8_t addr, uint8_t *date, uint8_t len);
void EEPROM_Write(uint8_t addr, uint8_t *date, uint8_t len);
void EEPROM_SET_Read(void);
void EEPROM_SET_Write(void);
void HightLight(uint8_t Index,PARAMETER_TypeDef *Sparameter);
#endif