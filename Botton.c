
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

u8 BottonReadBehind(void) {
    static u16 botton1_count = 0;
    static u16 botton2_count = 0;
    static u16 botton3_count = 0;
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
            if(BEHIND3 == 1) {
                if(botton2_count == 10000) {
                    botton2_count = 0;
                    return 0x81;
                }
            }
            botton2_count = 0;
        }
        if(BEHIND3 == 0) {
            if(botton3_count < 5000) {
                botton3_count++;
            } else {
                botton3_count = 0;
            }
        } else {
            botton3_count = 0;
        }
    }
    return 0x00;
}


u8 BottonReadRear(void) {
    static u16 botton1_count = 0;
    if(BEHIND3 == 0) {
        if(botton1_count < 5000) {
            botton1_count++;
        } else {
            botton1_count = 20000;
        }
    } else {
        if(botton1_count > 15000) {
            botton1_count = 0;
            return 0x80;
        }
        botton1_count = 0;
    }
    return 0x00;
}

u8 BottonRead(void) {
    static u16 botton1_count = 0;
    static u16 botton2_count = 0;
    static u16 botton3_count = 0;
    static u16 botton4_count = 0;
    static u16 mode_count = 0;
    if(BEHIND1 == 0) {
        if(botton1_count < 5000) {
            botton1_count++;
        } else { 
            
        }
    } else {
    
    }
    
    if(BEHIND2 == 0) {
        
    } else {
    
    } 
    
    if(BEHIND3 == 0) { 
    
    } else {
        
    }
    
    
    
    return 0x00;
}


#pragma vector=6
__interrupt void EXTI_PORTB_IRQHandler(void)
{
    SysOpen();
}
