#ifndef __MY_RTC_H
#define __MY_RTC_H
#include "allhead.h"

typedef struct
{
    void (*RTC_Get_Time)(void); // 获取时间
}myRTC_t;

extern myRTC_t myRTC;

#endif
