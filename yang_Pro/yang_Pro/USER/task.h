#ifndef __TASK_H
#define __TASK_H
#include "allhead.h"

typedef struct
{
    uint8_t Run_Status; // 任务状态：TRUE/FALSE
    uint16_t Task_Cnt;  // 任务定时计数器(ms)
    uint16_t Task_Timer;    // 重载计数器(任务分配的时间ms)
    void (*Task_Hook)(void);    // 任务函数
}Task_t;

extern Task_t Task[];
extern uint8_t ucTasks_Max; // 最大任务数量


#endif
