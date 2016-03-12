

#include "Com.h"
#include "Delay.h"


void ComInit(void) {
    //Watch
    PA_DDR_DDR2 = 0;
    PA_CR1_C12 = 0;
    PA_CR2_C22 = 1;
    
    //外部中断初始化
	EXTI_CR1 &= ~BIT(0);//开启PA口中断
	EXTI_CR1 &= ~BIT(1);
}

#define COM_BIT_OUT 	PA_ODR_ODR2
#define COM_BIT_IN 	    PA_IDR_IDR2
#define COM_BIT_DR      PA_DDR_DDR2
#define COM_BIT_INT     PA_CR2_C22

u8 ComSend(u8 data[]) {
	u16 wait = 0;
	u8 data_t = 0;//保存临时值
	u8 i = 0,j = 0;
	
	COM_BIT_INT = 0;//中断
	COM_BIT_DR = 1;//设置为输出
	COM_BIT_OUT = 0;
	DelayUs(100);//拉低20ms说明总线开始
	COM_BIT_DR = 0;//设置为输入
	DelayUs(1);
	while(COM_BIT_IN == 1) {//等待从机拉高
		if(wait < 100) {
			wait++;
		} else {//超时，退出
		
			COM_BIT_INT = 1;//中断
			return 0;
		}
	}
	wait = 0;
	while(COM_BIT_IN == 0) {
		if(wait < 100) {
			wait++;
		} else {//超时，退出
		
			return 0;
		}
	}
	COM_BIT_DR = 1;//设置为输出
	for(j = 0;j < 5;j++) {
		data_t = data[j];
		for(i=0;i<8;i++) {
			COM_BIT_OUT = 0;
			if(data_t&0x80) {
				DelayUs(80);
			} else {
				DelayUs(40);
			}
			COM_BIT_OUT = 1;
			DelayUs(20);
			data_t<<=1;
		}
	}
	DelayUs(60);
	COM_BIT_OUT = 1;
	COM_BIT_INT = 1;//中断
	COM_BIT_DR = 0;//设置为输入
	return 0;
}

u8 ComRead(u8 data_s[]) {
    static u16 wait = 0;
    u8 i = 0,j = 0;
	u8 data = 0;
	wait = 0;

	COM_BIT_DR = 0;//设置为输入
	while(COM_BIT_IN == 0) {
		if(wait < 60) {
			wait++;
		} else {
			return 0x44;
		}
	}
	if(wait > 25) {
		wait = 0;
		COM_BIT_DR = 1;//设置为输出
		COM_BIT_OUT = 0;
		DelayUs(1);
		COM_BIT_DR = 0;//设置为输入
		//开始接受数据
		for(j = 0;j < 5;j++) {
			for(i=0;i<8;i++) {  
				data<<=1; 
				while(COM_BIT_IN == 1) {
					if(wait < 200) {
						wait++;
					} else {
						return 0x44;
					}
				}
				wait = 0;
				while(COM_BIT_IN == 0) {
					if(wait < 200) {
						wait++;
					} else {
						return 0x44;
					}
				}
				if(wait > 30) {//为1
					data|=0x01;  
				}
				wait = 0;					
			}
			data_s[j] = data;
			data = 0;
		}
		if(data_s[4] == (data_s[0]+data_s[1]+data_s[2]+data_s[3])) {//累加校验
			if(data_s[4] != 0) {
				return 0x88;
			} else {
				return 0x44;
			}
		} else {
			return 0x44;
		}
	} else {//时间不对 推出
		return 0x44;
	}
} 


static u8 com_date[5];
static u8 rs_ok = 0;//whether it has received data

u8 ComGetData(u8 num) {
    return com_date[num];
}

u8 ComGetFlag(void) {
    return rs_ok;
}

void ComClearFlag(void) {
    rs_ok = 0;
}

void ComSendCmdWatch(u8 cmd,u8 par1,u8 par2,u8 par3) {
    u8 com_t_data[5] = {0,0,0,0,0};//前拨
    u8 com_t_data2[5] = {0,0,0,0,0};//前拨
	com_t_data[0] = cmd; //cmd
	com_t_data[1] = par1;
	com_t_data[2] = par2;
	com_t_data[3] = par3;
    com_t_data[4] = com_t_data[0]+com_t_data[1]+com_t_data[2]
                                    +com_t_data[3];
    INTOFF
    ComSend(com_t_data2);
	ComSend(com_t_data);
    INTEN
}

#pragma vector=7
__interrupt void EXTI_PORTA_IRQHandler(void)
{
    INTOFF
    if(ComRead(com_date) == 0x88) {
        rs_ok = 0x80;
    }
    INTEN
}




