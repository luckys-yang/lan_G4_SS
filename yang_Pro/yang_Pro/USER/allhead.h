#ifndef __ALLHEAD_H
#define __ALLHEAD_H
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "system.h"
#include "callback.h"
#include "led.h"
#include "menu.h"
#include "i2c_hal.h"
#include "eeprom.h"
#include "lcd.h"
#include "key.h"
#include "my_pwm.h"
#include "my_rtc.h"
#include "my_usart1.h"
#include "my_adc.h"

// TRUE/FALSE
typedef enum
{
    FALSE = 0U,
    TRUE = !FALSE 
}FLagStatus_t;

#endif
