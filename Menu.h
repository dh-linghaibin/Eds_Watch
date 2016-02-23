
#ifndef MENU_H
#define MENU_H

#include "Type.h"

#define HR  0x00
#define SPD 0x01
#define MAX 0x02
#define AVG 0x03

typedef struct Menu{
    u8 area;
    u8 area1;
    u8 area2;
    u8 area3;
    u8 area4;
    u8 area1_time_hour;
    u8 area1_time_minute;
    u8 area1_time_second;
    
    u8 area1_ble;
    u8 area1_ctp;//��������
    u8 area1_power;
    u8 area2_altitude;//����
    u8 area2_tm;//���
    u8 area2_odo;//�����
    
    u8 area3_fs;//ǰ��λ��
    u8 area3_rs;//��λ��
    
    u8 area4_rear;
    
    u8 power;
} 
Menu_bit;

void MenuInit(void);
void MenuSetStall(u8 num1, u8 num2);
void MenuFlickerServerTime(void);
void MenuSetFeatures(u8 com);
void MenuSetBattery(u8 level);
void MenuRefreshTime(void);
void MenuSetStalls(u8 bit, u8 num);

#endif

