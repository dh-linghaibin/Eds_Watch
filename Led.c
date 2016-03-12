
#include "Led.h"

#define LED 	PA_ODR_ODR4

void LedInit(void) {
    PA_DDR_DDR4 = 1;
    PA_CR1_C14 = 1;
    PA_CR2_C24 = 0;
}

void LedSet(u8 cmd) {
    LED = cmd;
}

