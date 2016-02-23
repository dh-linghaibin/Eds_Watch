
#include "Botton.h"
#include "Sys.h"

#define BEHIND1 PB_IDR_IDR7 //small best botton
#define BEHIND2 PA_IDR_IDR3
#define BEHIND3 PB_IDR_IDR3

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
    
	EXTI_CR1 &= ~BIT(2);
	EXTI_CR1 &= ~BIT(3);
}

void BottonOpenInt(void) {
    PB_CR2_C27 = 1;
    PA_CR2_C23 = 1;
    PB_CR2_C23 = 1;
    PB_CR2_C26 = 1;
}

void BottonClooseInt(void) {
    PB_CR2_C27 = 0;
    PA_CR2_C23 = 0;
    PB_CR2_C23 = 0;
    PB_CR2_C26 = 0;
}

u8 BottonRead(void) {
    static u16 botton1_count = 0;
    static u16 botton2_count = 0;
    static u16 botton3_count = 0;
    static u16 small_count = 0;
    static u16 mode_count = 0;
    if(BEHIND1 == 0) {
        if(botton1_count < 5000) {
            botton1_count++;
        }
    } else {
        botton1_count = 0;
    }
    
    if(BEHIND2 == 0) {
        if(botton2_count < 5000) {
            botton2_count++;
        }
    } else {
        if(mode_count == 0) {
            if( (botton1_count == 5000) && (botton2_count == 5000) ) {
                botton1_count = 0;
                botton2_count = 0;
                return 0x01;
            } else if(botton2_count == 5000) {
                botton2_count = 0;
                return 0x02;
            }
        }
        botton2_count = 0;
    } 
    
    if(BEHIND3 == 0) { 
        if(botton3_count < 5000) {
            botton3_count++;
        }
    } else {
        if(mode_count == 0) {
            if(botton3_count == 5000) {
                botton3_count = 0;
                return 0x03;
            }
        }
        botton3_count = 0;
    }
    
    if( (botton2_count > 4000) && (botton3_count > 4000) ) {
        if(mode_count < 12000) {
            if(small_count < 60) {
                small_count++;
            } else {
                small_count = 0;
                mode_count++;
                if(mode_count == 1000) {
                    return 0x11;
                } else if(mode_count == 7000) {
                    return 0x12;
                } else if(mode_count == 11000) {
                    return 0x13;
                }
            }
        }
    } else {
        if( (botton2_count == 0) && (botton3_count == 0) ) {
            if(mode_count > 100) {
                mode_count = 0;
                return 0x14;
            }
            mode_count = 0;
        }
    }
    return 0x00;
}


#pragma vector=6
__interrupt void EXTI_PORTB_IRQHandler(void)
{
    SysOpen();
}
