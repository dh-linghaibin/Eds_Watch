
#include "Botton.h"
#include "Sys.h"

#define BEHIND1 PB_IDR_IDR3 //small best botton
#define BEHIND2 PB_IDR_IDR4
#define BEHIND3 PB_IDR_IDR6
#define BEHIND4 PB_IDR_IDR5

void BottonInit(void) {
    PB_DDR_DDR7 = 0;
    PB_CR1_C17 = 1;
    PB_CR2_C27 = 0;
   
    PB_DDR_DDR6 = 0;
    PB_CR1_C16 = 1;
    PB_CR2_C26 = 0;
    
    PB_DDR_DDR5 = 0;
    PB_CR1_C15 = 1;
    PB_CR2_C25 = 0;
    
    PB_DDR_DDR4 = 0;
    PB_CR1_C14 = 1;
    PB_CR2_C24 = 0;
    
    PB_DDR_DDR3 = 0;
    PB_CR1_C13 = 1;
    PB_CR2_C23 = 0;
    //·äÃùÆ÷
    PB_DDR_DDR0 = 1;
    PB_CR1_C10 = 1;
    PB_CR2_C20 = 0;
    
    PB_ODR_ODR0 = 0;
    
	EXTI_CR1 &= ~BIT(2);
	EXTI_CR1 &= ~BIT(3);
}

void BottonOpenInt(void) {
    PB_CR2_C27 = 1;
    PB_CR2_C26 = 1;
    PB_CR2_C25 = 1;
    PB_CR2_C24 = 1;
    PB_CR2_C23 = 1;
}

void BottonClooseInt(void) {
    PB_CR2_C27 = 0;
    PB_CR2_C26 = 0;
    PB_CR2_C25 = 0;
    PB_CR2_C24 = 0;
    PB_CR2_C23 = 0;
}

u8 BottonRead(void) {
    static u16 botton1_count = 0;
    static u16 botton2_count = 0;
    static u16 botton3_count = 0;
    static u16 botton4_count = 0;
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
    
    if(BEHIND4 == 0) {
        if(botton4_count < 5000) {
            botton4_count++;
        }
    } else {
        if(botton4_count > 4500) {
            botton4_count = 0;
            return 0x11;
        }
        botton4_count = 0;
    }
    
    if( (botton2_count > 4000) && (botton3_count > 4000) ) {
        if(mode_count < 20000) {
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
                } else if(mode_count == 18000) {
                    return 0x14;
                }
            }
        }
    } else {
        if( (botton2_count == 0) && (botton3_count == 0) ) {
            if(mode_count > 100) {
                mode_count = 0;
                return 0x15;
            }
            mode_count = 0;
        }
    }
    return 0x00;
}


#pragma vector=8
__interrupt void EXTI_PORTB_IRQHandler(void)
{
    SysOpen();
}
