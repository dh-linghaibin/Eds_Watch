

#ifndef TIMER_H
#define TIMER_H

#include "Type.h"

void TimerInit(void);
u8 TimerGetSec(void);
void TimerSetSec(u8 data);

u8 TimerGetTimeFlag(void);
void TimerSetTimeFlag(u8 data);

#endif



