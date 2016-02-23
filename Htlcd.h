#ifndef HTLCD_H
#define HTLCD_H


#include "Type.h"

void HtlcdInit(void);
void HtlcdDisAll(void);
void HtlcdTime(u8 hour, u8 min, u8 sec);
void HT1622_show_setp(u8 behind_setp,u8 rear_setp);
void HT1622_duan(u8 addr,u8 data);
void HT1622_duan_big_t(u8 addr,u8 data);
void HtlcdRefresh(void);
void HtlcdOpenBacklight(void);
void HtlcdCloseBacklight(void);
void HtlcdSetSisp(u8 com, u8 seg, u8 com_bit);
void HtlcdSetStalls(u8 bit, u8 num);
void HtlcdSetTotalMileage(u8 num1, u8 num2, u8 num3, u8 num4, 
                          u8 num5, u8 num6, u8 num7);
void HtlcdSetSpeed(u8 num1, u8 num2, u8 num3);

#endif
