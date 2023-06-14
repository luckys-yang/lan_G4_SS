#ifndef __MY_USART1_H
#define __MY_USART1_H
#include "allhead.h"

// 定义接收发送最大长度
#define UART_Send_MAX   60
#define UART_Rec_MAX    50

// 定义上位机发送过来的协议固定长度
#define UART_Order_LENGTH 6

typedef struct
{
    uint8_t Upload_Data_Flag;   // 上传数据标志位 TRUE--上传 FALSE--不上传
    uint8_t *pucSend_Buffer;   // 发送数据缓存指针
    uint8_t *pucRec_Buffer;    // 接收数据缓存指针
    void (*UART_Send_Arrary)(uint8_t*, uint16_t);   // 发送数组
    void (*UART_Send_String)(uint8_t*);   // 发送字符串
    void (*UART_Protocol_Analysis)(void);   // 协议解析
}myUSART1_t;

extern myUSART1_t myUSART1;

#endif
