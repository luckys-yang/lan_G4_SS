#ifndef __EEPROM_H
#define __EEPROM_H
#include "main.h"

void eeprom_read_current(uint8_t *dat);
uint8_t eeprom_read_random(uint8_t addr);
void eeprom_read_sequential(uint8_t addr,uint8_t *dat,uint8_t len);
void eeprom_write_byte(uint8_t dat,uint8_t addr);
void eeprom_write_page(uint8_t addr,uint8_t *dat,uint8_t len);
void eeprom_write_u16(uint8_t addr,uint16_t dat);
uint16_t eeprom_read_u16(uint8_t addr);
void mcp_write_byte(uint8_t value);
uint8_t mcp_read_byte(void);



#endif
