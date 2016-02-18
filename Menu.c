

#include "Menu.h"
#include "Ds1302.h"
#include "Htlcd.h"

void MenuInit(void) {
    u8 *time;
    //show time
    time = Ds1302GetTime();
    HtlcdTime(*(time+2),*(time+1),*(time));
    HtlcdRegion1(0,0);
    HtlcdRegion2(0,0);
    //HtlcdRegion3(0,1,2,3,4);
    HtlcdRefresh();
}

void MenuSetStall(u8 num1, u8 num2) {
    HtlcdRegion3(num1,0,0,0,num2);
    HtlcdRefresh();
}

//static u16 num_bit = 0;
//static u16 num_data[20][2];

void MenuSetTwinkle(u8 x, u8 y) {
  //  num_data[num_bit][0] = x;
   // num_data[num_bit][1] = y;
   // num_bit++;
}


