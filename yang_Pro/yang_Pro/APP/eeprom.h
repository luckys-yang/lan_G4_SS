#ifndef __EEPROM_H
#define __EEPROM_H
#include "allhead.h"


// 存储的地址
#define STORAGE_ADDR	0x08

typedef struct
{
    void (*EEPROM_Write)(uint8_t, uint8_t*, uint8_t);   // 写入EEPROM
    void (*EEPROM_Read)(uint8_t, uint8_t*, uint8_t);    // 读取EEPROM
}EEPROM_t;

extern EEPROM_t EEPROM;

#endif
