
#include "Sys.h"
#include "Com.h"
#include "Htlcd.h"
#include "Timer.h"
#include "Botton.h"
#include "Led.h"
#include "Delay.h"
#include "Ds1302.h"
#include "Menu.h"
#include "Eeprom.h"

int main( void ) {
    SysInit();
    EeepromInit();
    TimerInit();
    ComInit();
    Ds1302Init();
    HtlcdInit();
    BottonInit();
    LedInit();
    MenuInit();
    INTEN
    while(1) {
        u8 botton_bit = 0;
        if(TimerGetSec() > 30) {
            TimerSetSec(0);
           // SysSleep();
        }
        if(TimerGetTimeFlag() > 0x01) {
            TimerSetTimeFlag(0);
            MenuRefreshTime();
        }
        MenuFlickerServerTime();
        botton_bit = BottonRead();
        switch(botton_bit) {
            case 0x01:
                MenuSetFeatures(7);
            break;//后减档
            case 0x02:
                MenuSetFeatures(5);
            break;//后加档
            case 0x03:
                MenuSetFeatures(4);
            break;//前换挡
            case 0x11:
                MenuSetFeatures(0);
            break;//区域3
            case 0x12:
                MenuSetFeatures(6);//1
            break;//区域2
            case 0x13:
                MenuSetFeatures(2);
            break;//区域1
            case 0x14:
                MenuSetFeatures(3);
            break;//放开信号
        }
        
        if(ComGetFlag() == 0x80) {
            ComClearFlag();
            LedSet(0);
            switch(ComGetData(0)) {
                case front:
                    switch(ComGetData(1)) {
                        case dce_gear:
                            MenuSetStalls(front,ComGetData(2)+1);
                            MenuSetBattery(ComGetData(3));
                        break;
                    }
                break;
                case behind: 
                    switch(ComGetData(1)) {
                        case dce_gear:
                            MenuSetStalls(behind,ComGetData(2));
                            MenuSetBattery(ComGetData(3));
                        break;
                    }
                break;
                case dce_powe:
                    MenuSetBattery(ComGetData(1));
                break;
            }
        }
    }
}



