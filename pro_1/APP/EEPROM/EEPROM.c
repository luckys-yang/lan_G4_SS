#include "allhead.h"

//当前地址读取 参数：存储接收的数据
void eeprom_read_current(uint8_t *dat)
{
	I2CStart();
	I2CSendByte(0xA1);	//发送器件地址(读操作)
	I2CWaitAck();
	*dat = I2CReceiveByte();	//读取数据
	I2CSendNotAck();
	I2CStop();
}


//随机地址读取 参数：需要读取的地址
uint8_t eeprom_read_random(uint8_t addr)
{
	uint8_t temp;	//保存数据的临时变量
	
	I2CStart();
	I2CSendByte(0xA0);	//发送器件地址(写操作)
	I2CWaitAck();
	
	I2CSendByte(addr);	//写入需要读取数据所在的地址
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(0xA1);	//发送器件地址(读操作)
	I2CWaitAck();
	
	temp = I2CReceiveByte();	//读取数据
	I2CSendNotAck();
	I2CStop();
	
	return temp;	//返回数据
}

//顺序地址读取 //参数1：想读取的数据地址 参数2：需要保存数据的数组 参数3：要读多少个数据(一个地址一个数据)
void eeprom_read_sequential(uint8_t addr,uint8_t *dat,uint8_t len)
{
	I2CStart();
	I2CSendByte(0xA0);	//发送器件地址(写操作)
	I2CWaitAck();
	
	I2CSendByte(addr);	//写入需要读取数据所在的地址
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(0xA1);
	I2CWaitAck();
	
	while(len--)
	{
		*dat++ = I2CReceiveByte();	//读取数据到数组
		
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

//字节写入 参数1：需要写的数据  参数2：需要写入的地址
void eeprom_write_byte(uint8_t dat,uint8_t addr)
{
	I2CStart();
	I2CSendByte(0xA0);	//发送器件地址(写操作)
	I2CWaitAck();
	
	I2CSendByte(addr);	//发送写入的地址
	I2CWaitAck();
	I2CSendByte(dat);	//发送数据
	I2CWaitAck();
	I2CStop();
}

//页写入 参数1：需要写入的地址 参数2：需要写入的数据(数组) 参数3：数据的长度
void eeprom_write_page(uint8_t addr,uint8_t *dat,uint8_t len)
{
	uint8_t temp = addr;
	
	I2CStart();
	I2CSendByte(0xA0);	//发送器件地址(写操作)
	I2CWaitAck();
	
	I2CSendByte(addr);	//发送写入的地址
	I2CWaitAck();
	while(len--)
	{
		I2CSendByte(*dat++);	//发送数据
		I2CWaitAck();
		temp ++;                    //E2PROM地址递增
		if((temp & 0x07) == 0)	//检查地址是否到达页边界，24C02每页8字节
		{
			break;	//到达页边界时，跳出循环，结束本次写操作
		}
					
	}
	I2CStop();
}

//写入uint16类型 参数1：数据要写入的地址 参数2：数值0~65535
void eeprom_write_u16(uint8_t addr,uint16_t dat)
{
    uint8_t Hn,Ln;	//高位，低位
    Hn = (dat>>8)&0xFF;	//获取高位
    Ln = dat&0xFF;	//获取低8位
    eeprom_write_byte(Ln,addr);	//发送低位
    HAL_Delay(10);
    eeprom_write_byte(Hn,addr+1);	//发送高位
    HAL_Delay(10);
}

//读取uint16类型 参数：数据的地址
uint16_t eeprom_read_u16(uint8_t addr)
{
    uint16_t tmp;
    tmp = (uint16_t)eeprom_read_random(addr);	
    HAL_Delay(10);
    tmp |= (uint16_t)(eeprom_read_random(addr+1)<<8);	//再读取高位，|就是合并也可以用+
    HAL_Delay(10);

    return tmp;
}

//MCP4017写操作
void mcp_write_byte(uint8_t value)
{
	I2CStart();
	I2CSendByte(0x5E);	//写操作
	I2CWaitAck();
	
	I2CSendByte(value);	//发送数据
	I2CWaitAck();
	I2CStop();
}

//MCP4017读操作
uint8_t mcp_read_byte(void)
{
	uint8_t temp;
	
	I2CStart();
	I2CSendByte(0x5F);	//读操作
	I2CWaitAck();
	
	temp = I2CReceiveByte();	//读取数据
	I2CSendNotAck();
	I2CStop();
	
	return temp;	
}


