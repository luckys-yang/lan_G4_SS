/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "allhead.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define alarm_hour 0
#define alarm_min 0
#define alarm_sec 10
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t ADC2_Value[10];	//一个通道转换30次
uint32_t ADC1_Value[2];	//ADC1
uint32_t ADC_1 = 0;	//求平均值后的值
uint32_t ADC_2 = 0;
RTC_TimeTypeDef rtc_time;
RTC_DateTypeDef rtc_date;
uint8_t tx_buff[]="报警================\r\n  UART1 Test\r\n  1234";
uint8_t alarm_clock_flag = 0;
uint8_t menu_flag = 0;
TIM_ICTypeDef TIM_IC_2;	//TIM2捕获
TIM_ICTypeDef TIM_IC_3;	//TIM3捕获
uint8_t TIM_IC_flag = 0;
uint8_t TIM_CHx_flag = 1;	//先转换TIM2

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Alarm_configuration(uint8_t hour,uint8_t min,uint8_t sec);
void Alarm_compute(uint8_t hour,uint8_t min,uint8_t sec);
void enter_standby_mode(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
//  uint8_t eeprom_num;
//  uint8_t arr[5]= "Fuck";
//  uint8_t arr2[5];
//	uint8_t i;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_DAC1_Init();
  MX_TIM6_Init();
  MX_RTC_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM1_Init();
  MX_TIM7_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim7);	//使能定时器2+中断
  LCD_Init();	//LCD初始化
  lightAll_open(0);	//一开始LED全灭
  //HAL_UART_Receive_IT(&huart1,(uint8_t*)&USART1_NewData,1);
  I2CInit();	//I2C初始化
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(Blue2);
  HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED);    //ADC1校准
  HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);    //ADC2校准 
  HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&ADC1_Value,2);
  HAL_ADC_Start_DMA(&hadc2,(uint32_t*)&ADC2_Value,10);
  HAL_TIM_Base_Start(&htim6);	//开启定时器6
  HAL_DAC_Start(&hdac1,DAC_CHANNEL_1);	//开启DAC通道1
  HAL_DAC_Start(&hdac1,DAC_CHANNEL_2);  //开启DAC通道2
//  set_DAC_value(&hdac1,DAC_CHANNEL_1,2.2);	//设置模数转换后的值为2.2v
  set_DAC_value(&hdac1,DAC_CHANNEL_2,3.1);  //设置模数转换后的值为3.1v
  mcp_write_byte(64);
  HAL_RTC_GetTime(&hrtc,&rtc_time,RTC_FORMAT_BIN);	//获取时间，二进制格式
  HAL_RTC_GetDate(&hrtc,&rtc_date,RTC_FORMAT_BIN);	//获取日期，二进制格式
  HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_1);	//开启定时器8通道1
  HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_2);	//开启定时器8通道2
  pwm_oc_setCH1(9000,70);	//设置频率，占空比
  pwm_oc_setCH2(600,50);	//设置频率，占空比
  HAL_TIM_OC_Start_IT(&htim4,TIM_CHANNEL_1);	//定时器4通道1
  HAL_TIM_OC_Start_IT(&htim4,TIM_CHANNEL_2);	//定时器4通道2
#if 0
  eeprom_write_page(0x08,arr,5);
  HAL_Delay(10);
  eeprom_read_sequential(0x08,arr2,5);
  printf("%s\r\n",arr2);
#endif
  


  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
#if 1
//	  if(RX_flag)
//	  {
//		  RX_flag = 0;
//		  printf("%s",USART1_RX_BUF);
//	  }
	  
	  UART1_function();
		if(0 == TIM_IC_flag)
		{
			TIM_IC_flag++;
			//清0
			if(TIM_CHx_flag)
			{
				TIM_IC_2.TIM_ONE = 0;
				TIM_IC_2.TIM_TWO = 0;
				TIM_IC_2.TIM_THREE = 0;
				TIM_IC_2.TIM_FOUR = 0;
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);	//上升沿捕获
				HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);	//启动输入捕获
			}
			else
			{
				TIM_IC_3.TIM_ONE = 0;
				TIM_IC_3.TIM_TWO = 0;
				TIM_IC_3.TIM_THREE = 0;
				TIM_IC_3.TIM_FOUR = 0;
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);	//上升沿捕获
				HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);	//启动输入捕获
			}
		}
		if(5 == TIM_IC_flag)	//计算
		{
			TIM_IC_flag = 0;
			if(TIM_CHx_flag)
			{
				TIM_IC_2.TIM_zkb = (TIM_IC_2.TIM_FOUR - TIM_IC_2.TIM_THREE) / (float)(TIM_IC_2.TIM_FOUR - TIM_IC_2.TIM_TWO) * 100;	//高电平值/一个周期值*100
				TIM_IC_2.TIM_freq = 1000000 / (TIM_IC_2.TIM_FOUR - TIM_IC_2.TIM_TWO);	//预分频后频率/一个周期值
			}
			else
			{
				TIM_IC_3.TIM_zkb = (TIM_IC_3.TIM_FOUR - TIM_IC_3.TIM_THREE) / (float)(TIM_IC_3.TIM_FOUR - TIM_IC_3.TIM_TWO) * 100;	//高电平值/一个周期值*100
				TIM_IC_3.TIM_freq = 1000000 / (TIM_IC_3.TIM_FOUR - TIM_IC_3.TIM_TWO);	//预分频后频率/一个周期值
			}
			TIM_CHx_flag = !TIM_CHx_flag;	//取反
		}
	    
	  if(PWM1_Flag)	//PWM 【按键3】
	  {
		  PWM1_Flag = 0;
		  p_SetFre(10);	//每次加10%
//		  pA6_SetDuty(100);
	  }
	  if(PWM2_Flag)	//PWM 【按键4】
	  {
		  PWM2_Flag = 0;
		  pA7_SetDuty(100);
	  }	  
		  
	  if(Key_flag)
	  {
		  Key_flag = 0;
		  KEY_Proc();
	  }
	  if(LCD_scan_Flag)
	  {
		  LCD_scan_Flag = 0;
		  if(menu_flag)
		  {
			  MY_LCD_DisNum(Long_Num);
		  }
		  else
		  {
			  pwm_display();
		  }
	  }
	  if(alarm_clock_flag)
	  {
		  alarm_clock_flag = 0;
		  HAL_UART_Transmit_DMA(&huart1,(uint8_t*)"闹钟响了\r\n",12);
	  }
	  
#endif
	  
#if 0

#endif
	  

	 

	  
	  
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

//闹钟配置函数 参数：时 分 秒
void Alarm_configuration(uint8_t hour,uint8_t min,uint8_t sec)
{
	//直接复制 rtc.c 然后把 RTC_FORMAT_BCD 改成 RTC_FORMAT_BIN
	RTC_AlarmTypeDef sAlarm = {0};
	
	sAlarm.AlarmTime.Hours = hour;
	sAlarm.AlarmTime.Minutes = min;
	sAlarm.AlarmTime.Seconds = sec;
	sAlarm.AlarmTime.SubSeconds = 0x0;
	sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = 0x1;
	sAlarm.Alarm = RTC_ALARM_A;
	if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}	
}
void Alarm_compute(uint8_t hour,uint8_t min,uint8_t sec)
{
	uint8_t h = 0,m = 0, s = 0;
	RTC_TimeTypeDef gtime = {0};
	RTC_DateTypeDef gdate = {0};
	
	HAL_RTC_GetTime(&hrtc,&gtime,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&gdate,RTC_FORMAT_BIN);
	
	if((s = (sec + gtime.Seconds)) > 59)	//如果设定值+获取值>59
	{
		s = s % 60;	//取余
		++gtime.Minutes;	//分+1
	}
	if((m = (min + gtime.Minutes)) > 59)	//如果设定值+获取值>59
	{
		m = m % 60;	//取余
		++gtime.Hours;	//时+1
	}
	if((h = (hour + gtime.Hours)) > 23)	//如果设定值+获取值>23
	{
		h = h % 23;	//取余
	}	
	
	Alarm_configuration(h,m,s);
}

//进入待机模式函数
void enter_standby_mode(void)
{
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);	//失能PA0
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);//PA0端口变成低电平，准备好唤醒键初始电平
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);	//清除唤醒标志位
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);	////使能WKUP引脚的唤醒功能
	HAL_PWR_EnterSTANDBYMode();	//进入待机模式
}

//闹钟中断回调函数
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	alarm_clock_flag = 1;	//闹钟标志位
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);//清除待机标志位
	Alarm_compute(alarm_hour,alarm_min,alarm_sec);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

