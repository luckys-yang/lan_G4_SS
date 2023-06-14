#ifndef __SYSTEM_H
#define __SYSTEM_H
#include "allhead.h"

// 首次上电需要提前写入
#define POWER_ONE 0

typedef struct
{
    void (*Hardware_Init)(void);    // 硬件初始化
    void (*System_Run)(void);  // 系统运行
    void (*Task_Marks_Handler)(void);   // 任务标记
}System_t;


extern System_t System;

#endif
