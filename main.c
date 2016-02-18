
#include "Sys.h"
#include "Com.h"
#include "Htlcd.h"
#include "Timer.h"
#include "Botton.h"
#include "Led.h"
#include "Delay.h"
#include "Ds1302.h"
#include "Menu.h"

int main( void ) {
    SysInit();
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
        botton_bit = BottonReadBehind();
        if(botton_bit == 0x80) {
            TimerSetSec(0);
            ComSendCmdWatch(front,add_setp,0x00,0x00);
        } else if(botton_bit == 0x81){
            TimerSetSec(0);
            ComSendCmdWatch(front,sub_setp,0x00,0x00);
        }
        
        botton_bit = BottonReadRear();
        if(botton_bit == 0x80) {
            TimerSetSec(0);
            ComSendCmdWatch(behind,sub_exchange,0x00,0x00);
        }
        
        if(ComGetFlag() == 0x80) {
            ComClearFlag();
            LedSet(0);
            switch(ComGetData(0)) {
                case front:
                    switch(ComGetData(1)) {
                        case dce_gear:MenuSetStall(ComGetData(2),0);break;
                    }
                break;
                case behind: 
                    switch(ComGetData(1)) {
                        case dce_gear:MenuSetStall(0,ComGetData(2));break;
                    }
                break;
            }
        }
    }
}



