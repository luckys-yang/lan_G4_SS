#ifndef __KEY_H
#define __KEY_H
#include "main.h"


#define KEY1	HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)
#define KEY2	HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)
#define KEY3	HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)
#define KEY4	HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)

extern uint16_t Long_Num;
extern uint8_t PWM1_Flag;
extern uint8_t PWM2_Flag;


unsigned char KEY_Scan(void);
void KEY_Proc(void);
#endif
