#ifndef __USART1_H
#define __USART1_H
#include "main.h"

//接收数组的最大字节
#define USART1_RX_LEN 100

extern char USART1_RX_BUF[USART1_RX_LEN];	//存放接收数据的最终数组
extern uint8_t RX_flag;	//接收标志位
extern char USART1_NewData;	//最新接收的字节

//存放接收数据结构体
typedef struct USART1_Receive{

	char c1[3];
	int x;
	int y;

}RX_Data;
extern RX_Data d1;

void PrintTest(void);
void TXTest(void);

#endif






