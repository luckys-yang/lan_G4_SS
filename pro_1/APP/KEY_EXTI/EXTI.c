#include "allhead.h"

//外部中断函数在stm32g4xx_hal_gpio.c里面有弱函数定义
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_0)	//如果是PB0按下
	{
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0) == 0)
		{
			light2468_open(1);
		}
	}
	if(GPIO_Pin == GPIO_PIN_1)	//如果是PB1按下
	{
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) == 0)
		{
			light2468_open(0);
		}		
	}
	if(GPIO_Pin == GPIO_PIN_2)	//如果是PB2按下
	{
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2) == 0)
		{
			lightAll_open(1);
		}		
	}
	
}
