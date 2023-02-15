#ifndef __MY_LCD_H
#define __MY_LCD_H
#include "allhead.h"


extern uint8_t uart1_scan[20];

void MY_LCD_Dis(void);
void hight_string(uint8_t* str,uint8_t Line,const uint8_t* pos,uint8_t pos_len,uint16_t color,uint16_t hight_color);
void MY_LCD_DisNum(uint32_t num);
void MY_LCD_Arr_Completion(uint8_t *ptr);
void pwm_display(void);
#endif






