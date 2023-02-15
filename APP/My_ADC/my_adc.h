#ifndef __MY_ADC_H
#define __MY_ADC_H
#include "main.h"

void ADC_DMA_Test(void);
float Adc_Proc(uint32_t _buf[]);
double getADC(ADC_HandleTypeDef *hadc);
void set_DAC_value(DAC_HandleTypeDef *hdac,uint32_t Channel,float Vol);
#endif






