#include "my.h"
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "lcd.h"

MYDATA_TypeDef MyData =
{
    .HightLight_Flag = 1,
    .Adc_Value = 0.0,
    .LCD_OVER_FLAG = 0,
    .Key_Down_State = {0},
    .Adc_Over_Flag = 0,
    .H_or_L_State = 1,
    .Fre_5s_Flag = 0,
    .Lock_State = 0,
    .Lcd_Dis_Choose_State = 1,
};

PARAMETER_TypeDef ParameterData =
{
    .PA1_Fre = 4000,
    .Data_P = 0,
    .Data_V = 0,
    .Para_R = 1,
    .Para_K = 1,
    .Recd_N = 0,
    .Recd_MH = 0,
    .Recd_ML = 0,
};

PWMDATA_TypeDef PwmData =
{
	.IC_OVER_FLAG = 0,
    .One_Value = 0,
    .Two_Value = 0,
    .Three_Value = 0,
    .Four_Value = 0,
    .Fre_Value = 0,
    .Duty_Value = 0,
    .IC_RUN_STATE = 0,
};

//全局变量
uint16_t Led_State = 0xFF;	//LED初始状态
uint8_t Key_Up, Key_Down, Key_Value;	//按键相关
uint16_t Key_Time = 0;

//显示数组
char Lcd_dis1_Arr[20] = "\0";
char Lcd_dis2_Arr[20] = "\0";
char Lcd_dis3_Arr[20] = "\0";
char Lcd_dis4_Arr[20] = "\0";
uint8_t R_temp = 1;	//临时变量 用于LCD显示
uint8_t K_temp = 1;	//临时变量
uint32_t ADC_BUFF[10];	//ADC DMA存储数组

void vLcd_Display_function(void)
{
    if(MyData.LCD_OVER_FLAG)
    {
        memset(Lcd_dis1_Arr, ' ', sizeof(Lcd_dis1_Arr));
        memset(Lcd_dis2_Arr, ' ', sizeof(Lcd_dis2_Arr));
        memset(Lcd_dis3_Arr, ' ', sizeof(Lcd_dis3_Arr));
        memset(Lcd_dis4_Arr, ' ', sizeof(Lcd_dis4_Arr));
        if(1 == MyData.Lcd_Dis_Choose_State)
        {
            MyData.HightLight_Flag = 1;	//高亮索引初始化
            snprintf(Lcd_dis1_Arr, sizeof(Lcd_dis1_Arr), "        DATA        ");
            LCD_DisplayStringLine(Line1, (uint8_t *)Lcd_dis1_Arr);

            if(1 == MyData.H_or_L_State)	//低 默认上电
            {
                snprintf(Lcd_dis2_Arr, sizeof(Lcd_dis2_Arr), "     M=L            ");
                LCD_DisplayStringLine(Line3, (uint8_t *)Lcd_dis2_Arr);
            }
            else if(2 == MyData.H_or_L_State)	//高
            {
                snprintf(Lcd_dis2_Arr, sizeof(Lcd_dis2_Arr), "     M=H            ");
                LCD_DisplayStringLine(Line3, (uint8_t *)Lcd_dis2_Arr);
            }

            snprintf(Lcd_dis3_Arr, sizeof(Lcd_dis3_Arr), "     P=%d%%          ", ParameterData.Data_P);
            LCD_DisplayStringLine(Line4, (uint8_t *)Lcd_dis3_Arr);

            snprintf(Lcd_dis4_Arr, sizeof(Lcd_dis4_Arr), "     V=%.1f          ", ParameterData.Data_V);
            LCD_DisplayStringLine(Line5, ( uint8_t *)Lcd_dis4_Arr);

            Led_State = (~0x01)&Led_State;	//LED1亮
        }
        else if(2 == MyData.Lcd_Dis_Choose_State)
        {
            snprintf(Lcd_dis1_Arr, sizeof(Lcd_dis1_Arr), "        PARA        ");
            LCD_DisplayStringLine(Line1, (uint8_t *)Lcd_dis1_Arr);

            snprintf(Lcd_dis2_Arr, sizeof(Lcd_dis2_Arr), "     R=%d           ", R_temp);
            snprintf(Lcd_dis3_Arr, sizeof(Lcd_dis3_Arr), "     K=%d           ", K_temp);

            if(1 == MyData.HightLight_Flag)	//高亮R
            {
                LCD_SetTextColor(White);
                LCD_DisplayStringLine(Line3, (uint8_t *)Lcd_dis2_Arr);
                LCD_SetTextColor(Green);
                LCD_DisplayChar(Line3, 319 - (5 * 16), 'R');

                LCD_SetTextColor(White);
                LCD_DisplayStringLine(Line4, (uint8_t *)Lcd_dis3_Arr);
            }
            else if(2 == MyData.HightLight_Flag)	//高亮K
            {
                LCD_SetTextColor(White);
                LCD_DisplayStringLine(Line4, (uint8_t *)Lcd_dis3_Arr);
                LCD_SetTextColor(Green);
                LCD_DisplayChar(Line4, 319 - (5 * 16), 'K');

                LCD_SetTextColor(White);
                LCD_DisplayStringLine(Line3, (uint8_t *)Lcd_dis2_Arr);
            }
            Led_State = (0x01) | Led_State;	//LED1灭
        }
        else if(3 == MyData.Lcd_Dis_Choose_State)
        {
            //退出参数页面，参数生效
            ParameterData.Para_R = R_temp;
            ParameterData.Para_K = K_temp;

            snprintf(Lcd_dis1_Arr, sizeof(Lcd_dis1_Arr), "        RECD        ");
            LCD_DisplayStringLine(Line1, (uint8_t *)Lcd_dis1_Arr);

            snprintf(Lcd_dis2_Arr, sizeof(Lcd_dis2_Arr), "     N=%d            ", ParameterData.Recd_N);
            LCD_DisplayStringLine(Line3, (uint8_t *)Lcd_dis2_Arr);

            snprintf(Lcd_dis3_Arr, sizeof(Lcd_dis3_Arr), "     MH=%.1f           ", ParameterData.Recd_MH);
            LCD_DisplayStringLine(Line4, (uint8_t *)Lcd_dis3_Arr);

            snprintf(Lcd_dis4_Arr, sizeof(Lcd_dis4_Arr), "     ML=%.1f           ", ParameterData.Recd_ML);
            LCD_DisplayStringLine(Line5, (uint8_t *)Lcd_dis4_Arr);
            Led_State = (0x01) | Led_State;	//LED1灭
        }
        vLed_Control(Led_State);
        MyData.LCD_OVER_FLAG = 0;
    }
}



void vHardware_Init(void)
{
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    vLed_Control2(RESET, 0xFF);
    //vLed_Control(0xFF);
    HAL_TIM_Base_Start_IT(&htim6);
    HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
    HAL_ADC_Start_DMA(&hadc2, (uint32_t *)&ADC_BUFF, 10);
    TIM2->CCR2 = 125;
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
}

//获取ADC值并且计算占空比
void vAdc_Get_function(void)
{
    uint32_t Adc_temp = 0;
    uint8_t count = 0;
    uint16_t Adc_temp2 = 0;
    double Adc_temp3 = 0.0;
    static double Adc_Old_Value = 0;

    HAL_ADC_Stop_DMA(&hadc2);
    for(uint8_t i = 0; i < 10 - 1; i++)
    {
        count = 0;
        for(uint8_t j = 0; j < 10 - 1 - i; j++)
        {
            if(ADC_BUFF[j] > ADC_BUFF[j + 1])
            {
                uint32_t temp = ADC_BUFF[j];
                ADC_BUFF[j] = ADC_BUFF[j + 1];
                ADC_BUFF[j + 1] = temp;
                count++;
            }
        }
        if(!count)
        {
            break;
        }
    }
    for(uint8_t k = 1; k < 10 - 1; k++)
    {
        Adc_temp += ADC_BUFF[k];
    }
    HAL_ADC_Start_DMA(&hadc2, (uint32_t *)&ADC_BUFF, 10);
    MyData.Adc_Value = (double)(Adc_temp / 8) / 4096 * 3.3f;
    Adc_temp2 = MyData.Adc_Value * 10;	//扩大10倍
    Adc_temp3 = (double)Adc_temp2 / 10;
    //改了频率，记得占空比也要改！！
    if(!MyData.Lock_State)	//解锁状态下
    {
        if((Adc_temp3 >= 0) && (Adc_temp3 <= 1))	//10%占空比
        {
            TIM2->CCR2 = (TIM2->ARR * 0.1);
            ParameterData.Data_P = 10;
        }
        else if(Adc_temp3 >= 3)	//85%占空比
        {
            TIM2->CCR2 = (TIM2->ARR * 0.85);
            ParameterData.Data_P = 85;
        }
        else	//其余情况
        {
            Adc_temp3 = (0.375 * Adc_temp3) - 0.275;
            TIM2->CCR2 = ((TIM2->ARR) * Adc_temp3);
            ParameterData.Data_P = Adc_temp3 * 100;
        }
    }
    MyData.Adc_Over_Flag = 1;
}

uint8_t ucKey_Scan(void)
{
    if((!KEY1) || (!KEY2) || (!KEY3) || (!KEY4))
    {
        if(!KEY1)
            return 1;
        if(!KEY2)
            return 2;
        if(!KEY3)
            return 3;
        if(!KEY4)
            return 4;
    }
    return 0;
}

void vKey_function(void)
{
    static uint8_t Key_Old_Value;

    Key_Value = ucKey_Scan();
    Key_Up = ~Key_Value & (Key_Value ^ Key_Old_Value);
    Key_Down = Key_Value & (Key_Value ^ Key_Old_Value);
    Key_Old_Value = Key_Value;

    if(Key_Down)
    {
        Key_Time = 0;
    }
    if(Key_Time < 20)
    {
        switch(Key_Up)
        {
        case 1:
        {
            MyData.Key_Down_State[0] = 1;
            break;
        }
        case 2:
        {
            MyData.Key_Down_State[1] = 1;
            break;
        }
        case 3:
        {
            MyData.Key_Down_State[2] = 1;
            break;
        }
        case 4:
        {
            MyData.Key_Down_State[3] = 1;
            break;
        }
        default:
            break;
        }
    }
    else
    {
        switch(Key_Value)
        {
        case 1:
        {
            MyData.Key_Down_State[4] = 1;
            break;
        }
        case 2:
        {
            MyData.Key_Down_State[5] = 1;
            break;
        }
        case 3:
        {
            MyData.Key_Down_State[6] = 1;
            break;
        }
        case 4:
        {
            MyData.Key_Down_State[7] = 1;
            break;
        }
        default:
            break;
        }
    }
}

void vRun_Flag_function(void)
{
    static uint8_t choose_Flag = 1;	//R、K切换

    if(MyData.Key_Down_State[0])	//界面切换
    {
        MyData.Key_Down_State[0] = 0;
        MyData.Lcd_Dis_Choose_State++;
        if(MyData.Lcd_Dis_Choose_State > 3)
        {
            MyData.Lcd_Dis_Choose_State = 1;
        }
        LCD_Clear(Black);	//清屏
    }
    if(MyData.Key_Down_State[1])
    {
        MyData.Key_Down_State[1] = 0;
        if((1 == MyData.Lcd_Dis_Choose_State) && (0 == MyData.Fre_5s_Flag))	//数据页面下
        {
            MyData.Fre_5s_Flag = 1;
        }
        else if(2 == MyData.Lcd_Dis_Choose_State)	//参数页面下
        {
            choose_Flag = !choose_Flag;
            if(!choose_Flag)	//选择K
            {
                MyData.HightLight_Flag = 2;

            }
            else	//选择R
            {
                MyData.HightLight_Flag = 1;
            }
        }
        else
        {
            ;
        }
    }
    if(MyData.Key_Down_State[2])
    {
        MyData.Key_Down_State[2] = 0;
        if(2 == MyData.Lcd_Dis_Choose_State)
        {
            if(1 == MyData.HightLight_Flag)
            {
                R_temp++;
                if(R_temp > 10)
                {
                    R_temp = 10;
                }
            }
            if(2 == MyData.HightLight_Flag)
            {
                K_temp++;
                if(K_temp > 10)
                {
                    K_temp = 10;
                }
            }
        }
    }
    if(MyData.Key_Down_State[3])
    {
        MyData.Key_Down_State[3] = 0;
        if(2 == MyData.Lcd_Dis_Choose_State)
        {
            if(1 == MyData.HightLight_Flag)
            {
                R_temp--;
                if(R_temp < 1)
                {
                    R_temp = 1;
                }
            }
            if(2 == MyData.HightLight_Flag)
            {
                K_temp--;
                if(K_temp < 1)
                {
                    K_temp = 1;
                }
            }
        }
        else if(1 == MyData.Lcd_Dis_Choose_State)
        {
            if(MyData.Lock_State)	//锁状态
            {
                MyData.Lock_State = 0;	//解锁
                Led_State = (0x04) | Led_State;	//LED3灭
                vLed_Control(Led_State);
            }
        }
    }
    if(MyData.Key_Down_State[7])	//长按
    {
        MyData.Key_Down_State[7] = 0;
        MyData.Lock_State = 1;	//锁
        Led_State = (~0x04)&Led_State;	//LED3亮
        vLed_Control(Led_State);
    }
    if(MyData.Adc_Over_Flag)
    {
        MyData.Adc_Over_Flag = 0;
#if TEST
        char arr[20];
        snprintf(arr, sizeof(arr), "ADC:%.1f\r\n", MyData.Adc_Value);
        HAL_UART_Transmit(&huart1, (uint8_t *)arr, strlen(arr), 500);
#endif
    }
    if(4 == PwmData.IC_RUN_STATE)	//计算实时速度
    {
        PwmData.Fre_Value = 1000000 / (PwmData.Four_Value - PwmData.Two_Value);
        PwmData.Duty_Value = (float)(PwmData.Four_Value - PwmData.Three_Value) / (float)(PwmData.Four_Value - PwmData.Two_Value) * 100;
        ParameterData.Data_V = (float)(PwmData.Fre_Value * 2 * 3.14 * ParameterData.Para_R) / (float)(100 * ParameterData.Para_K);
#if TEST
        char arr[20];
        snprintf(arr, sizeof(arr), "fre:%d--%.1f\r\n", PwmData.Fre_Value, PwmData.Duty_Value);
        HAL_UART_Transmit(&huart1, (uint8_t *)arr, strlen(arr), 500);
#endif
        PwmData.IC_RUN_STATE = 0;
        HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
		PwmData.IC_OVER_FLAG = 1;	//计算完成标志位
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim == &htim3)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
        {
            switch(PwmData.IC_RUN_STATE)
            {
            case 0:
            {
                PwmData.One_Value = TIM3->CCR2;
                __HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);	//下降沿
                PwmData.IC_RUN_STATE = 1;
                break;
            }
            case 1:
            {
                PwmData.Two_Value = TIM3->CCR2;
                __HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);	//上升沿
                PwmData.IC_RUN_STATE = 2;
                break;
            }
            case 2:
            {
                PwmData.Three_Value = TIM3->CCR2;
                __HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);	//下降沿
                PwmData.IC_RUN_STATE = 3;
                break;
            }
            case 3:
            {
                PwmData.Four_Value = TIM3->CCR2;
                __HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);	//上升沿
                HAL_TIM_IC_Stop_IT(&htim3, TIM_CHANNEL_2);
                __HAL_TIM_SetCounter(&htim3, 0);
                PwmData.IC_RUN_STATE = 4;
                break;
            }
            }
        }
    }
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint8_t Tim_10ms_count = 0;
    static uint8_t Tim_100ms_count = 0;
    static uint8_t Tim_150ms_count = 0;
    static uint16_t Tim_5s_count = 0;
	static uint16_t Tim_2s_count = 0;
	
    if(htim == &htim6)
    {
        Tim_10ms_count++;
        Tim_100ms_count++;
        Tim_150ms_count++;

        if(10 == Tim_10ms_count)
        {
            Tim_10ms_count = 0;
            vKey_function();
        }
        if(100 == Tim_100ms_count)
        {
            Tim_100ms_count = 0;
            MyData.LCD_OVER_FLAG = 1;
            Key_Time++;
        }
        if(150 == Tim_150ms_count)
        {
            Tim_150ms_count = 0;
            vAdc_Get_function();
        }
        if(MyData.Fre_5s_Flag)	//5s计时
        {
            Tim_5s_count++;
            if(5000 == Tim_5s_count)
            {
                Tim_5s_count = 0;
                MyData.Fre_5s_Flag = 0;
                //转换完成
                if(1 == MyData.H_or_L_State)
				{
					MyData.H_or_L_State = 2;
					
				}   
                else if(2 == MyData.H_or_L_State)
				{
					MyData.H_or_L_State = 1;
				}                   
                ParameterData.Recd_N++;	//切换次数加1

            }
            if(0 == (Tim_5s_count % 100))	//50次
            {
                Led_State ^= 2;
                vLed_Control(Led_State);
                if(1 == MyData.H_or_L_State)	//当前为低频---》高
                {
                    ParameterData.PA1_Fre += 80;
                    if(ParameterData.PA1_Fre >= 8000)
                    {
                        ParameterData.PA1_Fre = 8000;
                    }
                    //改了频率，记得占空比也要改！！
                    TIM2->ARR = (1000000.0f / ParameterData.PA1_Fre) - 1;
                    TIM2->CCR2 = (ParameterData.Data_P / 100.0f) * (TIM2->ARR);
                }
                else if(2 == MyData.H_or_L_State)	//当前为高频---》低
                {
                    ParameterData.PA1_Fre -= 80;
                    TIM2->ARR = ParameterData.PA1_Fre - 1;
                    if(ParameterData.PA1_Fre <= 4000)
                    {
                        ParameterData.PA1_Fre = 4000;
                    }
                    //改了频率，记得占空比也要改！！
                    TIM2->ARR = (1000000.0f / ParameterData.PA1_Fre) - 1;
                    TIM2->CCR2 = (ParameterData.Data_P / 100.0f) * (TIM2->ARR);
                }
            }
        }
		if((1 == PwmData.IC_OVER_FLAG) && (0 == MyData.Fre_5s_Flag))	//最大值(高低频切换期间的不算入)
		{
                if(1 == MyData.H_or_L_State)
				{
					if(ParameterData.Recd_ML < ParameterData.Data_V)	//当前速度最大值小于实时速度
					{
						Tim_2s_count++;
						if(Tim_2s_count >= 2000)
						{
							Tim_2s_count = 0;
							ParameterData.Recd_ML = ParameterData.Data_V;
							PwmData.IC_OVER_FLAG = 0;
						}
					}
					else
					{
						Tim_2s_count = 0;
						PwmData.IC_OVER_FLAG = 0;
					}
				}   
                else if(2 == MyData.H_or_L_State)
				{
					if(ParameterData.Recd_MH < ParameterData.Data_V)	//当前速度最大值小于实时速度
					{
						Tim_2s_count++;
						if(Tim_2s_count >= 2000)
						{
							Tim_2s_count = 0;
							ParameterData.Recd_MH = ParameterData.Data_V;
							PwmData.IC_OVER_FLAG = 0;
						}
					}
					else
					{
						Tim_2s_count = 0;
						PwmData.IC_OVER_FLAG = 0;
					}
				} 			
		}
    }
}

void vLed_Control(uint8_t led_num)
{
    uint16_t a = GPIOC->ODR;
    GPIOC->ODR = (uint16_t)led_num << 8;
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
    GPIOC->ODR = a;
}

void vLed_Control2(uint8_t swch, uint8_t num)
{
    if(SET == swch)
    {
        HAL_GPIO_WritePin(GPIOC, (uint16_t)num << 8, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOC, (uint16_t)num << 8, GPIO_PIN_SET);
    }
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}



