
#include "Sys.h"
#include "Com.h"
#include "Htlcd.h"
#include "Timer.h"
#include "Botton.h"
#include "Led.h"

int main( void ) {
    SysInit();
    TimerInit();
    ComInit();
    HtlcdInit();
    BottonInit();
    LedInit();
    INTEN
    HT1622_show_setp(4,1);
    HT1622_duan(18,2);
    
    HT1622_duan_big_t(21,0);HtlcdTime(14,33,24);
    while(1) {
        if(TimerGetSec() == 0x80) {
            TimerSetSec(0);
            LedSet(1);
            //ComSendCmdWatch(0x01,0x02,0x03,0x04);
        }
        
        if(BottonReadBehind() == 0x81) {
            ComSendCmdWatch(0x01,0x02,0x03,0x04);
        }
        
        if(ComGetFlag() == 0x80) {
            ComClearFlag();
            LedSet(0);
        }
        
        if(ComGetFlag() == 0x80) {
            ComClearFlag();
            switch(ComGetData(0)) {
                case 0x00:
                break;
            }
        }
    }
}



