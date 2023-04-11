#ifndef __PWM_H
#define __PWM_H
#include "main.h"

extern uint16_t pa6_duty;	//PA6通道1初始占空比10%
extern uint16_t pa7_duty;	//PA7通道1初始占空比50%
extern uint16_t pa6_fre;	//定时器初始ARR值
extern uint32_t ch1_pulse_val,ch2_pulse_val;	//通道1的CRRx值   通道2的CRRx值
extern uint32_t ch1_duty_val,ch2_duty_val;	//通道1有效电平占空比所需值   通道2有效电平占空比所需值
extern uint32_t ch1_high,ch2_high;
extern uint32_t ch1_low,ch2_low;

void pA6_SetDuty(uint16_t d);
void pA7_SetDuty(uint16_t d);
void p_SetFre(uint16_t f) ;
void pwm_oc_setCH1(uint32_t freq,uint8_t duty);
void pwm_oc_setCH2(uint32_t freq,uint8_t duty);

#endif
