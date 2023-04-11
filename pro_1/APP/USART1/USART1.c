#include "allhead.h"

char USART1_RX_TEMP[USART1_RX_LEN] = "\0";	//存放接收数据临时数组
char USART1_RX_BUF[USART1_RX_LEN];	//存放接收数据的最终数组
char USART1_NewData;	//最新接收的字节
uint8_t USART1_Count = 0;	//计数值
uint8_t RX_flag;


RX_Data d1;



//重定向发送函数
void PrintTest(void)
{
	HAL_Delay(1000);
	printf("测试\r\n");
	HAL_Delay(1000);
}

//HAL库发送函数
void TXTest(void)
{
	uint8_t arr[] = "按键按下\r\n";
	HAL_UART_Transmit(&huart1,(uint8_t*)&arr,strlen((char*)arr),0xFFFF);
}



//重定向printf函数
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,0xFFFF);
    return ch;
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	if(&huart1 == huart)	//如果是串口1
//	{	
//		if('\n' == USART1_NewData)	//遇到换行
//		{
//			USART1_RX_TEMP[USART1_Count-1] = '\0';	//把'\n'前面的'\r'去掉
//			sscanf(USART1_RX_TEMP,"%3s:%d:%d",d1.c1,&d1.x,&d1.y);
//			strcpy(USART1_RX_BUF,USART1_RX_TEMP);	//复制数组
//			RX_flag = 1;
//			USART1_Count = 0;
//			memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF));	//初始化为0
//			memset(USART1_RX_TEMP,0,sizeof(USART1_RX_TEMP));	//初始化为0
//		}
//		else
//		{
//			USART1_RX_TEMP[USART1_Count++] = USART1_NewData;			
//		}

//		HAL_UART_Receive_IT(&huart1,(uint8_t*)&USART1_NewData,1);
//	}
//}
