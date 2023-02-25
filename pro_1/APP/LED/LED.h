#ifndef __LED_H
#define __LED_H
#include "main.h"

#define LED1	{HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_8);\
				 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);\
				 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);}	//LED1µçÆ½·­×ª

				 
void LED_TEST(void);
void LED_Dis1(void);
void light1357_open(uint8_t mode);
void light2468_open(uint8_t mode);				 
void lightAll_open(uint8_t mode);
void Buzzer_test(uint8_t swch);				 
				 
#endif

