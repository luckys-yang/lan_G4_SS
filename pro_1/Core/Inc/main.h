/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
typedef struct
{
    uint32_t TIM_ONE;	//第一次上升沿
    uint32_t TIM_TWO;	//第一次下降沿
    uint32_t TIM_THREE;	//第二次上升沿
    uint32_t TIM_FOUR;	//第二次下降沿
	float TIM_zkb;	//占空比
	uint32_t TIM_freq;	//频率
}TIM_ICTypeDef;

extern TIM_ICTypeDef TIM_IC_2;	//TIM2捕获
extern TIM_ICTypeDef TIM_IC_3;	//TIM3捕获
extern uint8_t TIM_IC_flag;	//捕获状态标志位(分时复用)
extern uint8_t TIM_CHx_flag;	//切换定时器标志位
extern uint32_t ADC2_Value[10];
extern uint32_t ADC1_Value[2];
extern uint32_t ADC_1;
extern uint32_t ADC_2;
extern RTC_TimeTypeDef rtc_time;	//获取时间的结构体
extern RTC_DateTypeDef rtc_date;	//获取日期的结构体
extern uint8_t menu_flag;	
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
