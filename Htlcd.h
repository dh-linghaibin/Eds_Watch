#ifndef HTLCD_H
#define HTLCD_H


#include "Type.h"

/********
������ (16,0)
��̾�ţ�(17,0)
��أ�(15,2) (15,3) (15,4) (15,7) (15,6) (15,5)
���Σ�(21,2)
TM�� (21,3)
0D0:
���϶ȣ�(15,0)
��λ��  (15,1)
Km:   
���ʣ�(13,6)
SPD: (13,7)
MAX: (12,6)
AVG:  (12,7)
km/h��(9,2) (9,3)
���£�S5��s6��:  (9,0) (9,1)
FS : (11,6)
�ƣ�  (11,7)
�źţ�(10,6)
�绰�� (10,7)
RS: (9,4)
Ƶ�ʣ�(9,5)
���ã�(9,6)
��ʼ����(9,7)
*******/

void HtlcdInit(void);
void HtlcdDisAll(void);
void HtlcdTime(u8 hour, u8 min, u8 sec);
void HT1622_show_setp(u8 behind_setp,u8 rear_setp);
void HT1622_duan(u8 addr,u8 data);
void HT1622_duan_big_t(u8 addr,u8 data);


#endif
