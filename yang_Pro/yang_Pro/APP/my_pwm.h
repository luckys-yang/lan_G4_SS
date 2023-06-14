#ifndef __MY_PWM_H
#define __MY_PWM_H
#include "allhead.h"

typedef struct
{
    uint16_t PA3_Fre;
    float PA3_Duty;
    void (*PWM_Write_For_EEPROM)(void); // 写入频率占空比
    void (*PWM_Read_For_EEPROM)(void);  // 上电读取频率占空比
}myPWM_t;

extern myPWM_t myPWM;

#endif
