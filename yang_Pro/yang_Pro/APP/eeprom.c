/***************************************************************************
 * File: eeprom.c
 * Author: Luckys.
 * Date: 2023/06/13
 * description: EEPROM
 -----------------------------------
None
 -----------------------------------
****************************************************************************/
#include "allhead.h"

/*====================================static function declaration area BEGIN====================================*/

static void EEPROM_Write(uint8_t, uint8_t*, uint8_t); // 写入EEPROM
static void EEPROM_Read(uint8_t, uint8_t*, uint8_t); // 读取EEPROM

/*====================================static function declaration area   END====================================*/
EEPROM_t EEPROM = 
{
    EEPROM_Write,
    EEPROM_Read,
};

/*
* @function: EEPROM_Write
* @param: None
* @retval: None
* @brief: 写入EEPROM
*/
static void EEPROM_Write(uint8_t addr, uint8_t *p_Data, uint8_t len)
{
    I2CStart();
    I2CSendByte(0xA0);	//写操作
    I2CSendAck();

    I2CSendByte(addr);
    I2CSendAck();

    while(len--)
    {
        I2CSendByte(*p_Data++);
        I2CSendAck();
        addr++;
        if((addr & 0x07) == 0)
            break;
    }
    I2CStop();
}

/*
* @function: EEPROM_Read
* @param: None
* @retval: None
* @brief: 读取EEPROM
*/
static void EEPROM_Read(uint8_t addr, uint8_t *p_Data, uint8_t len)
{
    I2CStart();
    I2CSendByte(0xA0);	//写操作
    I2CSendAck();

    I2CSendByte(addr);	//需要读的开始地址
    I2CSendAck();
    I2CStart();
    I2CSendByte(0xA1);
    I2CSendAck();

    while(len--)
    {
        *p_Data++ = I2CReceiveByte();

        if(len)
            I2CSendAck();
        else
            I2CSendNotAck();
    }
    I2CStop();
}