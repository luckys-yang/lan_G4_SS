/***************************************************************************
 * File: system.c
 * Author: Luckys.
 * Date: 2023/06/13
 * description: 系统函数
****************************************************************************/
#include "allhead.h"

/*====================================static function declaration area BEGIN====================================*/
static void Hardware_Init(void);    // 硬件初始化
static void System_Run(void);  // 系统运行
static void Task_Marks_Handler(void);   // 任务标记函数
static void Task_Pro_Handler(void); // 任务处理函数

/*====================================static function declaration area   END====================================*/

System_t System = 
{
    Hardware_Init,
    System_Run,
    Task_Marks_Handler,
};

/*
* @function: Hardware_Init
* @param: None
* @retval: None
* @brief: 硬件初始化
*/
static void Hardware_Init(void)
{
    uint8_t MCU_Start_Success_Arr[6] = {0x55,0x01,0x00,0x00,0xA1,0xBB};

    HAL_TIM_Base_Start_IT(&htim6);  // 打开定时器6中断

    I2CInit();  // I2C初始化

    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White); 
    Led.Led_OFF(LED_ALL_OFF);

    HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);	//校准
    HAL_ADC_Start_DMA(&hadc2, (uint32_t *)&myADC.ADC_BUFF, 100); // 使能DMA

    myPWM.PWM_Read_For_EEPROM();    // 读取EEPROM
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);    // PWM使能输出---2500Hz 10%占空比
// 首次上电前需要打开    
#if POWER_ONE    
    myPWM.PA3_Duty = 10;
    myPWM.PA3_Fre = 2500;
    myPWM.PWM_Write_For_EEPROM();
    printf("fre:%d duty: %.0f\r\n",myPWM.PA3_Fre,myPWM.PA3_Duty);
#endif    
    sprintf((char*)Menu.LCD_Display_Buffer[DIS_duty], "PA3_Duty:%.0f%%",myPWM.PA3_Duty);
    Menu.padString((char*)Menu.LCD_Display_Buffer[DIS_duty],20);  
    sprintf((char*)Menu.LCD_Display_Buffer[DIS_fre], "PA3_Fre:%dHz",myPWM.PA3_Fre);
    Menu.padString((char*)Menu.LCD_Display_Buffer[DIS_fre],20);

    __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE); // 使能空闲中断
    HAL_UART_Receive_DMA(&huart1, myUSART1.pucRec_Buffer, UART_Rec_MAX);   // 开启DMA接收
    myUSART1.UART_Send_Arrary(MCU_Start_Success_Arr,sizeof(MCU_Start_Success_Arr)); // 上电发送
    printf(" \r\n");      
}

/*
* @function: System_Run
* @param: None
* @retval: None
* @brief: 系统运行
*/
static void System_Run(void)
{
    Task_Pro_Handler();
}


/*
* @function: Task_Marks_Handler
* @param: None
* @retval: None
* @brief: 任务标记函数
*/
static void Task_Marks_Handler(void)
{
    uint8_t i;

    for (i = 0; i < ucTasks_Max; i++)
    {
        if (Task[i].Task_Cnt)   // 判断计数是否为0
        {
            Task[i].Task_Cnt--; // 递减
            if (0 == Task[i].Task_Cnt)  // 计数到0
            {
                Task[i].Task_Cnt = Task[i].Task_Timer;  // 重装载计数
                Task[i].Run_Status = TRUE;  // 任务执行状态标志置1
            }
        }
    }
}

/*
* @function: Task_Pro_Handler
* @param: None
* @retval: None
* @brief: 任务处理函数
*/
static void Task_Pro_Handler(void)
{
    uint8_t i;

    for (i = 0; i < ucTasks_Max; i++)
    {
        if (Task[i].Run_Status) // 判断执行状态：TRUE--执行 FALSE--不执行
        {
            Task[i].Run_Status = FALSE;
            Task[i].Task_Hook();    // 执行函数
        }
    }
}