#include "my.h"
uint8_t LED_state = 0XFF;	//LED状态
uint8_t KEY_UP, KEY_DOWN, KEY_VALUE;
uint16_t KEY_COUNT;
DATA_TypeDef Data = {0};
PARAMETER_TypeDef parameter = {0.0, 0.1, 0, 0, 0, 0};
uint16_t LCD_State = 0xFFFF;
uint8_t LCD_Line_BUFF[10][20] = {""};
uint32_t ADC_BUFF[100];
RTC_TimeTypeDef my_time = {0};
RTC_DateTypeDef my_date = {0};
TASK_TINESTypeDef TASK_TIMES;
uint32_t sys_time = 0;

//初始化时间片
void TASK_TIMES_init(void)
{
    TASK_TIMES.TASK_KEY = get_time() + KEY_TIME;
    TASK_TIMES.TASK_ADC = get_time() + ADC_TIME;
    TASK_TIMES.TASK_LED = get_time() + LED_TIME;
}

//获取系统时间
uint32_t get_time(void)
{
    return sys_time;
}

//比较时间
uint32_t get_time_task(uint32_t time)
{
    if(time > sys_time)
    {
        time -= sys_time;
    }
    else
    {
        time = 0;
    }
    return time;
}

void LED_Write_all(uint8_t data)
{
    uint16_t a;
    a = GPIOC->ODR;
    GPIOC->ODR = (uint16_t)data << 8;
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
    GPIOC->ODR = a;
}

//闪烁
void LED_function(void)
{
    if(0 == get_time_task(TASK_TIMES.TASK_LED))	//200ms
    {
        TASK_TIMES.TASK_LED = get_time() + LED_TIME;
        if(Data.LED_RUN_Flag)
        {
            LED_state ^= (1 << 0);	//第一个LED
        }
        else
        {
            LED_state = 0xFF;
        }
        LED_Write_all(LED_state);
    }
}



void ADC_function(void)
{
    uint32_t adc = 0, temp;
    uint8_t count;

    if(0 == get_time_task(TASK_TIMES.TASK_ADC))
    {
        TASK_TIMES.TASK_ADC = get_time() + ADC_TIME;	//重新赋值
        HAL_ADC_Stop_DMA(&hadc2);	//停止ADC
        for(uint8_t i = 0; i < 100 - 1; i++)
        {
            count = 0;
            for(uint8_t j = 0; j < 100 - 1 - i; j++)
            {
                if(ADC_BUFF[j] > ADC_BUFF[j + 1])
                {
                    temp = ADC_BUFF[j];
                    ADC_BUFF[j] = ADC_BUFF[j + 1];
                    ADC_BUFF[j + 1] = temp;
                    count++;
                }
            }
            if(0 == count)
            {
                break;
            }
        }

        for(uint8_t k = 1; k < 100 - 1; k++)
        {
            adc += ADC_BUFF[k];
        }
        HAL_ADC_Start_DMA(&hadc2, (uint32_t *)&ADC_BUFF, 100);
        parameter.My_ADC = (adc / 98) * 3.3f / 4096;
        Data.ADC_Over_Flag = 1;
        if((parameter.My_ADC > (3.3 * parameter.K)) && (0 == parameter.LED_STATE))	//报警
        {
            Data.LED_RUN_Flag = 1;
        }
        else
        {
            Data.LED_RUN_Flag = 0;
        }
    }
}

uint8_t KEY_Proc(void)
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


void KEY_function(void)
{
    static uint8_t Old_VALUE;


    if(0 == get_time_task(TASK_TIMES.TASK_KEY))
    {
        TASK_TIMES.TASK_KEY = get_time() + KEY_TIME;
        KEY_VALUE = KEY_Proc();
        KEY_UP = ~KEY_VALUE & (KEY_VALUE ^ Old_VALUE);
        KEY_DOWN = KEY_VALUE & (KEY_VALUE ^ Old_VALUE);
        Old_VALUE = KEY_VALUE;

        if(KEY_DOWN)
        {
            KEY_COUNT = 0;
        }
        if(KEY_COUNT < 10)
        {
            switch(KEY_UP)
            {
            case 1:
            {
                Data.KEY1_DOWN_Flag = 1;
                break;
            }
            case 2:
            {
                Data.KEY2_DOWN_Flag = 1;
                break;
            }
            case 3:
            {
                if(!Data.MENU_Flag)	//主页面失效
                {
                    break;
                }
                Data.KEY3_DOWN_Flag = 1;
                break;
            }
            case 4:
            {
                if(!Data.MENU_Flag)	//主页面失效
                {
                    break;
                }
                Data.KEY4_DOWN_Flag = 1;
                break;
            }
            default:
                break;
            }
        }
    }
}

void Flag_Run_function(void)
{
    static uint8_t KEY2_Index = 0;
    static uint8_t HMS_Index = 0;	//时分秒选择
    uint8_t Report_Arr[30] = "";

    if(Data.KEY1_DOWN_Flag)	//按1
    {
        Data.KEY1_DOWN_Flag = 0;
        parameter.LED_STATE = !parameter.LED_STATE;	//报警LED功能开关
    }
    if(Data.KEY2_DOWN_Flag)	//按2
    {
        Data.KEY2_DOWN_Flag = 0;
        switch(KEY2_Index)
        {
        case 0:	//第一次按下切换页面
        {
            Data.MENU_Flag = 1;
            LCD_Dis2();
            KEY2_Index = 1;
            break;
        }
        case 1:	//第二次按下更新定时时间且回到主页面
        {

            Alarm_SetTime(parameter.temp_hour, parameter.temp_min, parameter.temp_sec);	//设置闹钟
            Data.MENU_Flag = 0;
            LCD_Dis1();
            KEY2_Index = 0;	//按键索引清0
            HMS_Index = 0;	//时分秒选择清0
            Data.TIMER_CHOOSE_Flag = 0;	//无选择状态，按键4无效
            break;
        }
        default:
            break;
        }
    }
    if(Data.KEY3_DOWN_Flag)	//按3
    {
        Data.KEY3_DOWN_Flag = 0;
        if(1 == KEY2_Index)	//在设置页面才有效
        {
            switch(HMS_Index)
            {
            case 0:	//选中时
            {
                Data.TIMER_CHOOSE_Flag = 1;
                HMS_Index = 1;
                break;
            }
            case 1:	//选中分
            {
                Data.TIMER_CHOOSE_Flag = 2;
                HMS_Index = 2;
                break;
            }
            case 2:	//选中秒
            {
                Data.TIMER_CHOOSE_Flag = 3;
                HMS_Index = 0;
                break;
            }
            default:
                break;
            }
			HightLight(Data.TIMER_CHOOSE_Flag,&parameter);
        }
    }
    if(Data.KEY4_DOWN_Flag)	//按4
    {
        Data.KEY4_DOWN_Flag = 0;
        if(Data.TIMER_CHOOSE_Flag)	//默认是0 不为0表示已经选中任意一个
        {
            switch(Data.TIMER_CHOOSE_Flag)
            {
            case 1:	//小时
            {
                parameter.temp_hour++;
                if(parameter.temp_hour > 23)
                {
                    parameter.temp_hour = 0;
                }
                break;
            }
            case 2:
            {
                parameter.temp_min++;
                if(parameter.temp_min > 59)
                {
                    parameter.temp_min = 0;
                }
                break;
            }
            case 3:
            {
                parameter.temp_sec++;
                if(parameter.temp_sec > 59)
                {
                    parameter.temp_sec = 0;
                }
				
                break;
            }
            default:
                break;
            }
			HightLight(Data.TIMER_CHOOSE_Flag,&parameter);	//高亮
        }
    }
    if(Data.ALARM_Over_Flag)	//触发闹钟 上报电压
    {
        Data.ALARM_Over_Flag = 0;
        sprintf((char *)Report_Arr, "%.2f+%.1f+%02d%02d%02d\n", parameter.My_ADC, parameter.K, my_time.Hours, my_time.Minutes, my_time.Seconds);
        HAL_UART_Transmit(&huart1, (uint8_t *)Report_Arr, sizeof(Report_Arr), 300);
    }
    if(Data.Storage_Success_Flag)
    {
        Data.Storage_Success_Flag = 0;
        EEPROM_SET_Write();
    }
}

//高亮时分秒函数
//参数1：行
//参数2：列（0~19）
//参数3：存储时/分/秒的变量
void HightLight(uint8_t Index,PARAMETER_TypeDef *Sparameter)
{
	LCD_SetBackColor(White);	//高亮前颜色
	LCD_SetTextColor(Black);
	switch(Index)
	{
		case 1:
		{
			LCD_DisplayChar(C3_STATE*24,319-(6*16),Sparameter->temp_hour/10+'0');
			LCD_DisplayChar(C3_STATE*24,319-(7*16),Sparameter->temp_hour%10+'0');
			break;
		}
		case 2:
		{
			LCD_DisplayChar(C3_STATE*24,319-(9*16),Sparameter->temp_min/10+'0');
			LCD_DisplayChar(C3_STATE*24,319-(10*16),Sparameter->temp_min%10+'0');			
			break;
		}
		case 3:
		{
			LCD_DisplayChar(C3_STATE*24,319-(12*16),Sparameter->temp_sec/10+'0');
			LCD_DisplayChar(C3_STATE*24,319-(13*16),Sparameter->temp_sec%10+'0');			
			break;
		}
		default:break;		
	}
	LCD_SetBackColor(Black);	//还原颜色
	LCD_SetTextColor(White);
	switch(Index)
	{
		case 1:
		{
			LCD_DisplayChar(C3_STATE*24,319-(12*16),Sparameter->temp_sec/10+'0');
			LCD_DisplayChar(C3_STATE*24,319-(13*16),Sparameter->temp_sec%10+'0');			

			break;
		}
		case 2:
		{
			LCD_DisplayChar(C3_STATE*24,319-(6*16),Sparameter->temp_hour/10+'0');
			LCD_DisplayChar(C3_STATE*24,319-(7*16),Sparameter->temp_hour%10+'0');						
			break;
		}
		case 3:
		{
			LCD_DisplayChar(C3_STATE*24,319-(9*16),Sparameter->temp_min/10+'0');
			LCD_DisplayChar(C3_STATE*24,319-(10*16),Sparameter->temp_min%10+'0');			
			break;
		}
		default:break;		
	}	
}

void LCD_function(void)
{
    for(uint8_t i = 0; i < 10; i++)
    {
        if(LCD_State & (1 << i))
        {
            LCD_State &= (~(0x01 << i));
            LCD_DisplayStringLine(i * 24, LCD_Line_BUFF[i]);
        }
    }
    if((!Data.MENU_Flag) && Data.ADC_Over_Flag)	//主页面并且采集完才刷新
    {
        Data.ADC_Over_Flag = 0;
        sprintf((char *)LCD_Line_BUFF[C1_STATE], "    V1:%.2fV        ", parameter.My_ADC);
        LCD_State |= (0x01 << C1_STATE);
    }
    if(!Data.MENU_Flag)	//主页面才刷新
    {
        if(parameter.LED_STATE)
        {
            sprintf((char *)LCD_Line_BUFF[C3_STATE], "    LED:OFF          ");
        }
        else
        {
            sprintf((char *)LCD_Line_BUFF[C3_STATE], "    LED:ON          ");
        }
        LCD_State |= (0x01 << C3_STATE);	//刷新
    }
}


void LCD_Dis1(void)
{
    sprintf((char *)LCD_Line_BUFF[C1_STATE], "    V1:%.2fV        ", parameter.My_ADC);
    sprintf((char *)LCD_Line_BUFF[C2_STATE], "    K:%.1f", parameter.K);
    if(parameter.LED_STATE)
    {
        sprintf((char *)LCD_Line_BUFF[C3_STATE], "    LED:OFF          ");
    }
    else
    {
        sprintf((char *)LCD_Line_BUFF[C3_STATE], "    LED:ON          ");
    }
    sprintf((char *)LCD_Line_BUFF[C4_STATE], "    T:%02d-%02d-%02d", my_time.Hours, my_time.Minutes, my_time.Seconds);

    LCD_State |= (0x01 << C1_STATE);
    LCD_State |= (0x01 << C2_STATE);
    LCD_State |= (0x01 << C3_STATE);
    LCD_State |= (0x01 << C4_STATE);
}

void LCD_Dis2(void)
{
    sprintf((char *)LCD_Line_BUFF[C1_STATE], "      setting       ");
    sprintf((char *)LCD_Line_BUFF[C3_STATE], "      %02d-%02d-%02d      ", parameter.temp_hour, parameter.temp_min, parameter.temp_sec);
    LCD_ClearLine(C2_STATE * 24);
    LCD_ClearLine(C4_STATE * 24);
    LCD_State |= (0x01 << C1_STATE);
    LCD_State |= (0x01 << C3_STATE);

}

void USART1_function(void)
{
    uint8_t c1, c2, c3, arr[20];
    float k;
    if(RX_Over_Flag)
    {
        RX_Over_Flag = 0;
        if(6 == RX_LEN)
        {
            sscanf((char *)RX_BUFF, "%c%f%c%c", &c1, &k, &c2, &c3);	//提取
            if(('k' == c1) && ('\\' == c2) && ('n' == c3))	//判断格式
            {
                if(k >= 0.1 && k <= 0.9)	//判断范围
                {
                    parameter.K = k;
                    Data.Storage_Success_Flag = 1;
                    sprintf((char *)LCD_Line_BUFF[C2_STATE], "    K:%.1f", parameter.K);
                    LCD_State |= (0x01 << C2_STATE);
                    HAL_UART_Transmit(&huart1, (uint8_t *)"ok\n", sizeof("ok\n"), 300);
                }
            }
        }

    }
    HAL_UART_Receive_DMA(&huart1, (uint8_t *)RX_BUFF, RX_MAX_LEN);
}


//写入数据
void EEPROM_SET_Write(void)
{
    uint8_t num[1] = {0};
    num[0] = parameter.K * 10;	//扩大10倍当整型存储
    EEPROM_Write(0x00, num, 1);
}

//读取数据
void EEPROM_SET_Read(void)
{
    uint8_t num[1] = {0};
    EEPROM_Read(0x00, num, 1);
    parameter.K = (float)num[0] / 10;	//还原小数【一定要强制类型转换否则结果是0】
}

void EEPROM_Write(uint8_t addr, uint8_t *date, uint8_t len)
{
    I2CStart();
    I2CSendByte(0xA0);
    I2CWaitAck();
    I2CSendByte(addr);
    I2CWaitAck();
    while(len--)
    {
        I2CSendByte(*date++);
        I2CWaitAck();
    }
    I2CStop();
    HAL_Delay(5);
}
void EEPROM_Read(uint8_t addr, uint8_t *date, uint8_t len)
{
    I2CStart();
    I2CSendByte(0xA0);
    I2CWaitAck();
    I2CSendByte(addr);
    I2CWaitAck();

    I2CStart();
    I2CSendByte(0xA1);
    I2CWaitAck();

    while(len--)
    {
        *date++ = I2CReceiveByte();

        if(len)
        {
            I2CSendAck();
        }
        else
        {
            I2CSendNotAck();
        }
    }
    I2CStop();
    HAL_Delay(5);
}

void TASK_Init(void)
{
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    LED_Write_all(0xFF);
    I2CInit();
    HAL_TIM_Base_Start_IT(&htim6);
    HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);	//校准
    HAL_ADC_Start_DMA(&hadc2, (uint32_t *)&ADC_BUFF, 100);
    HAL_RTC_GetTime(&hrtc, &my_time, RTC_FORMAT_BIN);	//获取时间
    HAL_RTC_GetDate(&hrtc, &my_date, RTC_FORMAT_BIN);
    TASK_TIMES_init();	//时间片初始化
    EEPROM_SET_Read();	//读取k
    LCD_Dis1();
}


void GetTime_function(void)
{
    static uint8_t old_time;
    old_time = my_time.Seconds;
    HAL_RTC_GetTime(&hrtc, &my_time, RTC_FORMAT_BIN);	//获取时间
    HAL_RTC_GetDate(&hrtc, &my_date, RTC_FORMAT_BIN);
    if((old_time != my_time.Seconds) && (!Data.MENU_Flag))
    {
        sprintf((char *)LCD_Line_BUFF[C4_STATE], "    T:%02d-%02d-%02d", my_time.Hours, my_time.Minutes, my_time.Seconds);
        LCD_State |= (0x01 << C4_STATE);
    }
}

//闹钟设置
void Alarm_SetTime(uint8_t Hours, uint8_t Minutes, uint8_t Seconds)
{
    RTC_AlarmTypeDef sAlarm = {0};

    sAlarm.AlarmTime.Hours = Hours;
    sAlarm.AlarmTime.Minutes = Minutes;
    sAlarm.AlarmTime.Seconds = Seconds;
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

//闹钟中断回调函数
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    Data.ALARM_Over_Flag = 1;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint8_t timer_100ms;
    if(htim == &htim6)
    {
        sys_time++;
        timer_100ms++;

        if(100 == timer_100ms)	//按键长短计数
        {
            timer_100ms = 0;
            KEY_COUNT++;
        }
    }
}