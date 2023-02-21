#include "my.h"

uint8_t Old_Garde;	//上一次等级
DATA_TypeDef Data = {0};
RARAMETER_TypeDef parameter = {0,0.0,0,0,0,0};
uint16_t LCD_State = 0xFFFF;
uint8_t KEY_UP,KEY_DOWN,KEY_VALUE;
uint8_t KEY_COUNT;
uint32_t ADC_BUFF[10];
uint8_t LCD_Line_BUFF[10][20]  ={""};

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

void KEY_function(void)
{
	static uint8_t Old_Value;
	
	KEY_VALUE = KEY_Proc();
	KEY_UP = ~KEY_VALUE & (KEY_VALUE^Old_Value);
	KEY_DOWN = KEY_VALUE & (KEY_VALUE^Old_Value);
	Old_Value = KEY_VALUE;
	
	if(KEY_DOWN)
	{
		KEY_COUNT = 0;
	}
	if(KEY_COUNT < 10)
	{
		switch(KEY_UP)
		{
			case 1:
			{
				Data.KEY1_DOWN_Flag = 1;
				break;
			}
			case 2:
			{
				if(!Data.MENU_Flag)	//主页面失效
				{
					break;
				}
				Data.KEY2_DOWN_Flag = 1;
				break;
			}
			case 3:
			{
				if(!Data.MENU_Flag)	//主页面失效
				{
					break;
				}				
				Data.KEY3_DOWN_Flag = 1;
				break;
			}
			case 4:
			{
				if(!Data.MENU_Flag)	//主页面失效
				{
					break;
				}				
				Data.KEY4_DOWN_Flag = 1;
				break;
			}
			default:break;			
		}
	}
}

void Flag_Run_function(void)
{
	if(Data.KEY1_DOWN_Flag)
	{
		Data.KEY1_DOWN_Flag = 0;
		Data.MENU_Flag = !Data.MENU_Flag;
		Data.HightLight_State = 0;	//清除高亮否则错乱
		if(Data.MENU_Flag)
		{
			LCD_Dis2();
		}
		else
		{
			//保存到EEPROM并且回到页面1
			Parameter_Write();
			LCD_Dis1();
		}
	}
	if(Data.KEY2_DOWN_Flag)
	{
		Data.KEY2_DOWN_Flag = 0;
		switch(Data.HightLight_State)
		{
			case 0:
			{
				Data.HightLight_State = 1;	//高亮1
				LCD_State |= (0x01<<D1_STATE); 
				break;
			}
			case 1:
			{
				Data.HightLight_State = 2;	//高亮2
				LCD_State |= (0x01<<D2_STATE); 
				break;
			}
			case 2:
			{			
				Data.HightLight_State = 3;	//高亮3
				LCD_State |= (0x01<<D3_STATE);					
				break;
			}
			case 3:
			{
				Data.HightLight_State = 1;	//高亮3
				LCD_State |= (0x01<<D1_STATE);
				break;
			}			
			default:break;
		}
			
	}
	if(Data.KEY3_DOWN_Flag)
	{
		Data.KEY3_DOWN_Flag = 0;
		switch(Data.HightLight_State)	//通过高亮索引来判断增加哪个值
		{
			case 1:
			{
				parameter.Threshold_1 += 5;
				if(parameter.Threshold_1 >= 95)
				{
					parameter.Threshold_1 = 95;
				}
				sprintf((char*)LCD_Line_BUFF[D1_STATE],"  Threshold1:%02dcm   ",parameter.Threshold_1);	
				LCD_State |= (0x01<<D1_STATE);				
				break;
			}
			case 2:
			{
				parameter.Threshold_2 += 5;
				if(parameter.Threshold_2 >= 95)
				{
					parameter.Threshold_2 = 95;
				}
				sprintf((char*)LCD_Line_BUFF[D2_STATE],"  Threshold2:%02dcm   ",parameter.Threshold_2);
				LCD_State |= (0x01<<D2_STATE);				
				break;
			}
			case 3:
			{
				parameter.Threshold_3 += 5;
				if(parameter.Threshold_3 >= 95)
				{
					parameter.Threshold_3 = 95;
				}
				sprintf((char*)LCD_Line_BUFF[D3_STATE],"  Threshold3:%02dcm   ",parameter.Threshold_3);
				LCD_State |= (0x01<<D3_STATE);				
				break;
			}
			default:break;			
		}
	}
	if(Data.KEY4_DOWN_Flag)
	{
		Data.KEY4_DOWN_Flag = 0;
		switch(Data.HightLight_State)	//通过高亮索引来判断增加哪个值
		{
			case 1:
			{
				parameter.Threshold_1 -= 5;
				if(parameter.Threshold_1 <= 5)
				{
					parameter.Threshold_1 = 5;
				}
				sprintf((char*)LCD_Line_BUFF[D1_STATE],"  Threshold1:%02dcm   ",parameter.Threshold_1);	
				LCD_State |= (0x01<<D1_STATE);				
				break;
			}
			case 2:
			{
				parameter.Threshold_2 -= 5;
				if(parameter.Threshold_2 <= 5)
				{
					parameter.Threshold_2 = 5;
				}
				sprintf((char*)LCD_Line_BUFF[D2_STATE],"  Threshold2:%02dcm   ",parameter.Threshold_2);
				LCD_State |= (0x01<<D2_STATE);				
				break;
			}
			case 3:
			{
				parameter.Threshold_3 -= 5;
				if(parameter.Threshold_3 <= 5)
				{
					parameter.Threshold_3 = 5;
				}
				sprintf((char*)LCD_Line_BUFF[D3_STATE],"  Threshold3:%02dcm   ",parameter.Threshold_3);
				LCD_State |= (0x01<<D3_STATE);				
				break;
			}
			default:break;			
		}
	}	
}

void USART1_function(void)
{
	if(RX_Over_Flag)
	{
		RX_Over_Flag = 0;
		if(RX_LEN)
		{
			if(1 == RX_LEN)
			{
				if('C' == RX_BUFF[0])	//发送的是C
				{
					uint8_t temp[30] = "\0";
					
					Data.Inquire_Flag = 1;	//LED闪烁
					//返回当前液位高度和液位等级（需要换行）
					sprintf((char*)temp,"C:H%d+L%d\r\n",parameter.Height,parameter.Level);
					HAL_UART_Transmit(&huart1,(uint8_t*)temp,sizeof(temp),300);
				}
				else if('S' == RX_BUFF[0])	//发送的是S
				{
					uint8_t temp[30] = "\0";
					
					Data.Inquire_Flag = 1;	//LED闪烁
					//返回当前液位高度和液位等级（需要换行）
					sprintf((char*)temp,"S:TL%d+TM%d+TH%d\r\n",parameter.Threshold_1,parameter.Threshold_2,parameter.Threshold_3);
					HAL_UART_Transmit(&huart1,(uint8_t*)temp,sizeof(temp),300);
				}
				else	//无效命令
				{
					HAL_UART_Transmit(&huart1,(uint8_t*)"Invalid command!\r\n",sizeof("Invalid command!\r\n"),300);
				}					
			}	
			else
			{
				HAL_UART_Transmit(&huart1,(uint8_t*)"Invalid command!\r\n",sizeof("Invalid command!\r\n"),300);
			}
		}
		HAL_UART_Receive_DMA(&huart1,(uint8_t*)RX_BUFF,RX_MAX_LEN);
	}
}


void LCD_function(void)
{
	for(uint8_t i = 0;i<10;i++)
	{
		if(LCD_State & (0x01<<i))
		{
			LCD_State &= (~(1<<i));
			if(Data.HightLight_State)
			{
				switch(Data.HightLight_State)
				{
					case 1:
					{
						LCD_SetBackColor(White);
						LCD_SetTextColor(Black);
						LCD_DisplayStringLine(D1_STATE*24,LCD_Line_BUFF[D1_STATE]);
						
						LCD_SetBackColor(Black);
						LCD_SetTextColor(White);
						LCD_DisplayStringLine(D2_STATE*24,LCD_Line_BUFF[D2_STATE]);
						LCD_DisplayStringLine(D3_STATE*24,LCD_Line_BUFF[D3_STATE]);
						break;
					}
					case 2:
					{
						LCD_SetBackColor(White);
						LCD_SetTextColor(Black);
						LCD_DisplayStringLine(D2_STATE*24,LCD_Line_BUFF[D2_STATE]);
						
						LCD_SetBackColor(Black);
						LCD_SetTextColor(White);
						LCD_DisplayStringLine(D1_STATE*24,LCD_Line_BUFF[D1_STATE]);
						LCD_DisplayStringLine(D3_STATE*24,LCD_Line_BUFF[D3_STATE]);
						break;
					}
					case 3:
					{
						LCD_SetBackColor(White);
						LCD_SetTextColor(Black);
						LCD_DisplayStringLine(D3_STATE*24,LCD_Line_BUFF[D3_STATE]);
						
						LCD_SetBackColor(Black);
						LCD_SetTextColor(White);
						LCD_DisplayStringLine(D2_STATE*24,LCD_Line_BUFF[D2_STATE]);
						LCD_DisplayStringLine(D1_STATE*24,LCD_Line_BUFF[D1_STATE]);
						break;
					}					
				}
			}
			else
			{
						LCD_SetBackColor(Black);
						LCD_SetTextColor(White);	
						LCD_DisplayStringLine(i*24,LCD_Line_BUFF[i]);
			}
			
		}
	}
	if((!Data.MENU_Flag) && Data.ADC_Over_Flag)
	{
		Data.ADC_Over_Flag = 0;
		parameter.Height = parameter.ADC*30.3f;	//K约等于30.30
		Judge_Grade();
		sprintf((char*)LCD_Line_BUFF[C1_STATE],"    Height:%dcm     ",parameter.Height);
		sprintf((char*)LCD_Line_BUFF[C2_STATE],"    ADC:%.2fV       ",parameter.ADC);
		sprintf((char*)LCD_Line_BUFF[C3_STATE],"    Level:%d         ",parameter.Level);
		LCD_State |= (0x01<<C1_STATE);
		LCD_State |= (0x01<<C2_STATE);
		LCD_State |= (0x01<<C3_STATE);	
	}	
}

void LCD_Dis1(void)
{
	sprintf((char*)LCD_Line_BUFF[BT_STATE],"     Liquid Level   ");	//标题
	sprintf((char*)LCD_Line_BUFF[C1_STATE],"    Height:%dcm    ",parameter.Height);	
	sprintf((char*)LCD_Line_BUFF[C2_STATE],"    ADC:%.2fV       ",parameter.ADC);	
	sprintf((char*)LCD_Line_BUFF[C3_STATE],"    Level:%d         ",parameter.Level);
	
	LCD_ClearLine(D1_STATE*24);
	LCD_ClearLine(D2_STATE*24);
	LCD_ClearLine(D3_STATE*24);	
	
	LCD_State |= (0x01<<BT_STATE);
	LCD_State |= (0x01<<C1_STATE);
	LCD_State |= (0x01<<C2_STATE);
	LCD_State |= (0x01<<C3_STATE);			
}

void LCD_Dis2(void)
{
	sprintf((char*)LCD_Line_BUFF[BT_STATE],"     Parameter Setup");	//标题
	sprintf((char*)LCD_Line_BUFF[D1_STATE],"  Threshold1:%02dcm   ",parameter.Threshold_1);	
	sprintf((char*)LCD_Line_BUFF[D2_STATE],"  Threshold2:%02dcm   ",parameter.Threshold_2);	
	sprintf((char*)LCD_Line_BUFF[D3_STATE],"  Threshold3:%02dcm   ",parameter.Threshold_3);	
	
	LCD_ClearLine(C1_STATE*24);
	LCD_ClearLine(C2_STATE*24);
	LCD_ClearLine(C3_STATE*24);
	LCD_State |= (0x01<<BT_STATE);
	LCD_State |= (0x01<<D1_STATE);
	LCD_State |= (0x01<<D2_STATE);
	LCD_State |= (0x01<<D3_STATE);
}

void LED_DIS(uint8_t num,uint8_t swch)
{
	if(SET == swch)
	{
		HAL_GPIO_WritePin(GPIOC,(uint16_t)num<<8,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC,(uint16_t)num<<8,GPIO_PIN_SET);
	}
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}

//LED翻转
void LED_Togg(uint8_t num)
{
	HAL_GPIO_TogglePin(GPIOC,(uint16_t)num<<8);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}

void ADC_Filtering(void)
{
	uint8_t count;
	uint32_t temp;
	uint32_t adc_value;
	
	for(uint8_t i = 0;i<10;i++)
	{
		count = 0;
		for(uint8_t j = 0;j<10-i;j++)
		{
			if(ADC_BUFF[j] > ADC_BUFF[j+1])
			{
				temp = ADC_BUFF[j];
				ADC_BUFF[j] = ADC_BUFF[j+1];
				ADC_BUFF[j+1] = temp;
				count++;
			}
		}
		if(0 == count)
		{
			break;
		}
	}
	for(uint8_t k = 1; k<10-1;k++)
	{
		adc_value+= ADC_BUFF[k];
	}
	parameter.ADC = (adc_value/8)*3.3f/4096;
	Data.ADC_Over_Flag = 1;
}

//页写
void EEPROM_Write(uint8_t addr,uint8_t *date,uint8_t len)
{
	I2CStart();
	I2CSendByte(0xA0);	//写操作
	I2CWaitAck();
	
	I2CSendByte(addr);
	I2CWaitAck();
	
	while(len--)
	{
		I2CSendByte(*date++);
		I2CWaitAck();
		addr++;
		if((addr & 0x07) == 0)
		{
			break;
		}
	}
	I2CStop();
	HAL_Delay(10);
}

//页读
void EEPROM_Read(uint8_t addr,uint8_t *date,uint8_t len)
{
	I2CStart();
	I2CSendByte(0xA0);	//写操作
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
			I2CSendAck();
		else
			I2CSendNotAck();
	}
	I2CStop();
	HAL_Delay(10);
}



//写入数据
void Parameter_Write(void)
{
	uint8_t temp[3] = {0};
	temp[0] = parameter.Threshold_1;
	temp[1] = parameter.Threshold_2;
	temp[2] = parameter.Threshold_3;
	
	EEPROM_Write(ADDR,temp,3);
}

//读数据
void Parameter_Read(void)
{
	uint8_t temp[3] = {0};
	
	EEPROM_Read(ADDR,temp,3);
	
	parameter.Threshold_1 = temp[0];
    parameter.Threshold_2 = temp[1];
	parameter.Threshold_3 = temp[2];
}

void TASK_Init(void)
{
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	LED_DIS(0xFF,RESET);                   
	I2CInit();
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc2,(uint32_t*)ADC_BUFF,10);
	Parameter_Read();
	LCD_Dis1();
}

//判断液位等级
void Judge_Grade(void)			
{
	static uint8_t count;//避免上电触发标志位
	count++;
	if(count>1)
	{
		count = 2;
	}
	if(parameter.Height <= parameter.Threshold_1)	//液位高度<=阈值1
	{
		parameter.Level = 0;	//等级：0
	}
	else if(parameter.Height > parameter.Threshold_1 && parameter.Height <= parameter.Threshold_2)	//液位高度大于阈值1且小于等于阈值2
	{
		parameter.Level = 1;	//等级：1
	}
	else if(parameter.Height > parameter.Threshold_2 && parameter.Height <= parameter.Threshold_3)	//液位高度大于阈值2且小于等于阈值3
	{
		parameter.Level = 2;	//等级：2
	}
	else if(parameter.Height > parameter.Threshold_3)	//液位高度大于阈值3
	{
		parameter.Level = 3;	//等级：3	
	}
	
	if((Old_Garde != parameter.Level) && (count >1))	//液位发送改变LED闪烁且上报串口助手
	{
		int8_t x;
		uint8_t temp[30] = "\0";
		x = parameter.Level - Old_Garde;
		if(x > 0)	//表示上升
		{
			sprintf((char*)temp,"A:H%d+L%d+U\r\n",parameter.Height,parameter.Level);
			HAL_UART_Transmit(&huart1,(uint8_t*)temp,sizeof(temp),300);
		}
		else	//表示下降
		{
			sprintf((char*)temp,"A:H%d+L%d+D\r\n",parameter.Height,parameter.Level);
			HAL_UART_Transmit(&huart1,(uint8_t*)temp,sizeof(temp),300);
		}
		Data.Grade_Check_Flag = 1;
	}
	Old_Garde = parameter.Level;
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint8_t timer_20ms;
	static uint8_t timer_100ms;
	static uint16_t timer_200ms;
	static uint16_t timer_1000ms;
	static uint16_t timer_1000ms_2;
	if(htim == &htim6)
	{
		timer_20ms++;
		timer_100ms++;
		timer_1000ms++;
		
		if(10 == timer_20ms)
		{
			timer_20ms = 0;
			KEY_function();
		}
		if(100 == timer_100ms)
		{
			timer_100ms = 0;
			KEY_COUNT++;
		}
		if(1000 == timer_1000ms)
		{
			timer_1000ms = 0;
			LED_Togg(0x01);	//LED1s闪烁表示运行中
			ADC_Filtering();
		}
		if(Data.Grade_Check_Flag)	//液位等级发生改变
		{
			timer_200ms++;
			if(0 == (timer_200ms % 200))
			{
				LED_Togg(0x02);
			}
			if(5000 == timer_200ms)	//5s
			{
				timer_200ms = 0;
				Data.Grade_Check_Flag = 0;
				LED_DIS(0x02,RESET);	//熄灭LED2
			}
		}
		if(Data.Inquire_Flag)	//接收到串口查询命令
		{
			timer_1000ms_2++;
			if(0 == (timer_1000ms_2 % 200))
			{
				LED_Togg(0x04);
			}
			if(5000 == timer_1000ms_2)
			{
				timer_1000ms_2 = 0;
				Data.Inquire_Flag = 0;
				LED_DIS(0x04,RESET);	//熄灭LED3
			}
		}
	}
}