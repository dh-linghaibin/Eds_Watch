

#ifndef COM_H
#define COM_H

#include "Type.h"

void ComInit(void);
u8 ComGetData(u8 num);
u8 ComGetFlag(void);
void ComClearFlag(void);
void ComSendCmdWatch(u8 cmd,u8 par1,u8 par2,u8 par3);
#endif
