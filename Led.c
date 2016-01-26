
#include "Led.h"

#define LED 	PD_ODR_ODR7

void LedInit(void) {
    PD_DDR_DDR7 = 1;
    PD_CR1_C17 = 1;
    PD_CR2_C27 = 0;
}

void LedSet(u8 cmd) {
    LED = cmd;
}

