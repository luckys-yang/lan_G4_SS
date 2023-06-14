/***************************************************************************
 * File: task.c
 * Author: Luckys.
 * Date: 2023/06/13
 * description: 任务管理
****************************************************************************/
#include "allhead.h"

/*====================================static function declaration area BEGIN====================================*/

static void TasksHandle_10MS(void); // 任务
static void TasksHandle_50MS(void);   // 任务
static void TasksHandle_100MS(void); // 任务
static void TasksHandle_200MS(void); // 任务
static void TasksHandle_250MS(void);   // 任务
static void TasksHandle_2S(void);   // 任务

/*====================================static function declaration area   END====================================*/

Task_t Task[] = 
{
    {FALSE, 10, 10, TasksHandle_10MS},  // task Period: 10ms
    {FALSE, 50, 50, TasksHandle_50MS},  // task Period: 50ms
    {FALSE, 100, 100, TasksHandle_100MS},  // task Period: 100ms
    {FALSE, 200, 200, TasksHandle_200MS},  // task Period: 200ms
    {FALSE, 250, 250, TasksHandle_250MS},  // task Period: 1000ms    
    {FALSE, 2000, 2000, TasksHandle_2S},  // task Period: 1000ms    
};

/*====================================variable definition declaration area BEGIN===================================*/

// 计算最大任务数量
uint8_t ucTasks_Max = sizeof(Task) / sizeof(Task[0]);

/*====================================variable definition declaration area   END===================================*/

/*
* @function: TasksHandle_10MS
* @param: None
* @retval: None
* @brief: 任务
*/
static void TasksHandle_10MS(void)
{
    Key.Key_Scan(); // 按键扫描
    Key.Key_Handler();  // 按键操作处理
}

/*
* @function: TasksHandle_50MS
* @param: None
* @retval: None
* @brief: 任务
*/
static void TasksHandle_50MS(void)
{
    myADC.ADC_Convert();    // ADC转换
}

/*
* @function: TasksHandle_100MS
* @param: None
* @retval: None
* @brief: 任务
*/
static void TasksHandle_100MS(void)
{
    // LED8状态切换
    if (Led.Led_Uart_Rx_Flag)
    {
        Led.Led_Flip(LED8);
    }
    else
    {
        Led.Led_OFF(LED8);
    }
    // LCD刷新显示
    Menu.Menu_Display();
}

/*
* @function: TasksHandle_200MS
* @param: None
* @retval: None
* @brief: 任务
*/
static void TasksHandle_200MS(void)
{
    // LED1,3,5状态切换
    if (Led.Led_K3_Flag)
    {
        Led.Led_Flip(LED1|LED3|LED5);
    }
    else if (Led.Led_K4_Flag)
    {
        Led.Led_OFF(LED1|LED3|LED5);
    }
}

/*
* @function: TasksHandle_250MS
* @param: None
* @retval: None
* @brief: 任务
*/
static void TasksHandle_250MS(void)
{
    // RTC刷新
    myRTC.RTC_Get_Time();
}

/*
* @function: TasksHandle_2S
* @param: None
* @retval: None
* @brief: 任务
*/
static void TasksHandle_2S(void)
{
    // 判断标志位上传数据到串口助手
    if (myUSART1.Upload_Data_Flag)
    {
        // 上传数据
        sprintf((char*)myUSART1.pucSend_Buffer,"\r\nadc:%.2fV\r\nfre:%dHz\r\nduty:%.0f%%\r\n", myADC.Now_ADC_Value, myPWM.PA3_Fre, myPWM.PA3_Duty);
        myUSART1.UART_Send_String(myUSART1.pucSend_Buffer);
    }
}
