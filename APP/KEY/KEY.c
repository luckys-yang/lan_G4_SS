#include "allhead.h"

uint8_t KEY_Time_Count;
uint8_t key_value, key_up, key_down;
uint16_t Long_Num = 1;
uint8_t PWM1_Flag = 0;
uint8_t PWM2_Flag = 0;
static uint8_t pwm1_count,pwm2_count;

extern void enter_standby_mode(void);

unsigned char KEY_Scan(void)  //带有返回值的按键扫描函数
{
    unsigned char ucKey_value;	//定义一个按键值的变量

    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET)	//按键B1按下键值返回为1
        ucKey_value = 1;

    else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET)	//按键B2按下键值返回为2
        ucKey_value = 2;

    else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET)	//按键B3按下键值返回为3
        ucKey_value = 3;

    else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)	//按键B4按下键值返回为4
        ucKey_value = 4;

    else
        ucKey_value = 0;	//无按键按下键值返回为0

    return ucKey_value;
}
void KEY_Proc(void)
{
    static uint8_t key_old;

    key_value = KEY_Scan();
    key_up = ~key_value & (key_old ^ key_value);

    key_down = key_value & (key_old ^ key_value);
    key_old = key_value;

    if(key_down)	//当有按键按下时
    {
        KEY_Time_Count = 0; //将计时器清零 从零开始计时 此处使用了基础定时器用于计时
    }

    if(KEY_Time_Count < 10) //如果计时时间小于1s 短按
    {
        switch(key_up) //判断按键是否抬起 选择键值执行短按的相应程序
        {
        case 1:
            menu_flag = !menu_flag;
		    LCD_Clear(Black);
            break;
        case 2:
			pwm1_count++;
			pwm2_count++;
			if(pwm1_count > 8)
			{
				pwm1_count = 0;
			}
			if(pwm2_count > 4)
			{
				pwm2_count = 0;
			}
			pwm_oc_setCH1(9000+(9000*(0.1*pwm1_count)),70);
			pwm_oc_setCH2(600+(600*(0.2*pwm2_count)),50);
//            light2468_open(1);
            //添加按键功能
            break;

        case 3:
        {
			PWM1_Flag = 1;
//            rtc_time.Minutes = 59;
//            rtc_date.Year = 24;
//            HAL_RTC_SetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
//            HAL_RTC_SetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);
            //TXTest();
            //添加按键功能
            break;
        }
        case 4:
			PWM2_Flag = 1;
//            Buzzer_test(SET);
            //添加按键功能
            break;
        }
    }
    else //长按 计时时间超过1s
    {
        switch(key_value) //判断按键是否按下 选择按键的键值执行相应功能
        {
        case 1:
            light1357_open(0);
            break;

        case 2:
            light2468_open(0);
            //添加按键功能
            break;

        case 3:
			__HAL_RCC_RTC_ENABLE();
//            Long_Num += 1;
            //添加按键功能
            break;

        case 4:
            Buzzer_test(RESET);
            //添加按键功能
            break;
        }

    }
}
