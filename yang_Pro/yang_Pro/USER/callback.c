/***************************************************************************
 * File: USER
 * Author: Luckys.
 * Date: 2023/06/13
 * description: 中断服务函数
****************************************************************************/
#include "allhead.h"

// 定时器计数中断服务函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim6)
    {
        System.Task_Marks_Handler();    // 任务标记
    }
}

// 串口中断服务函数
void USART1_IRQHandler(void)
{
    if (SET == __HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE))
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1); // 清除接收标志位
        myUSART1.UART_Protocol_Analysis();  // 协议解析
        HAL_UART_Receive_DMA(&huart1, myUSART1.pucRec_Buffer, UART_Rec_MAX);   // 重新打开DMA接收
    }

    HAL_UART_IRQHandler(&huart1);
}