

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
    //区域
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
    //显示电量
    MenuSetBattery(Menu_Data.power); 
    //显示前后档位
    HtlcdSetStalls(0,Menu_Data.area3_fs);
    HtlcdSetStalls(1,11);
    HtlcdSetStalls(2,11);
    HtlcdSetStalls(3,Menu_Data.area3_rs);
    //show time
    time = Ds1302GetTime();
    HtlcdTime(*(time+2),*(time+1),*(time));//显示时间
    //显示总里程
    HtlcdSetSisp(flicker[Menu_Data.area2+4][0],flicker[Menu_Data.area2+4][1],1);
    HtlcdSetTotalMileage(0,0,0,0,0,0,0);
    //显示速度栏目
    HtlcdSetSisp(flicker[Menu_Data.area2][0],flicker[Menu_Data.area2][1],1);
    //HtlcdSetSpeed(0,0,0);
    //刷新
    HtlcdRefresh();
}

void MenuRefreshTime(void) {
    u8 *time;
    time = Ds1302GetTime();
    HtlcdTime(*(time+2),*(time+1),*(time));//显示时间
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
static u8 flickerbit = 0;

void MenuSetFeatures(u8 com) {
    if(com == 0) {
        flickerbit = 1;
        //HtlcdSetSisp(flicker[Menu_Data.area3][0],flicker[Menu_Data.area3][1],0);
        /*
        if(Menu_Data.area3 == 3) {
            Menu_Data.area = 0; 
        } else {
            Menu_Data.area = Menu_Data.area3+1; 
        }*/
        Menu_Data.area = Menu_Data.area3;
        flickercount = 0;
        menu_flag = 1;//set flag
    } else if(com == 1) {
        HtlcdSetSisp(flicker[Menu_Data.area][0],flicker[Menu_Data.area][1],1);
        /*
        if(Menu_Data.area2 == 2) {
            Menu_Data.area = 4;
        } else {
            Menu_Data.area = Menu_Data.area2+5;
        }*/
        Menu_Data.area = Menu_Data.area2+4;
        flickercount = 0;
        menu_flag = 2;//set flag
    } else if(com == 2) {
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
            /*
            if(Menu_Data.area < 4) {
                if(Menu_Data.area3 < 3) {
                    HtlcdSetSisp(flicker[Menu_Data.area-1][0],flicker[Menu_Data.area-1][1],0);
                    Menu_Data.area3++;
                } else {
                    Menu_Data.area3 = 0;
                    HtlcdSetSisp(flicker[3][0],flicker[3][1],0);
                }
            } else if(Menu_Data.area < 7) {
                if(Menu_Data.area2 < 2) {
                    HtlcdSetSisp(flicker[Menu_Data.area-1][0],flicker[Menu_Data.area-1][1],0);
                    Menu_Data.area2++;
                } else {
                    Menu_Data.area2 = 0;
                    HtlcdSetSisp(flicker[6][0],flicker[6][1],0);
                }
            }*/
        } else {
            flickerbit = 0;
        }
    } else if(com == 4) {
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
            /*
            if(Menu_Data.area4_rear == front) {
                Menu_Data.area4_rear = behind;
                HtlcdSetSisp(6,2,0);//------------
                HtlcdSetSisp(4,0,1);//------------
            } else {
                Menu_Data.area4_rear = front;
                HtlcdSetSisp(6,2,1);//------------
                HtlcdSetSisp(4,0,0);//------------
            }*/
            MenuSetRead(&Menu_Data.area4_rear);
        } else {
            //换挡
            ComSendCmdWatch(behind,sub_exchange,0x00,0x00);
        }
    } else if(com == 5) {
        if(menu_flag >= 3) {
            flickerbit = 0;//停止闪烁
            HtlcdSetSisp(flicker[Menu_Data.area][0],flicker[Menu_Data.area][1],1);
            //完成功能
            if(menu_flag == 5) {
                if(Menu_Data.area4 == 2) {
                    menu_flag = 7;
                    /*
                    if(Menu_Data.area4_rear == front) {
                        Menu_Data.area4_rear = behind;
                        HtlcdSetSisp(6,2,0);//------------
                    } else {
                        Menu_Data.area4_rear = front;
                        HtlcdSetSisp(4,0,0);
                    }*/
                    MenuSetRead(&Menu_Data.area4_rear);
                } else {
                    menu_flag = 0;
                }
            } else if(menu_flag == 7) {
                //步进加
                //ComSendCmdWatch(Menu_Data.area4_rear,add_setp,0x00,0x00);
                MenuSetReadOk(Menu_Data.area4_rear,add_setp);
            } else {
                menu_flag = 0;
            }
        } else {
            //换挡加
            ComSendCmdWatch(front,add_stal,0x00,0x00);
        }
    } else if(com == 6) {
        HtlcdSetSisp(flicker[Menu_Data.area][0],flicker[Menu_Data.area][1],1);
        Menu_Data.area = Menu_Data.area4+9;
        flickercount = 0;
        menu_flag = 6;//set flag
    } else if(com == 7) {
        if(menu_flag >= 3) {
            flickerbit = 0;//停止闪烁
            HtlcdSetSisp(flicker[Menu_Data.area][0],flicker[Menu_Data.area][1],1);
            //完成功能
            if(menu_flag == 5) {
                if(Menu_Data.area4 == 2) {
                    menu_flag = 7;
                    /*
                    if(Menu_Data.area4_rear == front) {
                        Menu_Data.area4_rear = behind;
                        HtlcdSetSisp(6,2,0);//------------
                    } else {
                        Menu_Data.area4_rear = front;
                        HtlcdSetSisp(4,0,0);
                    }*/
                    MenuSetRead(&Menu_Data.area4_rear);
                } else {
                    menu_flag = 0;
                }
            } else if(menu_flag == 7) {
                //步进加
                //ComSendCmdWatch(Menu_Data.area4_rear,sub_setp,0x00,0x00);
                MenuSetReadOk(Menu_Data.area4_rear,sub_setp);
            } else {
                menu_flag = 0;
            }
        } else {
            //换挡加
            ComSendCmdWatch(front,sub_stal,0x00,0x00);
        }
    }
}

//闪烁 功能设置
void MenuFlickerServerTime(void) {
    static u8 bit = 0;
    if(flickercount < 50000) {
        flickercount++;
    } else {
        flickercount = 0;
        if(flickerbit == 1) {
            if(bit == 0) {
                bit = 1;
                HtlcdSetSisp(flicker[Menu_Data.area][0],flicker[Menu_Data.area][1],0);
            } else {
                HtlcdSetSisp(flicker[Menu_Data.area][0],flicker[Menu_Data.area][1],1);
                bit = 0;
            }
        }
        HtlcdRefresh();
    }
}
//设置电量 电池：(15,2) (15,3) (15,4) (15,7) (15,6) (15,5)
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
        Menu_Data.area3_rs = num;
        EepromWrite(11,Menu_Data.area3_rs);
        HtlcdSetStalls(3,Menu_Data.area3_rs);
    }
}