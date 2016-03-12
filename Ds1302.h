

#ifndef DS1302_H
#define DS1302_H

#include "Type.h"

void Ds1302Init(void);
u8* Ds1302GetTime(void);
void Ds1302SetTime(u8 set_hour, u8 set_min);

#endif
