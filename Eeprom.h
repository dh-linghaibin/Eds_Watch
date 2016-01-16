


#ifndef EEPROM_H
#define EEPROM_H

#include "Type.h"

void EeepromInit(void);
void EepromWrite(u8 addr,u8 dat);
u8 EepromRead(u8 addr);

#endif
