

#include "Ds1302.h"
#include "Delay.h"


#define DS13CLK 	PB_ODR_ODR2		    //1302 CLK
#define DS13IO	    PB_DDR_DDR1		    //IO方向0为输入，1为输出
#define DS13O		PB_ODR_ODR1			//out
#define DS13I		PB_IDR_IDR1         //in
#define DS13CS	    PB_ODR_ODR0		    //RST

#define NOP()	asm("nop;")

void Ds1302Init(void) {
    //RST PB0
    PB_DDR |= BIT(0);
    PB_CR1 |= BIT(0); 
    PB_CR2 |= BIT(0);
    //I/O PB1
    PB_DDR &= ~BIT(1);
    PB_CR1 |= BIT(1); 
    PB_CR2 &= ~BIT(1);
    //SCLK PB2
    PB_DDR |= BIT(2);
    PB_CR1 |= BIT(2); 
    PB_CR2 |= BIT(2);
}

/*向DS1302写一个字节*/
static void _wds13byte(u8 _code) {
    u8 i;
    DS13CLK =0;
    DS13IO=1;
    NOP();
    for(i=0;i<8;i++) {
        if(_code&0x01) { 
            DS13O =1;
        } else {
            DS13O =0;
        }
        DelayUs(10);
        DS13CLK =1;
        DelayUs(10);
        DS13CLK =0;
        _code >>=1; 
    }
}
/*从DS1302读一个字节*/
static u8 _rds13byte(void) {
    u8 i,_code=0;
    DS13CLK =0;
    //PB1_CR2=0;//静止中断
    DS13IO=0;//设为输入
    DelayUs(2);
    for(i=0;i<8;i++) {
        _code>>=1;
        DelayUs(10);
        if(DS13I) {
            _code|=0x80;
        } else { 
            _code&=0x7f;
        }
        DS13CLK =1;
        DelayUs(10);
        DS13CLK =0;
    }
    DS13IO=0;//设为输入
    return _code;
}
/*读功能 _code读功能命令   */
static u8 readds1302(u8 _code) {
    DS13CS =0;         /*关闭DS1302*/ 
    DS13CLK =0;
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    DS13CS =1;          /*使能DS1302*/
    NOP();NOP();NOP();NOP();NOP();NOP();
    _wds13byte(_code);   /*读代码*/
    _code=_rds13byte(); /*返回读取数字*/
    DS13CLK =1;
    DS13CS =0;         /*关闭DS1302*/ 
    return _code;
}
/*写功能 fp写的地址，_code写的内容*/
static void writeds1302(u8 fp,u8 _code) {
    DS13CS =0;         /*关闭DS1302*/ 
    DS13CLK =0;
    DS13CS =1;          /*使能DS1302*/
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    _wds13byte(fp);     /*写控制命令*/
    _wds13byte(_code); /*写入数据*/
    DS13CLK=1;
    DS13CS =0;          /*关闭DS1302*/ 
}

static u8 cur_time[3];
u8* Ds1302GetTime(u8 bit) {
	u8 i,t;
    
	u8 add=0x81;
	for(i=0;i<3;i++) {
		t=readds1302(add);	
		cur_time[i]=((t&0x70)>>4)*10+(t&0x0F);//将读出数据转化						 
		add+=2;
	}
    return cur_time;
}
//给1302设置时间
void Ds1302SetTime(u8 set_hour, u8 set_min) {
    u8 tempdata = 0;
    writeds1302(0x8e,0x00); //解除写保护
    writeds1302(0x80,0);//秒给0，第一次启动时钟用,下了这条才能启动1302
    tempdata=set_min/10;
    tempdata<<=4;
    tempdata|=(set_min%10);
    writeds1302(0x82,tempdata);//写之前要对时分做十位个位分离，10位放高3位，个位放低4位。
    tempdata=set_hour/10;
    tempdata<<=4;
    tempdata|=(set_hour%10);
    writeds1302(0x84,tempdata);
    writeds1302(0x8e,0x80); //置位写保护
}
