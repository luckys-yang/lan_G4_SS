#ifndef __LED_H
#define __LED_H
#include "allhead.h"

// 定义枚举类型
typedef enum
{
    LED1 = 0x01,
    LED2 = 0x02,
    LED3 = 0x04,
    LED4 = 0x08,
    LED5 = 0x10,
    LED6 = 0x20,
    LED7 = 0x40,
    LED8 = 0x80,
    LED_ALL_ON = 0x00,
    LED_ALL_OFF = 0xFF,       
} Led_Num_t;

typedef struct
{
    uint8_t Led_Uart_Rx_Flag;   // 接收到串口数据
    uint8_t Led_K3_Flag; // K3按下
    uint8_t Led_K4_Flag; // K4按下
    void (*Led_ON)(Led_Num_t);   // 打开
    void (*Led_OFF)(Led_Num_t);  // 关闭
    void (*Led_Flip)(Led_Num_t); // 翻转    
}Led_t;


extern Led_t Led;

#endif
