
#include "Timer.h"

//0x52E0

void TimerInit(void) {
    /*
    TIM4_IER = 0x00;//       
    TIM4_EGR = 0x01;// 
    TIM4_PSCR = 0x07;// 计数器时钟=主时钟/128=16MHZ/128
    TIM4_ARR = 0xFA;// 设定重装载时的寄存器值，255是最大值
    //TIM4_CNTR = 0x00;// 设定计数器的初值
    // 定时周期=(ARR+1)*64=16320uS
    TIM4_IER = 0x01;//   
    TIM4_CR1 = 0x01;  */
    CLK_PCKENR1 |= 0x04;
    
    TIM4_IER = 0x01;
    TIM4_PSCR = 0x07;
    TIM4_ARR = 0x7c; 
    TIM4_CR1 = 0x01;
    /*
    TIM4_IER = 0x00;//       
    TIM4_EGR = 0x01;// 
    TIM4_PSCR = 0x07;// 计数器时钟=主时钟/128=16MHZ/128
    TIM4_ARR = 0xFA;// 设定重装载时的寄存器值，255是最大值
    //TIM4_CNTR = 0x00;// 设定计数器的初值
    // 定时周期=(ARR+1)*64=16320uS
    TIM4_IER = 0x01;//   
    TIM4_CR1 = 0x01;  */
    
}

static u8 sec_flag = 0;

u8 TimerGetSec(void) {
    return sec_flag;
}

void TimerSetSec(u8 data) {
    sec_flag = data;
}

static u8 time_flag = 0;

u8 TimerGetTimeFlag(void) {
    return time_flag;
}

void TimerSetTimeFlag(u8 data) {
    time_flag = data;
}

#pragma vector=0x1b
__interrupt void TIM4_UPD_OVF_IRQHandler(void)
{
    static u16 count_time = 0;
    TIM4_SR1 = 0x00;
    
    if(count_time < 500) {
        count_time++;
    } else {
        count_time = 0;
        sec_flag++;
        time_flag++;
    }
    
    return;
}


