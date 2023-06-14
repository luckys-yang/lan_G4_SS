#include "my.h"

TASK_TIMESTypeDef TASK_TIMES;
uint16_t LCD_state = 0xFFFF;	//行变化的标记
RTC_TimeTypeDef rtc_time;	//时间结构体
RTC_DateTypeDef rtc_date;	//日期结构体

uint8_t Dang[] = {0x00,0x00,0x00,0x00,0x10,0x60,0xC0,0xC0,0x80,
				  0x00,0xF8,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,
				  0x00,0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x08,
				  0x18,0x18,0x18,0x18,0x18,0x98,0x58,0xFF,0x00,
	              0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0xFF,
				  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x06,
	              0x03,0x01,0x00,0x00,0x3F,0x18,0x18,0x18,0x18,
	              0x1F,0x18,0x18,0x18,0x18,0x1F,0x18,0x18,0x00};/*"当",0*/

uint8_t Qian[] = {0x00,0x80,0x00,0x00,0x00,0xFE,0x00,0x00,0xF0,
				  0x10,0x10,0x10,0xF0,0x10,0x10,0x10,0xF0,0x10,
				  0x10,0x10,0x10,0x90,0x10,0x00,0x00,0x00,0x81,
				  0x83,0x42,0xFF,0x00,0x00,0x47,0xC4,0x44,0x44,
	              0x47,0x44,0x44,0x44,0x47,0x44,0xC4,0x04,0x84,
				  0x07,0x02,0x00,0x00,0x00,0x01,0x00,0x10,0x3F,
	              0x00,0x04,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,
	              0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x07,0x02,0x00};/*"前",0*/			  
		
uint8_t Ping[] = {0x00,0x00,0x00,0xF8,0x00,0x00,0x20,0xC0,0x80,
				  0x80,0x00,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,
				  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				  0xFF,0x18,0x18,0x18,0x18,0x18,0x98,0x58,0xFF,
				  0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
				  0x18,0x08,0x00,0x00,0x00,0x08,0x1F,0x00,0x04,
				  0x06,0x02,0x01,0x00,0x10,0x3F,0x00,0x00,0x00,
				  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};/*"平",0*/

uint8_t Tai[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x40,0x20,
				 0x18,0xF8,0x10,0x00,0xC0,0x40,0x40,0x40,0x40,
				 0x40,0x40,0xC0,0x40,0x00,0x00,0x00,0x08,0x1C,
				 0x06,0x02,0x01,0x00,0x00,0x00,0xF0,0x0F,0x00,
				 0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
				 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				 0x01,0x06,0x0C,0x1F,0x18,0x10,0x00,0x07,0x02,
				 0x02,0x02,0x02,0x02,0x02,0x03,0x02,0x02,0x00};/*"台",0*/	

uint8_t LCD_Line_BUFF[10][20] =
{
    "                    ",	//0
    "                    ",	//1
    "                    ",	//2
    "                    ",	//3
    "                    ",	//4
    "         1          ",	//5
    "                    ",	//6
    "                    ",	//7
    "      00:00:00      ",	//8
    "                    "	//9
};

//LCD检测
void LCD_function(void)
{
    uint8_t i;

    for(i = 0; i < 10; i++)
    {
		if(i != 2)	//防止操作到中文显示那行
		{
			if(LCD_state & (0x01 << i))	//如果该行发生变化(上电默认全部都发生变化)
			{
				LCD_state &= (~(1 << i));	//清除状态
				LCD_DisplayStringLine(i * 24, LCD_Line_BUFF[i]);	//重新显示该行
			}			
		}
    }
}

//汉字显示函数
/*
参数1：x坐标(0~240)
参数2：y坐标(319~0)
参数3：背景颜色
参数4：字体颜色
参数5：需要显示的字模数组
*/
void LCD_DrawMonoPict1(uint16_t Xpos, uint16_t Ypos, uint32_t BackColor, uint32_t TextColor, uint8_t *Pict)
{
    uint8_t index = 0, i = 0, Xpos_Temp;
    Xpos_Temp = Xpos;	//保存初始坐标
    Ypos = 319 - (24 * Ypos);	//右移多少个汉字再显示24*24一行最多显示13个
    LCD_SetCursor(Xpos, Ypos); 	//写入坐标
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

    for(index = 0; index < 72; index++)
    {
        //需要改变Y两次(换行)，0-24  24-48  48-72
        if(24 == index || 48 == index)
        {
            Xpos = Xpos_Temp;	//x坐标恢复初始
            Ypos = Ypos - 8;	//Y右移8个像素
        }
        for(i = 0; i < 8; i++)
        {
            if((Pict[index] & (1 << i)) == 0x00)	//如果对应位置数据是0x00则显示背景颜色
            {
                LCD_WriteRAM(BackColor);
            }
            else	//不为0显示字体
            {
                LCD_WriteRAM(TextColor);
            }
        }
        LCD_SetCursor(Xpos++, Ypos); 	//写入坐标
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    }
}

//初始化
void TASK_Init(void)
{
    LCD_Init();	//LCD初始化
    LCD_SetBackColor(Black);	//背景
    LCD_SetTextColor(White);	//字体颜色
	LED_DIS(0xFF,RESET);	//LED默认灭
    HAL_TIM_Base_Start_IT(&htim6);	//定时器6开启中断
    HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);	//获取时间
    HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);	//获取日期
    Data.Platform = 1;	//默认平台为1
    Data.SJJ_Open_Over = 1;	//默认升降机开门结束状态
    Data.Platform_UPorDOWN = 1;	//默认上
    Data.KEY_InFore_Flag = 1;	//按键默认有效
    LED_DIS(0xF0, SET);	//流水灯默认全部亮表示可按
	
	LCD_DisplayStringLine(Line2,(uint8_t*)"                    ");	//先空格覆盖再显示
	LCD_DrawMonoPict1(Line2,4,Black,White,Dang);	//"当"
	LCD_DrawMonoPict1(Line2,5,Black,White,Qian);	//"前"
	LCD_DrawMonoPict1(Line2,6,Black,White,Ping);	//"平"
	LCD_DrawMonoPict1(Line2,7,Black,White,Tai);		//"台"
}


//RTC显示(默认MX设置时间：12:50:55)
void RTC_function(void)
{
    static uint8_t Seconds_last;
    if(!TASK_TIMES.RTC_TIME)	//200ms读取一次RTC时间
    {
        TASK_TIMES.RTC_TIME = RTC_TIMEST;
        HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);	//获取时间
        HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);	//获取日期

        if(Seconds_last != rtc_time.Seconds)
        {
            Seconds_last = rtc_time.Seconds;
            sprintf((char *)LCD_Line_BUFF[LCD_RTC], "      %02d:%02d:%02d      ", rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds);
            LCD_state |= (0x01 << LCD_RTC);
        }
    }
}

//按键
uint8_t KEY_pro(void)
{
    uint8_t value;
    if((!KEY1) || (!KEY2) || (!KEY3) || (!KEY4))
    {
        if(!KEY1)
            value = 1;
        if(!KEY2)
            value = 2;
        if(!KEY3)
            value = 3;
        if(!KEY4)
            value = 4;

        return value;
    }
    return 0;
}


uint8_t KEY_UP;	//按下松开瞬间有效
uint8_t KEY_DOWN;	//按下瞬间有效
uint8_t KEY_VALUE;	//按键键值
uint16_t KEY_COUNT = 0;	//短按计时


//执行对应按键标志位状态
void KEY_SCAN(void)
{
    static uint8_t KEY_Old;

    if(!TASK_TIMES.KEY_time)
    {
        TASK_TIMES.KEY_time = KEY_TIMEST;
        KEY_VALUE = KEY_pro();
        KEY_UP = ~(KEY_VALUE) & (KEY_Old ^ KEY_VALUE);	//抬起瞬间有有效
        KEY_DOWN = (KEY_VALUE) & (KEY_Old ^ KEY_VALUE);	//按下瞬间有效
        KEY_Old = KEY_VALUE;
        if(KEY_DOWN)
        {
            KEY_COUNT = 0;
        }

        if(KEY_COUNT < 10)	//小于1000ms短按且是等待启动
        {
            if(0 == Data.KEY_InFore_Flag)
            {
                return;	//退出
            }
            switch(KEY_UP)
            {
            case 1:
            {
                if(1 == Data.Platform)	//当前平台按下按键没效果
                {
                    break;
                }
                Data.LED1_State = 1;	//去平台1
                Data.KEY_Timer_Start = 1;
                Data.KEY_Timer_Count = 0;	//清0等待时间
                break;
            }

            case 2:
            {
                if(2 == Data.Platform)	//当前平台按下按键没效果
                {
                    break;
                }
                Data.LED2_State = 1;	//去平台2
                Data.KEY_Timer_Start = 1;
                Data.KEY_Timer_Count = 0;	//清0等待时间
                break;
            }

            case 3:
            {
                if(3 == Data.Platform)	//当前平台按下按键没效果
                {
                    break;
                }
                Data.LED3_State = 1;	//去平台3
                Data.KEY_Timer_Start = 1;
                Data.KEY_Timer_Count = 0;	//清0等待时间
                break;
            }

            case 4:
            {
                if(4 == Data.Platform)	//当前平台按下按键没效果
                {
                    break;
                }
                Data.LED4_State = 1;	//去平台4
                Data.KEY_Timer_Start = 1;
                Data.KEY_Timer_Count = 0;	//清0等待时间
                break;
            }

            default:
                break;
            }
        }
    }
}



DATA_TypeDef Data;	//数据结构体

//判断标志位完成功能
void Flag_Run(void)
{

    if(Data.LED1_State)
    {
        LED_DIS(0x01, SET);	//点亮LED1
    }
    if(Data.LED2_State)
    {
        LED_DIS(0x02, SET);	//点亮LED1
    }
    if(Data.LED3_State)
    {
        LED_DIS(0x04, SET);	//点亮LED1
    }
    if(Data.LED4_State)
    {
        LED_DIS(0x08, SET);	//点亮LED1
    }
    if(Data.KEY_Timer_Start)	//按下等待开始计时
    {
        if(KEY_OVER_TIME <= Data.KEY_Timer_Count)	//1s
        {
            Data.KEY_Timer_Start = 0;	//清除标志位
            Data.KEY_Timer_Over = 1;
        }
    }
    if(Data.KEY_Timer_Over)	//按下等待完成
    {
        Data.KEY_Timer_Over = 0;
        Data.SJJ_Close_Start = 1;
        LED_DIS(0xF0, RESET);	//灯全灭表示运行
        if(0 == Check_UP())
            Data.Platform_UPorDOWN = 0;
        else
            Data.Platform_UPorDOWN = 1;
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);	//PA5低电平
        Data.PWM_Out_OpenClose_Flag = 1;	//关门pwm
        Data.KEY_InFore_Flag = 0;	//按键无效
        Data.SJJ_OpenClose_Timer_Count = 0;	//清0
    }
    if(Data.SJJ_Close_Start)	//开始升降机关门
    {
        if(SJJ_OC_OVER_TIME <= Data.SJJ_OpenClose_Timer_Count)	//4s
        {
            Data.SJJ_Close_Start = 0;	//清除标志位
            Data.PWM_Out_OpenClose_Flag = 3;	//开门电机pwm输出无效
            Data.SJJ_Close_Over = 1;
        }

    }

    if(Data.SJJ_Wait_Start)	//开始停留计时
    {
        Data.KEY_InFore_Flag = 1;	//按键有效
        if(SJJ_WAIT_OVER_TIME <= Data.SJJ_Wait_Timer_Count)	//2s
        {
            Data.SJJ_Wait_Start = 0;
            Data.SJJ_Wait_Over = 1;
        }
    }
    if(Data.SJJ_Wait_Over)	//停留完成
    {
        Data.SJJ_Wait_Over = 0;
        Data.SJJ_Close_Over = 1;
        Data.KEY_InFore_Flag = 0;	//按键无效
    }
    if(Data.Platform_NumberBlink_Flag)	//闪烁字完成
    {
        Data.Platform_NumberBlink_Flag = 0;
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);	//PA5高电平
        Data.PWM_Out_UpDown_Flag = 3;	//上下电机输出无效
        Data.SJJ_OpenClose_Timer_Count = 0;	//清0
        Data.PWM_Out_OpenClose_Flag = 2;	//开门电机pwm输出
        Data.SJJ_Open_Start = 1;	//开始开门
    }
    if(Data.SJJ_Open_Start)	//开始升降机开门
    {
        if(SJJ_OC_OVER_TIME <= Data.SJJ_OpenClose_Timer_Count)	//4s
        {
            Data.SJJ_Open_Start = 0;	//清除标志位
            Data.PWM_Out_OpenClose_Flag = 3;	//开门电机pwm输出无效
            Data.SJJ_Open_Over = 1;
        }
    }
    if(Data.Platform_UPorDOWN)	//上
    {
        SJJ_UP();
    }
    else if(0 == Data.Platform_UPorDOWN)	//下
    {
        SJJ_DOWN();
    }
}

//升降机下降到目标
void SJJ_DOWN(void)
{
    if(Data.SJJ_Close_Over)	//升降机关门完成
    {
        Data.SJJ_Close_Over = 0;

        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);	//PA4低电平下降
        Data.PWM_Out_UpDown_Flag = 2;
        Data.SJJ_UpDown_Timer_Count = 0;	//清0
        Data.SJJ_DOWN_Start = 1;	//开始下降标志位
    }
    if(Data.SJJ_DOWN_Start)	//开始下降
    {
        if(SJJ_UD_OVER_TIME <= Data.SJJ_UpDown_Timer_Count)	//6s
        {
            Data.SJJ_DOWN_Start = 0;
            LED_DIS(0xF0, RESET);
            Data.SJJ_DOWN_Over = 1;
        }
    }
    if(Data.SJJ_DOWN_Over)	//下降完成
    {
        Data.Platform--;	//平台--
        if(Check() == 0 )	//未到达
        {
            Data.SJJ_UpDown_Timer_Count = 0;	//清0
            Data.SJJ_DOWN_Start = 1;	//继续下降
        }
        Data.SJJ_DOWN_Over = 0;
    }
    if(Data.SJJ_Open_Over)	//开门完成判断当前平台上面有没有目标平台再判断当前下面(因为题目要求先上后下)
    {

        if(Check_UP())	//有就停留2s再上
        {
            Data.SJJ_Wait_Timer_Count = 0;
            Data.SJJ_Wait_Start = 1;
            Data.Platform_UPorDOWN = 1;	//往上
        }
        else	//没有就判断下面有没有
        {

            if(Check_DOWN())	//如果往下有目标则开始下降
            {
                Data.SJJ_Wait_Timer_Count = 0;
                Data.SJJ_Wait_Start = 1;
                Data.Platform_UPorDOWN = 0;	//往下
            }
            else	//没有
            {

                LED_DIS(0xF0, SET);
                Data.KEY_InFore_Flag = 1;	//按键有效
            }
        }

        Data.SJJ_Open_Over = 0;
    }
}

//升降机上升到目标
void SJJ_UP(void)
{

    if(Data.SJJ_Close_Over)	//升降机关门完成
    {
        Data.SJJ_Close_Over = 0;

        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);	//PA4高电平上升
        Data.PWM_Out_UpDown_Flag = 1;
        Data.SJJ_UpDown_Timer_Count = 0;	//清0
        Data.SJJ_UP_Start = 1;
    }
    if(Data.SJJ_UP_Start)	//开始上升
    {
        if(SJJ_UD_OVER_TIME <= Data.SJJ_UpDown_Timer_Count)	//6s
        {
            Data.SJJ_UP_Start = 0;
            LED_DIS(0xF0, RESET);
            Data.SJJ_UP_Over = 1;
        }
    }
    if(Data.SJJ_UP_Over)	//上升完成
    {
        Data.Platform++;	//平台++
        if(Check() == 0 )	//未到达
        {
            Data.SJJ_UpDown_Timer_Count = 0;	//清0
            Data.SJJ_UP_Start = 1;	//继续上升
        }
        Data.SJJ_UP_Over = 0;
    }
    if(Data.SJJ_Open_Over)	//开门完成判断当前平台上面还有没有，有则继续上没就判断下面
    {

        if(Check_UP())	//有就停留2s再上
        {
            Data.SJJ_Wait_Timer_Count = 0;
            Data.SJJ_Wait_Start = 1;
        }
        else	//没有就判断下面有没有
        {

            if(Check_DOWN())	//如果往下有目标则开始下降
            {
                Data.SJJ_Wait_Timer_Count = 0;
                Data.SJJ_Wait_Start = 1;
                Data.Platform_UPorDOWN = 0;	//往下
            }
            else	//没有则恢复可按状态
            {

                LED_DIS(0xF0, SET);
                Data.KEY_InFore_Flag = 1;	//按键有效
            }
        }

        Data.SJJ_Open_Over = 0;
    }
}

//检测当前平台上面有没有目标平台
uint8_t Check_UP(void)
{
    uint8_t temp = 0;
    for(uint8_t i = Data.Platform + 1; i <= 4; i++)
    {
        if(2 == i)
        {
            if(Data.LED2_State)
            {
                temp++;
            }
        }
        if(3 == i)
        {
            if(Data.LED3_State)
            {
                temp++;
            }
        }
        if(4 == i)
        {
            if(Data.LED4_State)
            {
                temp++;
            }
        }
    }

    return temp;
}

//检测当前平台下面有没有目标平台
uint8_t Check_DOWN(void)
{
    uint8_t temp;
    for(uint8_t j = Data.Platform - 1; j > 0; j--)
    {
        if(1 == j)
        {
            if(Data.LED1_State)
            {
                temp++;
            }
        }
        if(2 == j)
        {
            if(Data.LED2_State)
            {
                temp++;
            }
        }
        if(3 == j)
        {
            if(Data.LED3_State)
            {
                temp++;
            }
        }
        if(4 == j)
        {
            if(Data.LED4_State)
            {
                temp++;
            }
        }

    }

    return temp;
}

//检测是否到达目标平台  返回0表示未到达 >0表示到达
uint8_t Check(void)
{
    uint8_t i = 0;
    switch(Data.Platform)
    {
    case 1:
    {
        if(Data.LED1_State)
        {
            Data.LED1_State = 0;
            LED_DIS(0x01, RESET);	//熄灭LED
            Data.Platform_Over = 1;	//到达标志位置1
            Data.SJJ_DOWN_Start = 0;
            Data.SJJ_UP_Start = 0;
            i++;
        }
        break;
    }
    case 2:
    {
        if(Data.LED2_State)
        {
            Data.LED2_State = 0;
            LED_DIS(0x02, RESET);	//熄灭LED
            Data.Platform_Over = 1;	//到达标志位置1
            Data.SJJ_DOWN_Start = 0;
            Data.SJJ_UP_Start = 0;
            i++;
        }
        break;
    }
    case 3:
    {
        if(Data.LED3_State)
        {
            Data.LED3_State = 0;
            LED_DIS(0x04, RESET);	//熄灭LED
            Data.Platform_Over = 1;
            Data.SJJ_DOWN_Start = 0;
            Data.SJJ_UP_Start = 0;
            i++;
        }
        break;
    }
    case 4:
    {
        if(Data.LED4_State)
        {
            Data.LED4_State = 0;
            LED_DIS(0x08, RESET);	//熄灭LED
            Data.Platform_Over = 1;
            Data.SJJ_DOWN_Start = 0;
            Data.SJJ_UP_Start = 0;
            i++;
        }
        break;
    }
    default:
        break;
    }
    return i;
}




//LED 点亮/熄灭任意
void LED_DIS(uint8_t num, uint8_t swch)
{
    if(SET == swch)
    {
        HAL_GPIO_WritePin(GPIOC, (uint16_t)num << 8, GPIO_PIN_RESET);	//点亮
    }
    else
    {
        HAL_GPIO_WritePin(GPIOC, (uint16_t)num << 8, GPIO_PIN_SET);	//灭
    }
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);	//拉高
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);	//拉低
}

typedef struct
{
    uint32_t pulse_val;	//频率所需值
    uint32_t duty_val;	//有效电平所需值
} TIM_OCTypeDef;	//输出比较

TIM_OCTypeDef TIM3_CH1;	//通道1(PA6)
TIM_OCTypeDef TIM3_CH2;	//通道2(PA7)

//设置PA6频率占空比
void PWM_SET_PA6(uint32_t freq, uint16_t duty)
{
    TIM3_CH1.pulse_val = 1000000 / freq;	//计算频率所需值
    TIM3_CH1.duty_val = TIM3_CH1.pulse_val * duty / 100;	//计算有效电平所需值
    TIM3->CCR1 = TIM3_CH1.pulse_val;
}

//设置PA7频率占空比
void PWM_SET_PA7(uint32_t freq, uint16_t duty)
{
    TIM3_CH2.pulse_val = 1000000 / freq;	//计算频率所需值
    TIM3_CH2.duty_val = TIM3_CH2.pulse_val * duty / 100;	//计算有效电平所需值
    TIM3->CCR2 = TIM3_CH2.pulse_val;
}


//输出比较回调函数
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint8_t PWM_CH1_Flag = 1;
    static uint8_t PWM_CH2_Flag = 1;

    if(htim == &htim3)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)	//通道1
        {
            if(PWM_CH1_Flag)
            {
                TIM3->CCR1 += TIM3_CH1.duty_val;	//高电平持续时间
            }
            else
            {
                TIM3->CCR1 += TIM3_CH1.pulse_val - TIM3_CH1.duty_val;	//低电平持续时间
            }
            PWM_CH1_Flag = !PWM_CH1_Flag;	//取反
        }

        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)	//通道2
        {
            if(PWM_CH2_Flag)
            {
                TIM3->CCR2 += TIM3_CH2.duty_val;	//高电平持续时间
            }
            else
            {
                TIM3->CCR2 += TIM3_CH2.pulse_val - TIM3_CH2.duty_val;	//低电平持续时间
            }
            PWM_CH2_Flag = !PWM_CH2_Flag;	//取反
        }
    }

}

uint32_t sys_time = 0;	//系统时间

//定时器6回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint16_t count_1s = 0;
    static uint16_t count_200ms = 0;
    static uint8_t LED_Index = 1;

    if(htim == &htim6)
    {
        sys_time++;
        Data.KEY_Timer_Count++;	//按键按下等待时间
        Data.SJJ_OpenClose_Timer_Count++;	//开关门时间
        Data.SJJ_UpDown_Timer_Count++;	//上下时间
        Data.SJJ_Wait_Timer_Count++;	//停留时间

        if(TASK_TIMES.KEY_time)
        {
            TASK_TIMES.KEY_time--;
        }
        if(TASK_TIMES.RTC_TIME)
        {
            TASK_TIMES.RTC_TIME--;
        }
        if(0 == (sys_time % 100))	//相当于100ms++一次
        {
            KEY_COUNT++;	//按键短按计数
        }
        if(Data.Platform_Over)	//平台数字闪烁
        {
            count_1s++;
            if(count_1s == 10)
            {
                sprintf((char *)LCD_Line_BUFF[LCD_NUMBER], "                    ");
                LCD_state |= (0x01 << LCD_NUMBER);
            }
            if(count_1s == 400)
            {
                sprintf((char *)LCD_Line_BUFF[LCD_NUMBER], "         %d          ", Data.Platform);
                LCD_state |= (0x01 << LCD_NUMBER);
            }
            if(count_1s == 600)
            {
                sprintf((char *)LCD_Line_BUFF[LCD_NUMBER], "                     ");
                LCD_state |= (0x01 << LCD_NUMBER);
            }
            if(count_1s == 1000)
            {
                count_1s = 0;
                Data.Platform_Over = 0;
                Data.Platform_NumberBlink_Flag = 1;
                sprintf((char *)LCD_Line_BUFF[LCD_NUMBER], "         %d          ", Data.Platform);
                LCD_state |= (0x01 << LCD_NUMBER);
            }

        }
		//上下
		if(1 == Data.PWM_Out_UpDown_Flag)	
		{
			Data.PWM_Out_UpDown_Flag = 0;
			PWM_SET_PA6(1000, 80);	//PA6上升频率1000占空比80   
			HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);	//通道1			
		}
		else if(2 == Data.PWM_Out_UpDown_Flag)
		{
			Data.PWM_Out_UpDown_Flag = 0;
			PWM_SET_PA6(1000, 60);	//PA6下降频率1000占空比60   
			HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);	//通道1			
		}
		else if(3 == Data.PWM_Out_UpDown_Flag)
		{
			Data.PWM_Out_UpDown_Flag = 0;
			PWM_SET_PA6(1000, 0);	//PA6频率1000占空比0 无效状态   
			HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);	//通道1				
		}
		else
		{
			;
		}
		
		//开关
	    if(1 == Data.PWM_Out_OpenClose_Flag)	
		{
			Data.PWM_Out_OpenClose_Flag = 0;
			PWM_SET_PA7(2000, 50);	//PA7关门频率 2000占空比50
			HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_2);	//通道1			
		}
		else if(2 == Data.PWM_Out_OpenClose_Flag)
		{
			Data.PWM_Out_OpenClose_Flag = 0;
			PWM_SET_PA7(2000, 60);	//PA7开门频率2000占空比60   
			HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_2);	//通道1			
		}
		else if(3 == Data.PWM_Out_OpenClose_Flag)
		{
			Data.PWM_Out_OpenClose_Flag = 0;
			PWM_SET_PA7(2000, 0);	//PA7频率2000占空比0 无效状态   
			HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_2);	//通道1				
		}
		else{
			;
		}
		

		if(Data.SJJ_UP_Start)	//上升时的流水方向
        {
            count_200ms++;
            if(200 == count_200ms)
            {
                count_200ms = 0;
                switch(LED_Index)
                {
                case 1:
                    LED_DIS(0x10, SET);LED_Index = 2;break;
                case 2:
                    LED_DIS(0x20, SET);LED_Index = 3;break;
                case 3:
                    LED_DIS(0x40, SET);LED_Index = 4;break;
                case 4:
                    LED_DIS(0x80, SET);LED_Index = 5;break;
                case 5:
                    LED_DIS(0x10, RESET);LED_Index = 6;break;
                case 6:
                    LED_DIS(0x20, RESET);LED_Index = 7;break;
                case 7:
                    LED_DIS(0x40, RESET);LED_Index = 8;break;
                case 8:
                    LED_DIS(0x80, RESET);LED_Index = 1;break;
                }
            }
        }
        else if(Data.SJJ_DOWN_Start)	//下降时的流水方向
        {
            count_200ms++;
            if(200 <= count_200ms)
            {
                count_200ms = 0;
                switch(LED_Index)
                {
                case 1:
                    LED_DIS(0x80, SET);LED_Index = 2;break;
                case 2:
                    LED_DIS(0x40, SET);LED_Index = 3;break;
                case 3:
                    LED_DIS(0x20, SET);LED_Index = 4;break;
                case 4:
                    LED_DIS(0x10, SET);LED_Index = 5;break;
                case 5:
                    LED_DIS(0x80, RESET);LED_Index = 6;break;
                case 6:
                    LED_DIS(0x40, RESET);LED_Index = 7;break;
                case 7:
                    LED_DIS(0x20, RESET);LED_Index = 8;break;
                case 8:
                    LED_DIS(0x10, RESET);LED_Index = 1;break;
                }
            }
        }
        else	//索引恢复
        {
            LED_Index = 1;
        }		
    }
}
