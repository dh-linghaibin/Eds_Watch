

#include "Menu.h"
#include "Ds1302.h"
#include "Htlcd.h"
#include "Com.h"
#include "Eeprom.h"

static Menu_bit Menu_Data;

static const u8 flicker[13][2] = {
    6,4,
    7,4,
    6,3,
    7,3,
    //quyu 2
    2,12,
    3,12,
    7,13,
    0,6,
    1,6,
    //����
    7,2,
    5,0,
    6,0,
    7,0,
};

void MenuInit(void) {
    u8 *time;
    if(EepromRead(5) != 0x55) {
        Menu_Data.area3_fs = 0;
        Menu_Data.area3_rs = 0;
        Menu_Data.power = 0;
        EepromWrite(10,Menu_Data.area3_fs);
        EepromWrite(11,Menu_Data.area3_rs);
        EepromWrite(12,Menu_Data.power);
        EepromWrite(5,0x55);
    }
    Menu_Data.area3_fs = EepromRead(10);
    Menu_Data.area3_rs = EepromRead(11);
    Menu_Data.power = EepromRead(12);
    //��ʾ����
    MenuSetBattery(Menu_Data.power); 
    //��ʾǰ��λ
    HtlcdSetStalls(0,Menu_Data.area3_fs);
    HtlcdSetStalls(1,12);
    HtlcdSetStalls(2,12);
    HtlcdSetStalls(3,Menu_Data.area3_rs);
    //show time
    time = Ds1302GetTime();
    HtlcdTime(*(time+2),*(time+1),*(time));//��ʾʱ��
    //��ʾ�����
    HtlcdSetSisp(flicker[Menu_Data.area2+4][0],flicker[Menu_Data.area2+4][1],1);
    HtlcdSetTotalMileage(0,0,0,0,0,0,0);
    //��ʾ�ٶ���Ŀ
    HtlcdSetSisp(flicker[Menu_Data.area2][0],flicker[Menu_Data.area2][1],1);
    HtlcdSetSpeed(0,0,0);
    //ˢ��
    HtlcdRefresh();
}

static u8 flickerbit = 0;//��˸��־λ

void MenuRefreshTime(void) {
    u8 *time;
    if(flickerbit == 0) {
        time = Ds1302GetTime();
        HtlcdTime(*(time+2),*(time+1),*(time));//��ʾʱ��
        Menu_Data.time_h = *(time+2);
        Menu_Data.time_m = *(time+1);
        Menu_Data.time_s = *(time);
    }
}

void MenuSetRead(u8 *bit) {
    if((*bit) < 3) {
        (*bit)++;
    } else {
        (*bit) = 0;
    }
    switch(*bit) {
        case 0: 
        HtlcdSetSisp(6,2,0);
        HtlcdSetSisp(4,0,1);
        HtlcdSetStalls(0,Menu_Data.area3_fs);
        HtlcdSetStalls(3,Menu_Data.area3_rs);
        break;
        case 1:
        HtlcdSetSisp(6,2,1);
        HtlcdSetSisp(4,0,0);
        HtlcdSetStalls(0,Menu_Data.area3_fs);
        HtlcdSetStalls(3,Menu_Data.area3_rs);
        break;
        case 2:
        HtlcdSetSisp(6,2,0);
        HtlcdSetSisp(4,0,0);
        HtlcdSetStalls(0,11);
        HtlcdSetStalls(3,Menu_Data.area3_rs);
        break;
        case 3:
        HtlcdSetSisp(6,2,0);
        HtlcdSetSisp(4,0,0);
        HtlcdSetStalls(0,Menu_Data.area3_fs);
        HtlcdSetStalls(3,11);
        break;
    }
}

static u8 menu_flag = 0;

void MenuSetReadOk(u8 bit,u8 com) {
    switch(bit) {
        case 0:
        ComSendCmdWatch(front,com,0x00,0x00);
        break;
        case 1:
        ComSendCmdWatch(behind,com,0x00,0x00);
        break;
        case 2:
        ComSendCmdWatch(front,set_inti,0x00,0x00);
        HtlcdSetSisp(6,2,1);
        HtlcdSetSisp(4,0,1);
        HtlcdSetStalls(0,Menu_Data.area3_fs);
        HtlcdSetStalls(3,Menu_Data.area3_rs);
        menu_flag = 0;
        break;
        case 3:
        ComSendCmdWatch(behind,set_inti,0x00,0x00);
        HtlcdSetSisp(6,2,1);
        HtlcdSetSisp(4,0,1);
        HtlcdSetStalls(0,Menu_Data.area3_fs);
        HtlcdSetStalls(3,Menu_Data.area3_rs);
        menu_flag = 0;
        break;
    }
}

static u16 flickercount = 0;
static u8 flicker_bit = 0;

void MenuSetFeatures(u8 com) {//��˸
    if(com == 0) {//��ʾ������
        flickerbit = 1;
        flicker_bit = 0;//������˸
        Menu_Data.area = Menu_Data.area3;
        flickercount = 0;
        menu_flag = 1;//set flag
    } else if(com == 1) {//��ʾ�ڶ���
         flickerbit = 1;
         flicker_bit = 0;//������˸
        HtlcdSetSisp(flicker[Menu_Data.area][0],flicker[Menu_Data.area][1],1);
        Menu_Data.area = Menu_Data.area2+4;
        flickercount = 0;
        menu_flag = 2;//set flag
    } else if(com == 2) {//�˳�
        flickerbit = 0;
        HtlcdSetSisp(flicker[Menu_Data.area][0],flicker[Menu_Data.area][1],1);
        flickercount = 0;
        menu_flag = 0;//set flag
    } else if(com == 3) {
        if(flickerbit == 1) {
            HtlcdSetSisp(flicker[Menu_Data.area][0],flicker[Menu_Data.area][1],1);
            if(menu_flag == 1) {
                menu_flag = 3;//set flag
            } else if(menu_flag == 2) {
                menu_flag = 4;//set flag
            } else if(menu_flag == 6) {
                menu_flag = 5;//set flag
            }
        } else {
            flickerbit = 0;
        }
    } else if(com == 4) {//�˵�ҳ
        if(menu_flag == 3) {
            HtlcdSetSisp(flicker[Menu_Data.area][0],flicker[Menu_Data.area][1],0);
            if(Menu_Data.area3 < 3) {
                Menu_Data.area3++;
            } else {
                Menu_Data.area3 = 0;
            }
            if(Menu_Data.area3 == 4) {
                Menu_Data.area = 0; 
            } else {
                Menu_Data.area = Menu_Data.area3; 
            }
        } else if(menu_flag == 4) {
            HtlcdSetSisp(flicker[Menu_Data.area][0],flicker[Menu_Data.area][1],0);
            if(Menu_Data.area2 < 4) {
                Menu_Data.area2++;
            } else {
                Menu_Data.area2 = 0;
            }
            if(Menu_Data.area2 == 5) {
                Menu_Data.area = 4;
            } else {
                Menu_Data.area = Menu_Data.area2+4;
            }
        } else if(menu_flag == 5) {
            HtlcdSetSisp(flicker[Menu_Data.area][0],flicker[Menu_Data.area][1],0);
            if(Menu_Data.area4 < 3) {
                Menu_Data.area4++;
            } else {
                Menu_Data.area4 = 0;
            }
            if(Menu_Data.area4 == 4) {
                Menu_Data.area = 4;
            } else {
                Menu_Data.area = Menu_Data.area4+9;
            }
        } else if(menu_flag == 7) {
            MenuSetRead(&Menu_Data.area4_rear);
        } else if(menu_flag == 8) {
            if(Menu_Data.area < 3) {
                Menu_Data.area++;
            } else {//����ʱ���Ƴ�
                menu_flag = 0;
                flickerbit = 0;
                Ds1302SetTime(Menu_Data.time_h,Menu_Data.time_m);
            }
        } else {
            //����
            ComSendCmdWatch(behind,sub_exchange,0x00,0x00);
        }
    } else if(com == 5) {
        if(menu_flag >= 3) {
            //��ɹ���
            if(menu_flag == 5) {
                flickerbit = 0;//ֹͣ��˸
                HtlcdSetSisp(flicker[Menu_Data.area][0],flicker[Menu_Data.area][1],1);
                if(Menu_Data.area4 == 2) {
                    menu_flag = 7;
                    MenuSetRead(&Menu_Data.area4_rear);
                } else {
                    menu_flag = 0;
                }
            } else if(menu_flag == 7) {
                //������
                //ComSendCmdWatch(Menu_Data.area4_rear,add_setp,0x00,0x00);
                MenuSetReadOk(Menu_Data.area4_rear,add_setp);
            } else if(menu_flag == 8) {//�޸�ʱ��
                if(Menu_Data.area == 0) {
                    if(Menu_Data.time_h < 24) {
                        Menu_Data.time_h++;
                    } else {
                        Menu_Data.time_h = 0;
                    }
                } else if(Menu_Data.area == 1) {
                    if(Menu_Data.time_m < 60) {
                        Menu_Data.time_m++;
                    } else {
                        Menu_Data.time_m = 0;
                    }
                } else if(Menu_Data.area == 2) {
                    if(Menu_Data.time_s < 60) {
                        Menu_Data.time_s++;
                    } else {
                        Menu_Data.time_s = 0;
                    }
                }
            } else {
                menu_flag = 0;
            }
        } else {
            //������
            ComSendCmdWatch(front,add_stal,0x00,0x00);
        }
    } else if(com == 6) {
        HtlcdSetSisp(flicker[Menu_Data.area][0],flicker[Menu_Data.area][1],1);
        Menu_Data.area = Menu_Data.area4+9;
        flicker_bit = 0;//������˸
        flickercount = 0;
        menu_flag = 6;//set flag
    } else if(com == 7) {
        if(menu_flag >= 3) {
            
            //��ɹ���
            if(menu_flag == 5) {
                flickerbit = 0;//ֹͣ��˸
                HtlcdSetSisp(flicker[Menu_Data.area][0],flicker[Menu_Data.area][1],1);
                if(Menu_Data.area4 == 2) {
                    menu_flag = 7;
                    MenuSetRead(&Menu_Data.area4_rear);
                } else {
                    menu_flag = 0;
                }
            } else if(menu_flag == 7) {
                //������
                //ComSendCmdWatch(Menu_Data.area4_rear,sub_setp,0x00,0x00);
                MenuSetReadOk(Menu_Data.area4_rear,sub_setp);
            } else if(menu_flag ==8) {//�޸�ʱ��
                if(Menu_Data.area == 0) {
                    if(Menu_Data.time_h > 0) {
                        Menu_Data.time_h--;
                    } else {
                        Menu_Data.time_h = 24;
                    }
                } else if(Menu_Data.area == 1) {
                    if(Menu_Data.time_m > 0) {
                        Menu_Data.time_m--;
                    } else {
                        Menu_Data.time_m = 59;
                    }
                } else if(Menu_Data.area == 2) {
                    if(Menu_Data.time_s > 0) {
                        Menu_Data.time_s--;
                    } else {
                        Menu_Data.time_s = 59;
                    }
                }
            } else {
                menu_flag = 0;
            }
        } else {
            //������
            ComSendCmdWatch(front,sub_stal,0x00,0x00);
        }
    } else if(com == 8) {
        flickerbit = 1;//��ʼ��˸
        flicker_bit = 2;//ʱ����˸
        menu_flag = 8;
        Menu_Data.area = 0;
    }
}

//��˸ ��������
void MenuFlickerServerTime(void) {
    if(flickercount < 30000) {
        flickercount++;
    } else {
        flickercount = 0;
        if(flickerbit == 1) {
            if(flicker_bit == 0) {
                flicker_bit = 1; 
                HtlcdSetSisp(flicker[Menu_Data.area][0],flicker[Menu_Data.area][1],0);
            } else if(flicker_bit == 1){
                flicker_bit = 0;
                HtlcdSetSisp(flicker[Menu_Data.area][0],flicker[Menu_Data.area][1],1);
            } else if(flicker_bit == 2) {
                flicker_bit = 3;
                HtlcdTime(Menu_Data.time_h,Menu_Data.time_m,Menu_Data.time_s);//��ʾʱ��
            } else if(flicker_bit == 3) {
                flicker_bit = 2;
                HtlcdTimeNull(Menu_Data.area);//��ʾʱ��
            }
        }
        HtlcdRefresh();
    }
}
//���õ��� ��أ�(15,2) (15,3) (15,4) (15,7) (15,6) (15,5)
void MenuSetBattery(u8 level) {
    Menu_Data.power = level;
    EepromWrite(12,Menu_Data.power);
    switch(level) {
        case 5:
            HtlcdSetSisp(3,6,1);
            HtlcdSetSisp(4,6,1);
            HtlcdSetSisp(7,6,1);
            HtlcdSetSisp(6,6,1);
            HtlcdSetSisp(5,6,1);
        break;
        case 4:
            HtlcdSetSisp(3,6,0);
            HtlcdSetSisp(4,6,1);
            HtlcdSetSisp(7,6,1);
            HtlcdSetSisp(6,6,1);
            HtlcdSetSisp(5,6,1);
        break;
        case 3:
            HtlcdSetSisp(3,6,0);
            HtlcdSetSisp(4,6,0);
            HtlcdSetSisp(7,6,1);
            HtlcdSetSisp(6,6,1);
            HtlcdSetSisp(5,6,1);
        break;
        case 2:
            HtlcdSetSisp(3,6,0);
            HtlcdSetSisp(4,6,0);
            HtlcdSetSisp(7,6,0);
            HtlcdSetSisp(6,6,1);
            HtlcdSetSisp(5,6,1);
        break;
        case 1:
            HtlcdSetSisp(3,6,0);
            HtlcdSetSisp(4,6,0);
            HtlcdSetSisp(7,6,0);
            HtlcdSetSisp(6,6,0);
            HtlcdSetSisp(5,6,1);
        break;
        case 0:
            HtlcdSetSisp(3,6,0);
            HtlcdSetSisp(4,6,0);
            HtlcdSetSisp(7,6,0);
            HtlcdSetSisp(6,6,0);
            HtlcdSetSisp(5,6,0);
        break;
    }
}

void MenuSetStalls(u8 bit, u8 num) {
    if(bit == front) {
        Menu_Data.area3_fs = num;
        EepromWrite(10,Menu_Data.area3_fs);
        HtlcdSetStalls(0,Menu_Data.area3_fs);
    } else {
        Menu_Data.area3_rs = 11 - num;
        EepromWrite(11,Menu_Data.area3_rs);
        HtlcdSetStalls(3,Menu_Data.area3_rs);
    }
}