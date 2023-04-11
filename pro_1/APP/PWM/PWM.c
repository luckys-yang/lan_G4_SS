#include "allhead.h"

/*====================PWM输出模式测试区=======================*/
//使用定时器8通道1,2
uint16_t pa6_duty = 100;	//PA6通道1初始占空比10%
uint16_t pa7_duty = 500;	//PA7通道1初始占空比50%
uint16_t pa6_fre = 1000;	//定时器初始ARR值

//PA6设置占空比函数 参数：占空比值：100~900
void pA6_SetDuty(uint16_t d)
{
	pa6_duty+=d;
	
	if(pa6_duty > 900)
	{
		pa6_duty = 100;
	}
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pa6_duty);	//设置占空比
}

//PA7设置占空比函数 参数：占空比值：100~900
void pA7_SetDuty(uint16_t d)
{
	pa7_duty+=d;
	
	if(pa7_duty > 900)
	{
		pa7_duty = 100;
	}
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pa7_duty);	//设置占空比
}

//定时器3设置频率函数 参数：每次增加多少 单位 %
void p_SetFre(uint16_t f) 
{
	uint16_t p_arr;
	uint32_t p_fre = 80000000/(31 + 1)/(htim8.Instance->ARR + 1);	//计算频率
	float p_temp1 = (float)f/100;	//百分数转小数形式【不加类型转换则变成0】
	float temp2 = p_fre*p_temp1;	//算出需要加多少
	p_fre += temp2;	//在原有基础上相加
	p_arr = (80000000/(31 + 1))/p_fre;	//算出需要的ARR
	
	if(p_fre > (2500+(2500*0.9)))	//超过初值的0.9倍则回到初值频率，即ARR回到初值即可
	{
		p_arr = 1000;
	}
	__HAL_TIM_SetAutoreload(&htim8,p_arr - 1);	//设置ARR
}




/*====================PWM输出比较模式测试区=======================*/

uint32_t ch1_pulse_val,ch2_pulse_val;	//通道1的CRRx值   通道2的CRRx值
uint32_t ch1_duty_val,ch2_duty_val;	//通道1有效电平占空比所需值   通道2有效电平占空比所需值
uint32_t ch1_high,ch2_high;
uint32_t ch1_low,ch2_low;

//设置通道1频率，占空比 参数1：频率Hz  参数2：占空比%
void pwm_oc_setCH1(uint32_t freq,uint8_t duty)
{
	ch1_pulse_val = 1000000 / freq;	//算出频率所需的CRRx值
	ch1_duty_val = ch1_pulse_val * duty / 100;	//算出有效电平所需值
	htim4.Instance->CCR1 = ch1_pulse_val;
}

//设置通道2频率，占空比 参数1：频率Hz  参数2：占空比%
void pwm_oc_setCH2(uint32_t freq,uint8_t duty)
{
	ch2_pulse_val = 1000000 / freq;	//算出总频率所需的CRRx值
	ch2_duty_val = ch2_pulse_val * duty / 100;	//算出有效电平所需值
	htim4.Instance->CCR2 = ch2_pulse_val;
}


//PWM比较模式回调函数
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint8_t pwm1_flag = 1;	//定义翻转标志位
	static uint8_t pwm2_flag = 1;	//定义翻转标志位
	
	if(htim == &htim4)	//如果是定时器4
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)	//如果是通道1
		{
			if(pwm1_flag)
			{
				htim4.Instance->CCR1 += ch1_duty_val;	//写入有效电平所需的CRRx值
			}
			else
			{
				htim4.Instance->CCR1 += ch1_pulse_val - ch1_duty_val;	//写入无效电平所需的CRRx值(总频率值-高电平值=低电平值)
			}
			pwm1_flag = !pwm1_flag;	//取反
		}
		
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)	//如果是通道2
		{
			if(pwm2_flag)
			{
				htim4.Instance->CCR2 += ch2_duty_val;	//写入有效电平所需的CRRx值	
			}
			else
			{
				htim4.Instance->CCR2 += ch2_pulse_val - ch2_duty_val;	//写入无效电平所需的CRRx值			
			}	
			pwm2_flag = !pwm2_flag;	//取反
		}
	}
}




/*====================PWM捕获测试区=======================*/


//捕获中断回调函数
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	
	if(htim==(&htim2))
	{
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
		{
			if(1 == TIM_IC_flag)
			{
				TIM_IC_flag++;
				TIM_IC_2.TIM_ONE = TIM2->CCR2;	//记录第一次上升沿
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_FALLING);	//下降沿捕获
			}
			else if(2 == TIM_IC_flag)
			{
				TIM_IC_flag++;
				TIM_IC_2.TIM_TWO = TIM2->CCR2;	//记录第一次下降沿
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_RISING);	//上升沿捕获			
			}
			else if(3 == TIM_IC_flag)
			{
				TIM_IC_flag++;
				TIM_IC_2.TIM_THREE = TIM2->CCR2;	//记录第二次上升沿
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_FALLING);	//下降沿捕获
			}
			else if(4 == TIM_IC_flag)
			{
				TIM_IC_flag++;
				TIM_IC_2.TIM_FOUR = TIM2->CCR2;	//记录第二次下降沿
				HAL_TIM_IC_Stop_IT(&htim2,TIM_CHANNEL_2);	//停止捕获
				__HAL_TIM_SetCounter(&htim2, 0);	//计数值清0				
			}
		}
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_3)
		{
			LED_Dis1();
		}
	}
	
	if(htim==(&htim3))
	{
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
		{
			if(1 == TIM_IC_flag)
			{
				TIM_IC_flag++;
				TIM_IC_3.TIM_ONE = TIM3->CCR1;	//记录第一次上升沿
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);	//下降沿捕获
			}
			else if(2 == TIM_IC_flag)
			{
				TIM_IC_flag++;
				TIM_IC_3.TIM_TWO = TIM3->CCR1;	//记录第一次下降沿
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);	//上升沿捕获			
			}
			else if(3 == TIM_IC_flag)
			{
				TIM_IC_flag++;
				TIM_IC_3.TIM_THREE = TIM3->CCR1;	//记录第二次上升沿
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);	//下降沿捕获
			}
			else if(4 == TIM_IC_flag)
			{
				TIM_IC_flag++;
				TIM_IC_3.TIM_FOUR = TIM3->CCR1;	//记录第二次下降沿
				HAL_TIM_IC_Stop_IT(&htim3,TIM_CHANNEL_1);	//停止捕获
				__HAL_TIM_SetCounter(&htim3, 0);	//计数值清0				
			}
		}
	}	
}
