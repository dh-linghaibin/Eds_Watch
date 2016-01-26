
#include "Botton.h"

#define BEHIND1 PB_IDR_IDR7
#define BEHIND2 PA_IDR_IDR3

void BottonInit(void) {
    PB_DDR_DDR7 = 0;
    PB_CR1_C17 = 1;
    PB_CR2_C27 = 0;
    
    PA_DDR_DDR3 = 0;
    PA_CR1_C13 = 1;
    PA_CR2_C23 = 0;
    
    PB_DDR_DDR3 = 0;
    PB_CR1_C13 = 1;
    PB_CR2_C23 = 0;
    
    PB_DDR_DDR6 = 0;
    PB_CR1_C16 = 1;
    PB_CR2_C26 = 0;
}


u8 BottonReadBehind(void) {
    static u16 botton1_count = 0;
    static u16 botton2_count = 0;
    if(BEHIND1 == 0) {
        if(botton1_count < 5000) {
            botton1_count++;
        } else {
            if(BEHIND2 == 0) {
                if(botton1_count < 10000) {
                    botton1_count++;
                } else {
                    botton1_count = 20000;
                }
            } else {
                if(botton1_count < 20000) {
                    botton1_count = 5000;
                }
            }
        }
    } else {
        if(botton1_count > 15000) {
            botton1_count = 0;
            return 0x80;
        }
        botton1_count = 0;
        if(BEHIND2 == 0) {
            if(botton2_count < 5000) {
                botton2_count++;
            } else {
                botton2_count = 10000;
            }
        } else {
            if(botton2_count == 10000) {
                botton2_count = 0;
                return 0x81;
            }
            botton2_count = 0;
        }
    }
    return 0x00;
}


u8 BottonReadRear(void) {
    
}

