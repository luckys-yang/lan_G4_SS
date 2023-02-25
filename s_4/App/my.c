#include "my.h"


uint32_t sys_time = 0;
TASK_TIMESTypeDef Task_time = {0};
DATA_TypeDef Data = {0};
PARAMETER_TypeDef Parameter = {0};
uint16_t LCD_State = 0xFFFF;
uint8_t LCD_Line_BUFF[10][20] = {""};
RTC_TimeTypeDef my_sTime = {0};
RTC_DateTypeDef my_sDate = {0};
static uint8_t KEY1_Index;
static uint8_t KEY3_Index;
uint8_t KEY_UP, KEY_DOWN, KEY_VALUE;


uint32_t get_time(void)
{
    return sys_time;
}

uint32_t get_task_time(uint32_t time)
{
    if(time > get_time())
    {
        time--;
    }
    else
    {
        time = 0;
    }
    return time;
}

void TASK_TIME_init(void)
{
    Task_time.TASK_KEY = get_time() + KEY_TIME;
}

void LED_Dis(uint8_t num, uint8_t swch)
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

    if(Data.DATA_OVER_FLAG)
    {
        Data.DATA_OVER_FLAG = 0;
        memset(LCD_Line_BUFF[C6_STATE], 0, sizeof(LCD_Line_BUFF[C6_STATE]));
        sprintf((char *)LCD_Line_BUFF[C6_STATE], "  %02d:%02d:%02d-PA%d-%dS", Parameter.RX_hour, Parameter.RX_min, Parameter.RX_sec, Parameter.CHxNumber, Parameter.RunTime_Count);
        Add_Space(LCD_Line_BUFF, C6_STATE);
        LCD_DisplayStringLine(C6_STATE * 24, LCD_Line_BUFF[C6_STATE]);
        Alarm_SetTime(Parameter.RX_hour, Parameter.RX_min, Parameter.RX_sec);	//设置闹钟
    }

    //以下是刷新输出通道那行
    memset(LCD_Line_BUFF[C4_STATE], 0, sizeof(LCD_Line_BUFF[C4_STATE]));	//清空数组
    if(KEY1_Index && (!KEY3_Index))	//PA1输出PA2没输出
    {
        sprintf((char *)LCD_Line_BUFF[C4_STATE], "  Channel: PA1");
        Add_Space(LCD_Line_BUFF, C4_STATE);
        LCD_DisplayStringLine(C4_STATE * 24, LCD_Line_BUFF[C4_STATE]);
    }
    else if(KEY3_Index && (!KEY1_Index))	//PA2输出PA1没输出
    {
        sprintf((char *)LCD_Line_BUFF[C4_STATE], "  Channel: PA2");
        Add_Space(LCD_Line_BUFF, C4_STATE);
        LCD_DisplayStringLine(C4_STATE * 24, LCD_Line_BUFF[C4_STATE]);
    }
    else if(KEY1_Index && KEY3_Index)	//两个同时输出
    {
        sprintf((char *)LCD_Line_BUFF[C4_STATE], "  Channel: PA1,PA2");
        Add_Space(LCD_Line_BUFF, C4_STATE);
        LCD_DisplayStringLine(C4_STATE * 24, LCD_Line_BUFF[C4_STATE]);
    }
    else	//都没输出
    {
        sprintf((char *)LCD_Line_BUFF[C4_STATE], "  Channel: None");
        Add_Space(LCD_Line_BUFF, C4_STATE);
        LCD_DisplayStringLine(C4_STATE * 24, LCD_Line_BUFF[C4_STATE]);
    }
}

void LCD_Dis1(void)
{
    sprintf((char *)LCD_Line_BUFF[C1_STATE], "  PWM_PA1:%d%%", Parameter.PA1_duty);
    Add_Space(LCD_Line_BUFF, C1_STATE);
    sprintf((char *)LCD_Line_BUFF[C2_STATE], "  PWM_PA2:%d%%", Parameter.PA2_duty);
    Add_Space(LCD_Line_BUFF, C3_STATE);
    sprintf((char *)LCD_Line_BUFF[C3_STATE], "  Time:%02d:%02d:%02d", my_sTime.Hours, my_sTime.Minutes, my_sTime.Seconds);
    Add_Space(LCD_Line_BUFF, C3_STATE);
    sprintf((char *)LCD_Line_BUFF[C4_STATE], "  Channel: None");
    Add_Space(LCD_Line_BUFF, C4_STATE);
    sprintf((char *)LCD_Line_BUFF[C5_STATE], "  Command:");
    Add_Space(LCD_Line_BUFF, C5_STATE);
    sprintf((char *)LCD_Line_BUFF[C6_STATE], "       None");
    Add_Space(LCD_Line_BUFF, C6_STATE);

    LCD_State |= (0x01 << C1_STATE);
    LCD_State |= (0x01 << C2_STATE);
    LCD_State |= (0x01 << C3_STATE);
    LCD_State |= (0x01 << C4_STATE);
    LCD_State |= (0x01 << C5_STATE);
    LCD_State |= (0x01 << C6_STATE);
}

void Add_Space(uint8_t date[][20], uint8_t State)
{
    for(uint8_t i = 0; i < 20; i++)
    {
        if(date[State][i] == '\0')
        {
            date[State][i] = ' ';
        }
    }
}

//闹钟设置时间
void Alarm_SetTime(uint16_t hour, uint16_t min, uint16_t sec)
{
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

//回调函数
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    if(Parameter.RunTime_Count != 0)
    {
        Data.Alarm_OVER_FLAG = 1;
    }
}


void RtcGetTime_function(void)
{
    static int8_t Old_Time = -1;

    HAL_RTC_GetTime(&hrtc, &my_sTime, RTC_FORMAT_BIN);	//获取时间
    HAL_RTC_GetDate(&hrtc, &my_sDate, RTC_FORMAT_BIN);

    if(Old_Time != my_sTime.Seconds)
    {
        sprintf((char *)LCD_Line_BUFF[C3_STATE], "  Time:%02d:%02d:%02d", my_sTime.Hours, my_sTime.Minutes, my_sTime.Seconds);
        Add_Space(LCD_Line_BUFF, C3_STATE);
        LCD_State |= (0x01 << C3_STATE);
    }
    Old_Time = my_sTime.Seconds;
}

void TASK_init(void)
{
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    LED_Dis(0xFF, RESET);
    I2CInit();
    HAL_TIM_Base_Start_IT(&htim6);
    HAL_RTC_GetTime(&hrtc, &my_sTime, RTC_FORMAT_BIN);	//获取时间
    HAL_RTC_GetDate(&hrtc, &my_sDate, RTC_FORMAT_BIN);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
#if 0	//默认占空比是10 10(需要重置EEPROM里的数据时打开)
    Parameter.PA1_duty = 10;
    Parameter.PA2_duty = 10;
    Parameter_Write();
#endif
    Parameter_Read();
    LCD_Dis1();
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
    static uint8_t Old_Value;

    if(0 == get_task_time(Task_time.TASK_KEY))
    {
        Task_time.TASK_KEY = get_time() + KEY_TIME;
        KEY_VALUE = KEY_Proc();
        KEY_UP = ~KEY_VALUE & (KEY_VALUE ^ Old_Value);
        KEY_DOWN = KEY_VALUE & (KEY_VALUE ^ Old_Value);
        Old_Value = KEY_VALUE;

        switch(KEY_UP)
        {
        case 1:
        {
            Data.KEY1_DOWN_FLAG = 1;
            break;
        }
        case 2:
        {
            Data.KEY2_DOWN_FLAG = 1;
            break;
        }
        case 3:
        {
            Data.KEY3_DOWN_FLAG = 1;
            break;
        }
        case 4:
        {
            Data.KEY4_DOWN_FLAG = 1;
            break;
        }
        default:
            break;
        }
    }
}

//配置低电平
void GPIO_Out(uint16_t Pin, GPIO_PinState PinState)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA, Pin, PinState);	//拉高/拉低
}

//配置PWM
void PWM_Out(uint16_t Pin)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void FlagRun_function(void)
{

    if(Data.KEY1_DOWN_FLAG)
    {
        Data.KEY1_DOWN_FLAG = 0;
        KEY1_Index = !KEY1_Index;	//取反
        if(KEY1_Index)	//PA1输出脉冲
        {
            PWM_Out(GPIO_PIN_1);
            TIM2->CCR2 = Parameter.PA1_duty * 10;
            LED_Dis(0x01, SET);	//LED1亮
        }
        else	//PA1输出低电平
        {
            GPIO_Out(GPIO_PIN_1, GPIO_PIN_RESET);
            LED_Dis(0x01, RESET);	//LED1灭
        }
    }
    if(Data.KEY2_DOWN_FLAG)	//调整占空比并且保存到EEPROM
    {
        Data.KEY2_DOWN_FLAG = 0;
        Parameter.PA1_duty += 10;
        if(Parameter.PA1_duty > 90)
        {
            Parameter.PA1_duty = 10;
        }
        if(KEY1_Index)	//在PA1输出脉冲时才写入寄存器否则不写入
        {
            TIM2->CCR2 = Parameter.PA1_duty * 10;
        }
        sprintf((char *)LCD_Line_BUFF[C1_STATE], "  PWM_PA1:%d%%", Parameter.PA1_duty);
        Add_Space(LCD_Line_BUFF, C1_STATE);
        LCD_State |= (0x01 << C1_STATE);
        Parameter_Write();
    }
    if(Data.KEY3_DOWN_FLAG)
    {
        Data.KEY3_DOWN_FLAG = 0;
        KEY3_Index = !KEY3_Index;	//取反
        if(KEY3_Index)	//PA2输出脉冲
        {
            PWM_Out(GPIO_PIN_2);
            TIM2->CCR3 = Parameter.PA2_duty * 10;
            LED_Dis(0x02, SET);	//LED2亮
        }
        else	//PA2输出低电平
        {
            GPIO_Out(GPIO_PIN_2, GPIO_PIN_RESET);
            LED_Dis(0x02, RESET);	//LED2灭
        }
    }
    if(Data.KEY4_DOWN_FLAG)
    {
        Data.KEY4_DOWN_FLAG = 0;
        Parameter.PA2_duty += 10;
        if(Parameter.PA2_duty > 90)
        {
            Parameter.PA2_duty = 10;
        }
        if(KEY3_Index)	//在PA2输出脉冲时才写入寄存器否则不写入
        {
            TIM2->CCR3 = Parameter.PA2_duty * 10;
        }
        sprintf((char *)LCD_Line_BUFF[C2_STATE], "  PWM_PA2:%d%%", Parameter.PA2_duty);
        Add_Space(LCD_Line_BUFF, C2_STATE);
        LCD_State |= (0x01 << C2_STATE);
        Parameter_Write();
    }
    if(Data.Alarm_OVER_FLAG)
    {
        Data.Alarm_OVER_FLAG = 0;
        if(1 == Parameter.CHxNumber)
        {
            PWM_Out(GPIO_PIN_1);
            TIM2->CCR2 = Parameter.PA1_duty * 10;	//需要写入不然不会触发
            LED_Dis(0x01, SET);	//LED1亮
        }
        else if(2 == Parameter.CHxNumber)
        {
            PWM_Out(GPIO_PIN_2);
            TIM2->CCR3 = Parameter.PA2_duty * 10;
            LED_Dis(0x02, SET);	//LED2亮
        }
        Data.RUN_START_FLAG = 1;
    }
    if(Data.RUN_START_OVER_FLAG)	//脉冲输出完成
    {
        Data.RUN_START_OVER_FLAG = 0;
        if(1 == Parameter.CHxNumber)
        {
            GPIO_Out(GPIO_PIN_1, GPIO_PIN_RESET);	//输出低电平
            LED_Dis(0x01, RESET);	//LED1灭
        }
        else if(2 == Parameter.CHxNumber)
        {
            GPIO_Out(GPIO_PIN_2, GPIO_PIN_RESET);	//输出低电平
            LED_Dis(0x02, RESET);	//LED2灭
        }
    }
}


//接收的数据格式：hh:mm:ss-PAx-yS（0<y<10）
//不考虑换行
void USART1_function(void)
{
    int8_t hh, mm, ss, x, y;

    if(RX_Over_Flag)
    {
        RX_Over_Flag = 0;
        if(15 == RX_LEN)
        {
            //先判断固定的那些字符是否相等
            if(':' == RX_BUFF[2] && ':' == RX_BUFF[5] && '-' == RX_BUFF[8] && 'P' == RX_BUFF[9] && 'A' == RX_BUFF[10] && '-' == RX_BUFF[12] && 'S' == RX_BUFF[14])
            {
                hh = Hour_DealWith(RX_BUFF);	//小时处理
                if(hh != -1)
                {
                    mm = Min_DealWith(RX_BUFF);	//分钟处理
                    if(mm != -1)
                    {
                        ss = Sec_DealWith(RX_BUFF);	//秒处理
                        if(ss != -1)
                        {
                            if(('1' == RX_BUFF[11]) || ('2' == RX_BUFF[11]))
                            {
                                if(RX_BUFF[13] > '0' && RX_BUFF[13] <= '9')
                                {
                                    Data.DATA_OVER_FLAG = 1;
                                    Parameter.RX_hour = hh;
                                    Parameter.RX_min = mm;
                                    Parameter.RX_sec = ss;
                                    Parameter.CHxNumber = RX_BUFF[11] - '0';
                                    Parameter.RunTime_Count = RX_BUFF[13] - '0';
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    HAL_UART_Receive_DMA(&huart1, RX_BUFF, RX_MAX_LEN);
}

//小时处理
int8_t Hour_DealWith(uint8_t *date)
{
    uint8_t a, b, c;
    a = date[0] - '0';
    b = date[1] - '0';
    c = a * 10 + b;

    if(a >= 0 && a <= 2)
    {
        if(b >= 0 && b <= 9)
        {
            if(c >= 0 && c <= 23)
            {
                return c;
            }
        }
    }
    return -1;
}
//分钟处理
int8_t Min_DealWith(uint8_t *date)
{
    uint8_t a, b, c;
    a = date[3] - '0';
    b = date[4] - '0';
    c = a * 10 + b;

    if(a >= 0 && a <= 5)
    {
        if(b >= 0 && b <= 9)
        {
            if(c >= 0 && c <= 59)
            {
                return c;
            }
        }
    }
    return -1;
}
//秒处理
int8_t Sec_DealWith(uint8_t *date)
{
    uint8_t a, b, c;
    a = date[6] - '0';
    b = date[7] - '0';
    c = a * 10 + b;

    if(a >= 0 && a <= 5)
    {
        if(b >= 0 && b <= 9)
        {
            if(c >= 0 && c <= 59)
            {
                return c;
            }
        }
    }
    return -1;
}

//写数据
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
        addr++;
        if((addr & 0x07) == 0)
        {
            break;
        }
    }
    I2CStop();
    HAL_Delay(5);
}
//读数据
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
}

//写入参数EEPROM
void Parameter_Write(void)
{
    uint8_t temp[2];
    temp[0] = Parameter.PA1_duty;
    temp[1] = Parameter.PA2_duty;

    EEPROM_Write(ADDR, temp, 2);
}

//读取参数EEPROM
void Parameter_Read(void)
{
    uint8_t temp[2];
    EEPROM_Read(ADDR, temp, 2);
    Parameter.PA1_duty = temp[0];
    Parameter.PA2_duty = temp[1];
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint16_t timer_x;

    if(htim == &htim6)
    {
        sys_time++;

        if(Data.RUN_START_FLAG)
        {
            timer_x++;
            if((Parameter.RunTime_Count * 1000) == timer_x)	//运行到规定秒数
            {
                timer_x = 0;
                Data.RUN_START_FLAG = 0;
                Data.RUN_START_OVER_FLAG = 1;
            }
        }
    }
}