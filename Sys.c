
#include "Sys.h"
#include "Htlcd.h"
#include "Led.h"
#include "Botton.h"
#include "Delay.h"

void SysInit(void) {
    CLK_CKDIVR=0x00;//时钟预分频，默认8分配，0x18.16M-0x00；8M-0x08;4M-0x10;
}

void SysSleep(void) {
    HtlcdCloseBacklight();
    LedSet(1);
    //BottonOpenInt();
    //DelayMs(20);
    //MCUSLEEP
}

void SysOpen(void) {
    HtlcdOpenBacklight();
    //LedSet(0);
    //BottonClooseInt();
}
