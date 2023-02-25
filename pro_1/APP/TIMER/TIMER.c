#include "allhead.h"

uint8_t Time7_1s_Flag = 0;
uint32_t Sys_Count = 0;
uint16_t LCD_NUM = 0;
uint8_t KEY_1sFlag = 0;
uint8_t LCD_scan_Flag = 0;
uint8_t ADC_flag = 0;
uint8_t Key_flag = 0;
uint8_t pwm_ic_flag = 0;


extern uint8_t KEY_Time_Count;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint16_t count_100ms = 0;
	static uint16_t count_10ms = 0;
	static uint16_t count_50ms = 0;
	static uint16_t count_80ms = 0;	
	static uint16_t count_1s = 0;	
	static uint16_t count_200ms = 0;	
	
	if(htim == &htim7)	//如果是TIM2产生中断
	{	
		count_100ms++;
		count_10ms++;
		count_50ms++;
		count_80ms++;
		count_1s++;
		count_200ms++;
		if(10 == count_10ms)
		{
			count_10ms = 0;
			Key_flag = 1;
		}
		if(100 == count_100ms)
		{
			count_100ms = 0;
			KEY_Time_Count++;
		}
		if(200 == count_200ms)
		{
			count_200ms = 0;
			pwm_ic_flag = 1;
		}
		if(50 == count_50ms)
		{
			count_50ms = 0;
			LCD_scan_Flag = 1;
		}
		if(80 == count_80ms)
		{
			count_80ms = 0;
			ADC_flag = 1;
		}
		if(1000 == count_1s)	//1s获取一次日期时间
		{
			count_1s = 0;
			HAL_RTC_GetTime(&hrtc,&rtc_time,RTC_FORMAT_BIN);	//获取时间，二进制格式
		    HAL_RTC_GetDate(&hrtc,&rtc_date,RTC_FORMAT_BIN);	//获取日期，二进制格式
		}
		
	}
}


