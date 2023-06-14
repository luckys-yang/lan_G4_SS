#include "allhead.h"

#define DELAY_TIME 1000

//流水灯
void LED_TEST(void)
{
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);	//翻转电平
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);	//拉高
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);	//拉低
    HAL_Delay(DELAY_TIME);

    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_Delay(DELAY_TIME);

    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_Delay(DELAY_TIME);

    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_11);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_Delay(DELAY_TIME);

    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_12);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_Delay(DELAY_TIME);

    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_Delay(DELAY_TIME);

    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_14);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_Delay(DELAY_TIME);

    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_15);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_Delay(DELAY_TIME);
}

//枚举
enum {
	START_OPEN_1_3_5_7,	//打开1,3,5LED
	START_OPEN_2_4_6_8,	//打开2,4,6LED
	START_OPEN_ALL,	//打开所有灯
};

//定义一开始状态(看你想上电后先亮哪几个)
uint8_t startNow = START_OPEN_ALL;

void LED_Dis1(void)
{
	switch(startNow)
	{
		case START_OPEN_ALL:	//如果当前状态是所有灯亮则
			startNow = START_OPEN_1_3_5_7;	//状态改成1,3,5,7亮
			lightAll_open(0);	//关闭所有灯
			light1357_open(1);	//点亮1357
		break;
		case START_OPEN_1_3_5_7:
			startNow = START_OPEN_2_4_6_8;
			light1357_open(0);
			light2468_open(1);
		break;
		case START_OPEN_2_4_6_8:
			startNow = START_OPEN_ALL;
			lightAll_open(1);
		break;
	}
}
//点亮/关闭1357
void light1357_open(uint8_t mode)
{
	if(1 == mode)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8|GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_14,GPIO_PIN_RESET);	//点亮1,3,5,7
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8|GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_14,GPIO_PIN_SET);	//关闭1,3,5,7
	}
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);	//拉高
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);	//拉低
}
//点亮/关闭2468
void light2468_open(uint8_t mode)
{
	if(1 == mode)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9|GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_15,GPIO_PIN_RESET);	//点亮2,4,6,8
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9|GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_15,GPIO_PIN_SET);	//关闭2,4,6,8
	}
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);	//拉高
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);	//拉低
}
//点亮/关闭所有
void lightAll_open(uint8_t mode)
{
	if(1 == mode)
	{
		HAL_GPIO_WritePin(GPIOC,0xFF00,GPIO_PIN_RESET);	//点亮所有
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC,0xFF00,GPIO_PIN_SET);	//关闭所有
	}
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);	//拉高
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);	//拉低
}

//蜂鸣器【参数：1表示打开蜂鸣器 0表示关闭】
void Buzzer_test(uint8_t swch)
{
	if(SET == swch)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);	//打开蜂鸣器
	}
	else if(RESET == swch)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);	//关闭蜂鸣器		
	}
}
