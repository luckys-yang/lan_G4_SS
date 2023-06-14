#ifndef __KEY_H
#define __KEY_H
#include "allhead.h"

// 读取按键电平
#define READ_KEY1 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)
#define READ_KEY2 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)
#define READ_KEY3 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)
#define READ_KEY4 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)

typedef enum
{
    KEY_NULL  = (uint8_t)0x00,  // 无按键按下键值
    KEY1_DOWN = (uint8_t)0x01,  // 按键1按下键值  
    KEY2_DOWN = (uint8_t)0x02,  // 按键2按下键值
    KEY3_DOWN = (uint8_t)0x03,  // 按键3按下键值
    KEY4_DOWN = (uint8_t)0x04,  // 按键4按下键值
}Key_Status_t;

typedef struct
{
    uint16_t volatile vusKey_Timer_Count; // 长按计数
    uint8_t volatile vucKey_Flag_Arr[4];  // 按键标志位(短按)
    void (*Key_Scan)(void);   // 按键三行消抖---按键扫描
    void (*Key_Handler)(void);    // 按键处理
}Key_t;

extern Key_t Key;

#endif
