#include "my.h"

PWM_PARAMETER_TypeDef TIM2_CH2 = {0};	//捕获通道2
PWM_PARAMETER_TypeDef TIM2_CH3 = {0};	//捕获通道3
PWM_OCTypeDef PWM_oc = {0};
DATA_TypeDef Data = {0};
PWM_ICTypeDef PWM_ic = {0};
TASK_TIMESTypeDef TASK_TIME;
uint32_t sys_time = 0;
uint8_t LCD_Line_BUFF[10][20] = {""};
uint16_t LCD_State = 0xFFFF;
uint8_t CAPTURE_MODE = 1;


//引脚输出低电平或者高电平
void GPIO_OUT(uint16_t pin,GPIO_PinState PinState)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = pin;	//引脚
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA,pin,PinState);	//高、低
}
//引脚输出PWM
void PWM_OC(uint16_t pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
}

void TIM_IN_function(void)
{
    if(0 == get_time_task(TASK_TIME.TASK_TIMIC))
    {

        TASK_TIME.TASK_TIMIC = get_time() + PWMIC_TIME;

        if(5 == TIM2_CH2.RUN_FLAG)
        {
            PWM_ic.CH2_IC_freq = 1000000 / (TIM2_CH2.CH_FOUR - TIM2_CH2.CH_TWO);
            if((PWM_ic.CH2_IC_freq > 5000) || (PWM_ic.CH2_IC_freq <50))	//大于50KHz小于50Hz
            {
                //PWM_SET_PA6(1000 - 1, 0);	//输出低电平
				GPIO_OUT(GPIO_PIN_6,GPIO_PIN_RESET);	//输出低电平
            }
            else
            {
				PWM_OC(GPIO_PIN_6);	//输出PWM
                HAL_TIM_OC_Stop_IT(&htim3, TIM_CHANNEL_1);
                PWM_SET_PA6(PWM_ic.CH2_IC_freq * PWM_ic.Mult_CH2, 50);
                HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);
            }
            sprintf((char *)LCD_Line_BUFF[C1_STATE], " Channel(1):%dHz", PWM_ic.CH2_IC_freq);	//PA1捕获
            Add_Space(LCD_Line_BUFF, C1_STATE);
            LCD_State |= (0x01 << C1_STATE);
            TIM2_CH2.RUN_FLAG = 0;
            HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);	//重新开启
        }
        if(5 == TIM2_CH3.RUN_FLAG)
        {
            if((PWM_ic.CH3_IC_freq > 50000) || (PWM_ic.CH3_IC_freq <50))	//大于50KHz小于50Hz
            {
                PWM_SET_PA7(1000 - 1, 0);	//输出低电平
            }
            else
            {
                HAL_TIM_OC_Stop_IT(&htim3, TIM_CHANNEL_2);
                PWM_SET_PA7(PWM_ic.CH3_IC_freq * PWM_ic.Mult_CH3, 50);
                HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_2);
            }
            PWM_ic.CH3_IC_freq = 1000000 / (TIM2_CH3.CH_FOUR - TIM2_CH3.CH_TWO);
            sprintf((char *)LCD_Line_BUFF[C3_STATE], " Channel(2):%dHz", PWM_ic.CH3_IC_freq);	//PA1捕获
            Add_Space(LCD_Line_BUFF, C3_STATE);
            LCD_State |= (0x01 << C3_STATE);
            TIM2_CH3.RUN_FLAG = 0;
            HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3);	//重新开启
        }
    }
}

//输出比较
void PWM_SET_PA6(uint32_t freq, uint16_t duty)
{
    PWM_oc.CH1_pluse_val = 1000000 / freq;	//总频率
    PWM_oc.CH1_duty = PWM_oc.CH1_pluse_val * duty / 100;	//有效电平的值
    TIM3->CCR1 = PWM_oc.CH1_pluse_val;
}
void PWM_SET_PA7(uint32_t freq, uint16_t duty)
{
    PWM_oc.CH2_pluse_val = 1000000 / freq;	//总频率
    PWM_oc.CH2_duty = PWM_oc.CH2_pluse_val * duty / 100;	//有效电平的值
    TIM3->CCR2 = PWM_oc.CH2_pluse_val;
}


void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint8_t pwm1 = 1, pwm2 = 1;

    if(htim == &htim3)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            if(pwm1)
            {
                TIM3->CCR1 += PWM_oc.CH1_duty;	//高电平有效时间
            }
            else
            {
                TIM3->CCR1 += (PWM_oc.CH1_pluse_val - PWM_oc.CH1_duty);
            }
            pwm1 = !pwm1;
        }
        else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
        {
            if(pwm2)
            {
                TIM3->CCR2 += PWM_oc.CH2_duty;	//高电平有效时间
            }
            else
            {
                TIM3->CCR2 += (PWM_oc.CH2_pluse_val - PWM_oc.CH2_duty);
            }
            pwm2 = !pwm2;
        }
    }
}

void Add_Space(uint8_t date[][20], uint8_t index)
{
    for(uint8_t i = 0; i < 20; i++)
    {
        if(date[index][i] == '\0')
        {
            date[index][i] = ' ';
        }
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim == &htim2)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
        {
            if(CAPTURE_MODE)
            {
                switch(TIM2_CH2.RUN_FLAG)
                {
                case 0:	//记录第一次的上升沿值 设置下一次为下降沿
                {
                    TIM2_CH2.CH_ONE = TIM2->CCR2;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);	//下降沿
                    TIM2_CH2.RUN_FLAG = 1;
                    break;
                }
                case 1:	//记录第一次的下降沿值 设置下一次为上升沿
                {
                    TIM2_CH2.CH_TWO = TIM2->CCR2;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);	//上升沿
                    TIM2_CH2.RUN_FLAG = 2;
                    break;
                }
                case 2:	//记录第二次的上升沿值 设置下一次为下降沿
                {
                    TIM2_CH2.CH_THREE = TIM2->CCR2;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);	//下降沿
                    TIM2_CH2.RUN_FLAG = 3;
                    break;
                }
                case 3:	//记录第二次的下降沿值 停止捕获 清0
                {
                    TIM2_CH2.CH_FOUR = TIM2->CCR2;
                    HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_2);
                    __HAL_TIM_SetCounter(&htim2, 0);
                    TIM2_CH2.RUN_FLAG = 5;
                    break;
                }
                default:
                    break;
                }
            }
        }
        else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
        {
            if(!CAPTURE_MODE)
            {
                switch(TIM2_CH3.RUN_FLAG)
                {
                case 0:	//记录第一次的上升沿值 设置下一次为下降沿
                {
                    TIM2_CH3.CH_ONE = TIM2->CCR3;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING);	//下降沿
                    TIM2_CH3.RUN_FLAG = 1;
                    break;
                }
                case 1:	//记录第一次的下降沿值 设置下一次为上升沿
                {
                    TIM2_CH3.CH_TWO = TIM2->CCR3;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);	//上升沿
                    TIM2_CH3.RUN_FLAG = 2;
                    break;
                }
                case 2:	//记录第二次的上升沿值 设置下一次为下降沿
                {
                    TIM2_CH3.CH_THREE = TIM2->CCR3;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING);	//下降沿
                    TIM2_CH3.RUN_FLAG = 3;
                    break;
                }
                case 3:	//记录第二次的下降沿值 停止捕获 清0
                {
                    TIM2_CH3.CH_FOUR = TIM2->CCR3;
                    HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_3);
                    __HAL_TIM_SetCounter(&htim2, 0);
                    TIM2_CH3.RUN_FLAG = 5;
                    break;
                }
                default:
                    break;
                }
            }
        }

    }
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

void FlagRun_function(void)
{
    if(Data.KEY1_DOWN_Flag)	//按键1
    {
        Data.KEY1_DOWN_Flag = 0;
        Data.UARTorLOCAL_Flag = !Data.UARTorLOCAL_Flag;
        if(Data.UARTorLOCAL_Flag)
        {
            LED_Dis(0x01, SET);
        }
        else
        {
            LED_Dis(0x01, RESET);
        }
    }
    if(Data.KEY2_DOWN_Flag)
    {
        Data.KEY2_DOWN_Flag = 0;
        Data.CH_SWITCH_Flag = !Data.CH_SWITCH_Flag;
        if(Data.CH_SWITCH_Flag)
        {
            sprintf((char *)LCD_Line_BUFF[C5_STATE], "                  2");	//当前通道号
        }
        else
        {
            sprintf((char *)LCD_Line_BUFF[C5_STATE], "                  1");	//当前通道号
        }
        LCD_State |= (0x01 << C5_STATE);
    }
    if(Data.KEY3_DOWN_Flag)
    {
        Data.KEY3_DOWN_Flag = 0;
        if(!Data.CH_SWITCH_Flag)	//通道1
        {
            PWM_ic.Mult_CH2--;
            if(PWM_ic.Mult_CH2 <= 1)
            {
                PWM_ic.Mult_CH2 = 1;
            }
            sprintf((char *)LCD_Line_BUFF[C2_STATE], " N(1):%d           ", PWM_ic.Mult_CH2);	//PA6倍频
            LCD_State |= (0x01 << C2_STATE);
        }
        else	//通道2
        {
            PWM_ic.Mult_CH3--;
            if(PWM_ic.Mult_CH3 <= 1)
            {
                PWM_ic.Mult_CH3 = 1;
            }
            sprintf((char *)LCD_Line_BUFF[C4_STATE], " N(2):%d           ", PWM_ic.Mult_CH3);	//PA6倍频
            LCD_State |= (0x01 << C4_STATE);
        }
        PWM_Wirte();	//存储
    }
    if(Data.KEY4_DOWN_Flag)
    {
        Data.KEY4_DOWN_Flag = 0;
        if(!Data.CH_SWITCH_Flag)	//通道1
        {
            PWM_ic.Mult_CH2++;
            if(PWM_ic.Mult_CH2 >= 10)
            {
                PWM_ic.Mult_CH2 = 10;
            }
            sprintf((char *)LCD_Line_BUFF[C2_STATE], " N(1):%d           ", PWM_ic.Mult_CH2);	//PA6倍频
            LCD_State |= (0x01 << C2_STATE);
        }
        else	//通道2
        {
            PWM_ic.Mult_CH3++;
            if(PWM_ic.Mult_CH3 >= 10)
            {
                PWM_ic.Mult_CH3 = 10;
            }
            sprintf((char *)LCD_Line_BUFF[C4_STATE], " N(2):%d          ", PWM_ic.Mult_CH3);	//PA6倍频
            LCD_State |= (0x01 << C4_STATE);
        }
        PWM_Wirte();	//存储
    }
}

void USART1_function(void)
{
    uint8_t c1, c2, c3, c4, c5;
    int d1, d2;
    if(RX_Over_Flag)
    {
        RX_Over_Flag = 0;
        if(Data.UARTorLOCAL_Flag)
        {
            if(RX_LEN)
            {
                sscanf((char *)RX_BUFF, "%c%c%c%c%d%c%d", &c1, &c2, &c3, &c4, &d1, &c5, &d2);	//提取
                if(('S' == c1) && ('E' == c2) && ('T' == c3) && (':' == c4) && (':' == c5))
                {
                    if((1 == d1) || (2 == d1))
                    {
                        if(d2 >= 2 && d2 <= 10)
                        {
                            if(1 == d1)
                            {
                                PWM_ic.Mult_CH2 = d2;
                                sprintf((char *)LCD_Line_BUFF[C2_STATE], " N(1):%d           ", PWM_ic.Mult_CH2);	//PA6倍频
                                LCD_State |= (0x01 << C2_STATE);	//刷新
                            }
                            else
                            {
                                PWM_ic.Mult_CH3 = d2;
                                sprintf((char *)LCD_Line_BUFF[C4_STATE], " N(2):%d           ", PWM_ic.Mult_CH3);	//PA6倍频
                                LCD_State |= (0x01 << C4_STATE);	//刷新
                            }
                            PWM_Wirte();	//保存到EEPROM
                        }
                    }
                }
            }
        }
    }
    HAL_UART_Receive_DMA(&huart1, (uint8_t *)RX_BUFF, RX_MAX_LEN);
}

void PWM_Wirte(void)
{
    uint8_t temp[2] = {0};
    temp[0] = PWM_ic.Mult_CH2;
    temp[1] = PWM_ic.Mult_CH3;
    EEPROM_Write(ADDR, temp, 2);
    HAL_Delay(5);
}

void PWM_Read(void)
{
    uint8_t temp[2] = {0};
    EEPROM_Read(ADDR, temp, 2);
    PWM_ic.Mult_CH2 = temp[0];
    PWM_ic.Mult_CH3 = temp[1];
    HAL_Delay(5);
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

uint8_t KEY_UP, KEY_DOWN, KEY_VALUE;

void KEY_function(void)
{
    static uint8_t Old_VALUE;
    if(0 == get_time_task(TASK_TIME.TASK_KEY))
    {
        KEY_VALUE = KEY_Proc();

        KEY_UP = ~KEY_VALUE & (KEY_VALUE ^ Old_VALUE);
        KEY_DOWN = KEY_VALUE & (KEY_VALUE ^ Old_VALUE);
        Old_VALUE = KEY_VALUE;

        switch(KEY_UP)
        {
        case 1:
        {
            Data.KEY1_DOWN_Flag = 1;
            break;
        }
        case 2:
        {
            if(Data.UARTorLOCAL_Flag)	//本地才有效
            {
                break;
            }
            Data.KEY2_DOWN_Flag = 1;
            break;
        }
        case 3:
        {
            if(Data.UARTorLOCAL_Flag)	//本地才有效
            {
                break;
            }
            Data.KEY3_DOWN_Flag = 1;
            break;
        }
        case 4:
        {
            if(Data.UARTorLOCAL_Flag)	//本地才有效
            {
                break;
            }
            Data.KEY4_DOWN_Flag = 1;
            break;
        }
        default:
            break;
        }
        TASK_TIME.TASK_KEY = get_time() + KEY_TIME;
    }
}

void LED_Write_all(uint8_t num)
{
    uint16_t a;
    a = GPIOC->ODR;
    GPIOC->ODR = (uint16_t)num << 8;
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
    GPIOC->ODR = a;
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
}

void LCD_Dis(void)
{
    sprintf((char *)LCD_Line_BUFF[C1_STATE], " Channel(1):%dHz", PWM_ic.CH2_IC_freq);	//PA1捕获
    sprintf((char *)LCD_Line_BUFF[C2_STATE], " N(1):%d           ", PWM_ic.Mult_CH2);	//PA6倍频
    sprintf((char *)LCD_Line_BUFF[C3_STATE], " Channel(2):%dHz", PWM_ic.CH3_IC_freq);	//PA1捕获
    sprintf((char *)LCD_Line_BUFF[C4_STATE], " N(2):%d           ", PWM_ic.Mult_CH3);	//PA6倍频
    sprintf((char *)LCD_Line_BUFF[C5_STATE], "                  1");	//当前通道号默认1

    Add_Space(LCD_Line_BUFF, C1_STATE);
    Add_Space(LCD_Line_BUFF, C3_STATE);
    LCD_State |= (0x01 << C1_STATE);
    LCD_State |= (0x01 << C2_STATE);
    LCD_State |= (0x01 << C3_STATE);
    LCD_State |= (0x01 << C4_STATE);
    LCD_State |= (0x01 << C5_STATE);

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
    TASK_TIME_init();
    PWM_Read();
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);	//开启捕获
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3);	//开启捕获
    HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);
    HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_2);
    LCD_Dis();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint16_t timer_200ms;
    if(htim == &htim6)
    {
        timer_200ms++;
        sys_time++;
        if(200 == timer_200ms)
        {
            timer_200ms = 0;
            CAPTURE_MODE ^= 1;
        }
    }
}

uint32_t get_time_task(uint32_t time)
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

uint32_t get_time(void)
{
    return sys_time;
}

void TASK_TIME_init(void)
{
    TASK_TIME.TASK_KEY = get_time() + KEY_TIME;
    TASK_TIME.TASK_TIMIC = get_time() + PWMIC_TIME;
}