#ifndef __MY_ADC_H
#define __MY_ADC_H
#include "allhead.h"

typedef struct
{
    uint32_t ADC_BUFF[100]; // DMA存储
    float volatile Now_ADC_Value;    // 当前ADC值
    void (*ADC_Convert)(void);  // ADC转换
}myADC_t;

extern myADC_t myADC;

#endif
