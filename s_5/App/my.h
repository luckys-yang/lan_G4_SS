#ifndef __MY_H
#define __MY_H
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "lcd.h"
#include "i2c_hal.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"


#define KEY1 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)
#define KEY2 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)
#define KEY3 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)
#define KEY4 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)

#define ADDR 0x00

#define C1_STATE 2
#define C2_STATE 4
#define C3_STATE 6
#define C4_STATE 8
#define C5_STATE 9

#define KEY_TIME 10	//按键扫描时间
#define PWMIC_TIME 100	//PWM捕获时间

typedef struct
{
    uint32_t CH1_pluse_val;	//通道1总频率
    uint32_t CH1_duty;	//通道1有效电平值
    uint32_t CH2_pluse_val;	//通道2总频率
    uint32_t CH2_duty;	//通道2有效电平值
} PWM_OCTypeDef;

typedef struct
{
    bool KEY1_DOWN_Flag;	//按键1
    bool KEY2_DOWN_Flag;	//按键2
    bool KEY3_DOWN_Flag;	//按键3
    bool KEY4_DOWN_Flag;	//按键4
    bool UARTorLOCAL_Flag;	//串口还是本地切换标志位
    bool CH_SWITCH_Flag;	//通道切换
} DATA_TypeDef;

typedef struct
{
    uint32_t CH2_IC_freq;	//输入捕获通道2的频率
    uint32_t CH3_IC_freq;	//输入捕获通道3的频率
    uint8_t Mult_CH2;	//通道2倍频数(默认2)
    uint8_t Mult_CH3;	//通道3倍频数
} PWM_ICTypeDef;

typedef struct
{
    uint32_t CH_ONE;	//记录4次的值
    uint32_t CH_TWO;
    uint32_t CH_THREE;
    uint32_t CH_FOUR;
    uint8_t RUN_FLAG;	//运行标志位
} PWM_PARAMETER_TypeDef;

typedef struct
{
    uint32_t TASK_KEY;
    uint32_t TASK_TIMIC;
} TASK_TIMESTypeDef;

void TASK_init(void);
void LCD_Dis(void);
void LCD_function(void);
void LED_Write_all(uint8_t num);
uint32_t get_time_task(uint32_t time);
uint32_t get_time(void);
void TASK_TIME_init(void);
void KEY_function(void);
uint8_t KEY_Proc(void);
void EEPROM_Read(uint8_t addr, uint8_t *date, uint8_t len);
void EEPROM_Write(uint8_t addr, uint8_t *date, uint8_t len);
void PWM_Read(void);
void PWM_Wirte(void);
void USART1_function(void);
void FlagRun_function(void);
void LED_Dis(uint8_t num, uint8_t swch);
void TIM_IN_function(void);
void Add_Space(uint8_t date[][20], uint8_t index);
void PWM_SET_PA7(uint32_t freq, uint16_t duty);
void PWM_SET_PA6(uint32_t freq, uint16_t duty);
void GPIO_OUT(uint16_t pin,GPIO_PinState PinState);
void PWM_OC(uint16_t pin);

#endif