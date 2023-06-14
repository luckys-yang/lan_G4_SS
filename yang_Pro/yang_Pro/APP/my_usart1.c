/***************************************************************************
 * File: my_usart1.c
 * Author: Luckys.
 * Date: 2023/06/13
 * description: 串口
 -----------------------------------
协议(以下数据均为十六进制形式发送或接收！！！)：
    收到：
        55 02 00 01 01 BB ---- 上传数据
        55 02 00 01 02 BB ---- 停止上传数据
    发送：
        55 01 00 00 A1 BB ---- 单片机启动成功        
 -----------------------------------
****************************************************************************/
#include "allhead.h"

/*====================================variable definition declaration area BEGIN===================================*/

static uint8_t ucUART_Send_Buff[UART_Send_MAX] = {0x00};    // 发送缓存数组
static uint8_t ucUART_Rec_Buff[UART_Rec_MAX] = {0x00};  // 接收缓存数组

/*====================================variable definition declaration area   END===================================*/

/*====================================static function declaration area BEGIN====================================*/

static void UART_Send_Arrary(uint8_t*, uint16_t);	// 发送数组
static void UART_Send_String(uint8_t*);	// 发送字符串
static void UART_Protocol_Analysis(void);   // 协议解析

/*====================================static function declaration area   END====================================*/

myUSART1_t myUSART1 = 
{
    FALSE,
    ucUART_Send_Buff,
    ucUART_Rec_Buff,
    UART_Send_Arrary,
    UART_Send_String,
    UART_Protocol_Analysis,
};

/*
* @function: UART_Send_Arrary
* @param: p_Arr -> 待发送数组 LEN -> 数组长度(使用sizeof计算)
* @retval: None
* @brief: 发送数组
*/
static void UART_Send_Arrary(uint8_t* p_Arr, uint16_t LEN)
{
    HAL_UART_Transmit_DMA(&huart1,p_Arr,LEN);
}

/*
* @function: UART_Send_String
* @param: p_Str -> 待发送字符串
* @retval: None
* @brief: 发送字符串
*/
static void UART_Send_String(uint8_t* p_Str)
{
    HAL_UART_Transmit_DMA(&huart1,p_Str,strlen((const char*)p_Str));
}

// 重定向printf
int fputc(int ch,FILE* f)
{
	//通过查询的方式循环发送
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0x000A);

	return ch;
}

/*
* @function: UART_Protocol_Analysis
* @param: None
* @retval: None
* @brief: 协议解析
*/
static void UART_Protocol_Analysis(void)
{
    uint8_t i,Index = 0;

    HAL_UART_DMAStop(&huart1);  // 停止DMA接收

    // 过滤数据
    for (i = 0; i < UART_Rec_MAX; i++)
    {
        // 检测起始数据是不是0x55
        if (0 == Index)
        {
            if (*(myUSART1.pucRec_Buffer + i) != 0x55)
            {
                continue;
            }
        }
        *(myUSART1.pucRec_Buffer + Index) = *(myUSART1.pucRec_Buffer + i);

        // 已读6个字节
        if (Index == UART_Order_LENGTH)
        {
            break;
        }
        Index++;
    }

    // 进行数据处理
    if ((*(myUSART1.pucRec_Buffer + 1) == 0x02) && (*(myUSART1.pucRec_Buffer + 2) == 0x00) &&
        (*(myUSART1.pucRec_Buffer + 3) == 0x01) && (*(myUSART1.pucRec_Buffer + 5) == 0xBB))
    {
        switch(*(myUSART1.pucRec_Buffer + 4))   // 判断第5个元素
        {
            case 0x01:Led.Led_Uart_Rx_Flag = TRUE;myUSART1.Upload_Data_Flag = TRUE;break;
            case 0x02:Led.Led_Uart_Rx_Flag = FALSE;myUSART1.Upload_Data_Flag = FALSE;break;
            default:break;
        }
    }

    // 清除缓存
    for (i = 0; i < UART_Rec_MAX; i++)
    {
        *(myUSART1.pucRec_Buffer + i) = 0x00;
    }
}