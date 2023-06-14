#include "my.h"

/*****************测试数据****************/
#define WR	0

//TIM1默认时间：12:59:50
//TIM2默认时间：07:30:28
//TIM3默认时间：13:22:30
//TIM4默认时间：01:04:20
//TIM5默认时间：02:10:59

/*********************************/


DATA_TypeDef Data;
TIMER_TypeDef TIM_1 = {0};	//位置1定时器
TIMER_TypeDef TIM_2 = {0};	//位置2定时器
TIMER_TypeDef TIM_3 = {0};	//位置3定时器
TIMER_TypeDef TIM_4 = {0};	//位置4定时器
TIMER_TypeDef TIM_5 = {0};	//位置5定时器
TIMER_TypeDef TIM_TEMP = {0};	//缓存时间

static uint8_t HMS_Index = H_INDEX;	//当前时分秒的位置(默认--时)
uint8_t vkey4_count = 0;	//记录定时器初始时间，等定时完成恢复

uint32_t LCD_state = 0xFFFF;
uint8_t LCD_LINE_BuFF[10][20] =
{

    "                    ",
    "                    ",
    "    No 1            ",	//存储位置【3】
    "                    ",
    "                    ",
    "                    ",	//定时时间【6】
    "                    ",
    "                    ",
    "                    ",	//状态【9】
    "                    ",
};




//LCD检测
void LCD_function(void)
{
    uint8_t i;


    for(i = 0; i < 10; i++)
    {
        if(LCD_state & (0x01 << i))
        {
            LCD_state &= (~(1 << i));	//清除状态
            LCD_DisplayStringLine(i * 24, (uint8_t *)LCD_LINE_BuFF[i]);	//先刷新

            if(Data.LCD_HIGH_State)	//再高亮对应位置
            {
                switch(Data.LCD_HIGH_State)
                {
                case H_INDEX:	//高亮--时
                {
                    LCD_SetBackColor(Red);
                    LCD_DisplayChar(Line5, 319 - (16 * 6), LCD_LINE_BuFF[5][6]);
                    LCD_DisplayChar(Line5, 319 - (16 * 7), LCD_LINE_BuFF[5][7]);
                    LCD_SetBackColor(Black);
                    break;
                }
                case M_INDEX:	//高亮--分
                {
                    LCD_SetBackColor(Red);
                    LCD_DisplayChar(Line5, 319 - (16 * 9), LCD_LINE_BuFF[5][9]);
                    LCD_DisplayChar(Line5, 319 - (16 * 10), LCD_LINE_BuFF[5][10]);
                    LCD_SetBackColor(Black);
                    break;
                }
                case S_INDEX:	//高亮--秒
                {
                    LCD_SetBackColor(Red);
                    LCD_DisplayChar(Line5, 319 - (16 * 12), LCD_LINE_BuFF[5][12]);
                    LCD_DisplayChar(Line5, 319 - (16 * 13), LCD_LINE_BuFF[5][13]);
                    LCD_SetBackColor(Black);
                    break;
                }
                default:
                {
                    break;
                }
                }
            }

        }
    }
    if(Data.One_Read_State)	//只在上电执行一次
    {
        Data.One_Read_State = 0;
        Read_Time(&TIM_1, WZ1_ADDR);
    }
}


void TASK_Init(void)
{
    LCD_Init();
    I2CInit();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    HAL_GPIO_WritePin(GPIOC, 0xFF00, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_TIM_Base_Start_IT(&htim6);
    Data.WZ_State = 1;	//默认上电存储位置是1
    Data.One_Read_State = 1;	//读取
#if WR	//写入时间
    TIM_1.Hour = 12;
    TIM_1.Min = 59;
    TIM_1.Sec = 50;
    Write_Time(&TIM_1, WZ1_ADDR);
    TIM_2.Hour = 7;
    TIM_2.Min = 30;
    TIM_2.Sec = 28;
    Write_Time(&TIM_2, WZ2_ADDR);
    TIM_3.Hour = 13;
    TIM_3.Min = 22;
    TIM_3.Sec = 30;
    Write_Time(&TIM_3, WZ3_ADDR);
    TIM_4.Hour = 1;
    TIM_4.Min = 4;
    TIM_4.Sec = 20;
    Write_Time(&TIM_4, WZ4_ADDR);
    TIM_5.Hour = 2;
    TIM_5.Min = 10;
    TIM_5.Sec = 59;
    Write_Time(&TIM_5, WZ5_ADDR);
#endif
}





uint8_t KEY_pro(void)
{

    if((KEY1 == RESET) || (KEY2 == RESET) || (KEY3 == RESET) || (KEY4 == RESET))
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

uint8_t KEY_UP, KEY_DOWN, KEY_COUNT_TIME = 0, KEY_VALUE;



void KEY_function(void)
{
    static uint8_t KEY_Old_value;

    KEY_VALUE = KEY_pro();
    KEY_UP = ~KEY_VALUE & (KEY_VALUE ^ KEY_Old_value);
    KEY_DOWN = KEY_VALUE & (KEY_VALUE ^ KEY_Old_value);
    KEY_Old_value = KEY_VALUE;

    if(KEY_DOWN)
        KEY_COUNT_TIME = 0;

    if(KEY_COUNT_TIME < 10)	//短按
    {
        switch(KEY_UP)
        {
        case 1:	//切换存储位置
        {
            if((RUNNING == Data.Now_State) || (PAUSE == Data.Now_State))	//运行或者暂停状态下按键无效
            {
                break;
            }
            Data.KEY1_Short_Flag = 1;
            break;
        }
        case 2:	//切换时分秒并高亮
        {
            if((RUNNING == Data.Now_State) || (PAUSE == Data.Now_State))	//运行或者暂停状态下按键无效
            {
                break;
            }
            Data.KEY2_Short_Flag = 1;
            Data.Now_State = SETTING;	//设置状态
            break;
        }
        case 3:
        {
            if(SETTING == Data.Now_State)	//如果是在设置状态下才有效
            {
                Data.KEY3_Short_Flag = 1;
            }
            break;
        }
        case 4:
        {
            Data.KEY4_Short_Flag = 1;
            break;
        }
        default:
            break;
        }
    }
    else
    {
        switch(KEY_VALUE)
        {
        case 1:
        {
            break;
        }
        case 2:
        {
            if((RUNNING == Data.Now_State) || (PAUSE == Data.Now_State))	//运行或者暂停状态下按键无效
            {
                break;
            }
            Data.KEY2_Long_Flag = 1;
            Data.Now_State = NULLER;	//设置状态
            break;
        }
        case 3:
        {
            if(SETTING == Data.Now_State)	//如果是在设置状态下才有效
            {
                Data.KEY3_Long_Flag = 1;
            }
            break;
        }
        case 4:
        {
            if((RUNNING == Data.Now_State) || (PAUSE == Data.Now_State))
            {
                Data.KEY4_Long_Flag = 1;
            }
            break;
        }
        default:
            break;
        }
    }
}


//标志位功能实现
void Flag_function(void)
{
    static uint8_t Num = 0;

    if(Data.KEY1_Short_Flag)	//按键1短按
    {
        Data.KEY1_Short_Flag = 0;
        KEY1_Short_function();
    }
    if(Data.KEY2_Short_Flag)	//按键2短按
    {
        Data.KEY2_Short_Flag = 0;
        KEY2_Short_function();
    }
    if(Data.KEY2_Long_Flag)	//按键2长按
    {
        Data.KEY2_Long_Flag = 0;
        Data.LCD_HIGH_State = 0;	//清除高亮
        HMS_Index = 1;	//时分秒控制恢复初始
        LCD_state |= (0x01 << TIME_NUM);
        sprintf((char *)LCD_LINE_BuFF[ZT_NUM], "                    ");
        LCD_state |= (0x01 << ZT_NUM);
        switch(Data.WZ_State)	//判断位置
        {
        case 1:
        {
            Write_Time(&TIM_1, WZ1_ADDR);
            break;
        }
        case 2:
        {
            Write_Time(&TIM_2, WZ2_ADDR);
            break;
        }
        case 3:
        {
            Write_Time(&TIM_3, WZ3_ADDR);
            break;
        }
        case 4:
        {
            Write_Time(&TIM_4, WZ4_ADDR);
            break;
        }
        case 5:
        {
            Write_Time(&TIM_5, WZ5_ADDR);
            break;
        }

        }

    }
    if(Data.KEY3_Short_Flag)	//按键3短按
    {
        Data.KEY3_Short_Flag = 0;
        KEY3_Short_function();
    }
    if(Data.KEY3_Long_Flag)	//按键3长按
    {
        Data.KEY3_Long_Flag = 0;
        KEY3_Short_function();
        HAL_Delay(60);	//触发长按的间隔60ms（这样数字递增的速度会慢点）
    }
    if(Data.KEY4_Short_Flag)	//按键4短按
    {
        Data.KEY4_Short_Flag = 0;
        Data.LCD_HIGH_State = 0;	//清除高亮
        HMS_Index = 1;	//时分秒控制恢复初始
        LCD_state |= (0x01 << TIME_NUM);

        switch(Num)
        {
        case 0:
        {
            Data.Now_State = RUNNING;	//设置状态
            sprintf((char *)LCD_LINE_BuFF[ZT_NUM], "       Running      ");
            LCD_state |= (0x01 << ZT_NUM);
            Data.TIMERT_1S_Count = 0;	//清0
            HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);	//开启PWM模式
            __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, 800);	//输出PWM 1KHz 80%占空比
            Num = 1;
            break;
        }
        case 1:
        {
            Data.Now_State = PAUSE;	//设置状态
            sprintf((char *)LCD_LINE_BuFF[ZT_NUM], "       Pause        ");
            LCD_state |= (0x01 << ZT_NUM);
            Data.TIMERT_1S_Count = 0;	//清0
            HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);	//停止PWM模式
            LED_DIS(0x01, RESET);
            Num = 0;
            break;
        }
        }
    }
    if(Data.KEY4_Long_Flag)	//按键4长按
    {
        Data.KEY4_Long_Flag = 0;
        switch(Data.WZ_State)
        {
        case 1:
        {
            KEY4_Long_function(&TIM_1);
            break;
        }
        case 2:
        {
            KEY4_Long_function(&TIM_2);
            break;
        }
        case 3:
        {
            KEY4_Long_function(&TIM_3);
            break;
        }
        case 4:
        {
            KEY4_Long_function(&TIM_4);
            break;
        }
        case 5:
        {
            KEY4_Long_function(&TIM_5);
            break;
        }
        default:
            break;
        }

    }
    if(STANDBY == Data.Now_State)
    {
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);	//停止PWM模式
        LED_DIS(0x01, RESET);
        Num = 0;	//默认是暂停状态才能保证在standby状态下短按B4就开始运行
    }
}

//按键4长按功能实现
void KEY4_Long_function(TIMER_TypeDef *timx)
{
    timx->Hour = TIM_TEMP.Hour;
    timx->Min = TIM_TEMP.Min;
    timx->Sec = TIM_TEMP.Sec;
    vkey4_count = 0;
    Data.Now_State = STANDBY;		//状态
    sprintf((char *)LCD_LINE_BuFF[ZT_NUM], "      Standby       ");
    LCD_state |= (0x01 << ZT_NUM);
    sprintf((char *)LCD_LINE_BuFF[TIME_NUM], "      %02d:%02d:%02d      ", timx->Hour, timx->Min, timx->Sec);	//刷新时间
    LCD_state |= (0x01 << TIME_NUM);
}

//按键3短按功能实现
void KEY3_Short_function(void)
{
    //先判断当前位置是哪个定时器
    switch(Data.WZ_State)
    {
    case 1:
    {
        switch(Data.LCD_HIGH_State)	//根据高亮的地方判断
        {
        case H_INDEX:	//(0~23)
        {
            TIM_1.Hour += 1;
            if(TIM_1.Hour > 23)
                TIM_1.Hour = 0;
            break;
        }
        case M_INDEX:	//(0~59)
        {
            TIM_1.Min += 1;
            if(TIM_1.Min > 59)
                TIM_1.Min = 0;
            break;
        }
        case S_INDEX:	//(0~59)
        {
            TIM_1.Sec += 1;
            if(TIM_1.Sec > 59)
                TIM_1.Sec = 0;
            break;
        }
        default:
            break;
        }
        sprintf((char *)LCD_LINE_BuFF[TIME_NUM], "      %02d:%02d:%02d      ", (&TIM_1)->Hour, (&TIM_1)->Min, (&TIM_1)->Sec);	//更新时间
        break;
    }
    case 2:
    {
        switch(Data.LCD_HIGH_State)	//根据高亮的地方判断
        {
        case H_INDEX:	//(0~23)
        {
            TIM_2.Hour += 1;
            if(TIM_2.Hour > 23)
                TIM_2.Hour = 0;
            break;
        }
        case M_INDEX:	//(0~59)
        {
            TIM_2.Min += 1;
            if(TIM_2.Min > 59)
                TIM_2.Min = 0;
            break;
        }
        case S_INDEX:	//(0~59)
        {
            TIM_2.Sec += 1;
            if(TIM_2.Sec > 59)
                TIM_2.Sec = 0;
            break;
        }
        default:
            break;
        }
        sprintf((char *)LCD_LINE_BuFF[TIME_NUM], "      %02d:%02d:%02d      ", (&TIM_2)->Hour, (&TIM_2)->Min, (&TIM_2)->Sec);
        break;
    }
    case 3:
    {
        switch(Data.LCD_HIGH_State)	//根据高亮的地方判断
        {
        case H_INDEX:	//(0~23)
        {
            TIM_3.Hour += 1;
            if(TIM_3.Hour > 23)
                TIM_3.Hour = 0;
            break;
        }
        case M_INDEX:	//(0~59)
        {
            TIM_3.Min += 1;
            if(TIM_3.Min > 59)
                TIM_3.Min = 0;
            break;
        }
        case S_INDEX:	//(0~59)
        {
            TIM_3.Sec += 1;
            if(TIM_3.Sec > 59)
                TIM_3.Sec = 0;
            break;
        }
        default:
            break;
        }
        sprintf((char *)LCD_LINE_BuFF[TIME_NUM], "      %02d:%02d:%02d      ", (&TIM_3)->Hour, (&TIM_3)->Min, (&TIM_3)->Sec);
        break;
    }
    case 4:
    {
        switch(Data.LCD_HIGH_State)	//根据高亮的地方判断
        {
        case H_INDEX:	//(0~23)
        {
            TIM_4.Hour += 1;
            if(TIM_4.Hour > 23)
                TIM_4.Hour = 0;
            break;
        }
        case M_INDEX:	//(0~59)
        {
            TIM_4.Min += 1;
            if(TIM_4.Min > 59)
                TIM_4.Min = 0;
            break;
        }
        case S_INDEX:	//(0~59)
        {
            TIM_4.Sec += 1;
            if(TIM_4.Sec > 59)
                TIM_4.Sec = 0;
            break;
        }
        default:
            break;
        }
        sprintf((char *)LCD_LINE_BuFF[TIME_NUM], "      %02d:%02d:%02d      ", (&TIM_4)->Hour, (&TIM_4)->Min, (&TIM_4)->Sec);
        break;
    }
    case 5:
    {
        switch(Data.LCD_HIGH_State)	//根据高亮的地方判断
        {
        case H_INDEX:	//(0~23)
        {
            TIM_5.Hour += 1;
            if(TIM_5.Hour > 23)
                TIM_5.Hour = 0;
            break;
        }
        case M_INDEX:	//(0~59)
        {
            TIM_5.Min += 1;
            if(TIM_5.Min > 59)
                TIM_5.Min = 0;
            break;
        }
        case S_INDEX:	//(0~59)
        {
            TIM_5.Sec += 1;
            if(TIM_5.Sec > 59)
                TIM_5.Sec = 0;
            break;
        }
        default:
            break;
        }
        sprintf((char *)LCD_LINE_BuFF[TIME_NUM], "      %02d:%02d:%02d      ", (&TIM_5)->Hour, (&TIM_5)->Min, (&TIM_5)->Sec);
        break;
    }
    default:
        break;
    }

    LCD_state |= (0x01 << TIME_NUM);	//刷新显示
}


//按键2功能实现
void KEY2_Short_function(void)
{
    sprintf((char *)LCD_LINE_BuFF[ZT_NUM], "       Setting      ");
    LCD_state |= (0x01 << ZT_NUM);
    switch(HMS_Index)	//控制时分秒
    {
    case H_INDEX:	//时
    {
        Data.LCD_HIGH_State = H_INDEX;
        HMS_Index = M_INDEX;
        break;
    }
    case M_INDEX:	//分
    {
        Data.LCD_HIGH_State = M_INDEX;
        HMS_Index = S_INDEX;
        break;
    }
    case S_INDEX:	//秒
    {
        Data.LCD_HIGH_State = S_INDEX;
        HMS_Index = H_INDEX;
        break;
    }
    default:
        break;
    }
    LCD_state |= (0x01 << TIME_NUM);

}





//按键1功能实现
void KEY1_Short_function(void)
{
    switch(Data.WZ_State)
    {
    case 1:	//读取位置2并且显示
    {

        Read_Time(&TIM_2, WZ2_ADDR);
        sprintf((char *)LCD_LINE_BuFF[WZ_NUM], "    No 2            ");
        Data.WZ_State = 2;
        break;
    }
    case 2:	//读取位置3并且显示
    {
        Read_Time(&TIM_3, WZ3_ADDR);
        sprintf((char *)LCD_LINE_BuFF[WZ_NUM], "    No 3            ");
        Data.WZ_State = 3;
        break;
    }
    case 3:	//读取位置4并且显示
    {
        Read_Time(&TIM_4, WZ4_ADDR);
        sprintf((char *)LCD_LINE_BuFF[WZ_NUM], "    No 4            ");
        Data.WZ_State = 4;
        break;
    }
    case 4:	//读取位置5并且显示
    {
        Read_Time(&TIM_5, WZ5_ADDR);
        sprintf((char *)LCD_LINE_BuFF[WZ_NUM], "    No 5            ");
        Data.WZ_State = 5;
        break;
    }
    case 5:	//读取位置1并且显示
    {
        Read_Time(&TIM_1, WZ1_ADDR);
        sprintf((char *)LCD_LINE_BuFF[WZ_NUM], "    No 1            ");
        Data.WZ_State = 1;
        break;
    }
    default:
        break;
    }

    LCD_state |= (0x01 << WZ_NUM);	//刷新

}



void LED_DIS(uint8_t num, uint8_t swch)
{
    if(SET == swch)
    {
        HAL_GPIO_WritePin(GPIOC, (uint16_t)num << 8, GPIO_PIN_RESET);	//点亮
    }
    else
    {
        HAL_GPIO_WritePin(GPIOC, (uint16_t)num << 8, GPIO_PIN_SET);	//熄灭
    }
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}


//页写
void EEPROM_Write(uint8_t addr, uint8_t *Data, uint8_t len)
{
    I2CStart();
    I2CSendByte(0xA0);	//写操作
    I2CSendAck();

    I2CSendByte(addr);
    I2CSendAck();

    while(len--)
    {
        I2CSendByte(*Data++);
        I2CSendAck();
        addr++;
        if((addr & 0x07) == 0)
            break;
    }
    I2CStop();
}

//页读
void EEPROM_Read(uint8_t addr, uint8_t *data, uint8_t len)
{
    I2CStart();
    I2CSendByte(0xA0);	//写操作
    I2CSendAck();

    I2CSendByte(addr);	//需要读的开始地址
    I2CSendAck();
    I2CStart();
    I2CSendByte(0xA1);
    I2CSendAck();

    while(len--)
    {
        *data++ = I2CReceiveByte();

        if(len)
            I2CSendAck();
        else
            I2CSendNotAck();
    }
    I2CStop();
}

//从EEPROM读取时间并且显示
//参数1：读取的时间需要保存在哪个定时器 参数2：定时器地址
void Read_Time(TIMER_TypeDef *tim, uint8_t addr)
{
    uint8_t Read_Temp[3] = {0};
    EEPROM_Read(addr, (uint8_t *)Read_Temp, 3);	//读取
    HAL_Delay(10);
    tim->Hour = Read_Temp[0];
    tim->Min = Read_Temp[1];
    tim->Sec = Read_Temp[2];
    sprintf((char *)LCD_LINE_BuFF[TIME_NUM], "      %02d:%02d:%02d      ", tim->Hour, tim->Min, tim->Sec);	//刷新时间
    LCD_state |= (0x01 << TIME_NUM);
}

//往EEPROM写入时间
//参数1：需要写入的定时器 参数2：定时器地址
void Write_Time(TIMER_TypeDef *tim, uint8_t addr)
{
    uint8_t Write_Temp[3] = {0};
    Write_Temp[0] = tim->Hour;
    Write_Temp[1] = tim->Min;
    Write_Temp[2] = tim->Sec;
    EEPROM_Write(addr, (uint8_t *)Write_Temp, 3);
    HAL_Delay(10);
}

//倒计时
void Timer_CountDown(TIMER_TypeDef *timx)
{
    timx->Sec--;
    if(timx->Sec == 255)	//注意溢出，无符号0之后变255
    {
        timx->Min--;
        timx->Sec = 59;
    }
    if(timx->Min == 255)
    {
        timx->Hour--;
        timx->Min = 59;
    }


    if((timx->Hour == 255) && (timx->Min == 59) && (timx->Sec == 59))	//停止 0:0:0 再减1 就是 255:59:59
    {
        //恢复初值
        timx->Hour = TIM_TEMP.Hour;
        timx->Min = TIM_TEMP.Min;
        timx->Sec = TIM_TEMP.Sec;
        vkey4_count = 0;
        Data.Now_State = STANDBY;		//状态
        sprintf((char *)LCD_LINE_BuFF[ZT_NUM], "      Standby       ");
        LCD_state |= (0x01 << ZT_NUM);
    }
    sprintf((char *)LCD_LINE_BuFF[TIME_NUM], "      %02d:%02d:%02d      ", timx->Hour, timx->Min, timx->Sec);	//刷新时间
    LCD_state |= (0x01 << TIME_NUM);
}



uint32_t sys_time = 0;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint16_t count = 0;
    static uint16_t key_count = 0;
    static uint16_t LED_500ms_count = 0;

    if(htim == &htim6)
    {
        sys_time++;
        count++;
        key_count++;
        Data.TIMERT_1S_Count++;

        if(10 == count)	//按键扫描
        {
            count = 0;
            KEY_function();
        }
        if(80 == key_count)	//长按计数
        {
            key_count = 0;
            KEY_COUNT_TIME++;
        }
        if(RUNNING == Data.Now_State)
        {
            LED_500ms_count++;
            if(500 == LED_500ms_count)
            {
                LED_500ms_count = 0;
                HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);	//翻转电平
                HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
                HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
            }

            if(1000 == Data.TIMERT_1S_Count)
            {
                Data.TIMERT_1S_Count = 0;
                switch(Data.WZ_State)
                {
                case 1:
                {
                    if(0 == vkey4_count)
                    {
                        TIM_TEMP.Hour = TIM_1.Hour;
                        TIM_TEMP.Min = TIM_1.Min;
                        TIM_TEMP.Sec = TIM_1.Sec;
                        vkey4_count++;
                    }
                    Timer_CountDown(&TIM_1);
                    break;
                }
                case 2:
                {
                    if(0 == vkey4_count)
                    {
                        TIM_TEMP.Hour = TIM_2.Hour;
                        TIM_TEMP.Min = TIM_2.Min;
                        TIM_TEMP.Sec = TIM_2.Sec;
                        vkey4_count++;
                    }
                    Timer_CountDown(&TIM_2);
                    break;
                }
                case 3:
                {
                    if(0 == vkey4_count)
                    {
                        TIM_TEMP.Hour = TIM_3.Hour;
                        TIM_TEMP.Min = TIM_3.Min;
                        TIM_TEMP.Sec = TIM_3.Sec;
                        vkey4_count++;
                    }
                    Timer_CountDown(&TIM_3);
                    break;
                }
                case 4:
                {
                    if(0 == vkey4_count)
                    {
                        TIM_TEMP.Hour = TIM_4.Hour;
                        TIM_TEMP.Min = TIM_4.Min;
                        TIM_TEMP.Sec = TIM_4.Sec;
                        vkey4_count++;
                    }
                    Timer_CountDown(&TIM_4);
                    break;
                }
                case 5:
                {
                    if(0 == vkey4_count)
                    {
                        TIM_TEMP.Hour = TIM_5.Hour;
                        TIM_TEMP.Min = TIM_5.Min;
                        TIM_TEMP.Sec = TIM_5.Sec;
                        vkey4_count++;
                    }
                    Timer_CountDown(&TIM_5);
                    break;
                }
                default:
                    break;
                }
            }

        }
    }
}
