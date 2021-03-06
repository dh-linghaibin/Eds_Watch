

#include "Htlcd.h"
#include "Delay.h"


#define       LCD_ON          0x03    //启动偏压发生器
#define       LCD_OFF         0x02    //关闭偏压发生器
#define       SYS_DIS         0x00    //关闭振荡器和LCD驱动
#define       SYS_EN          0x01    //启动系统振荡器
#define       RC32K           0x18    //启动内部32KRC
#define       TONE_OFF        0x08    //关闭蜂鸣器
#define       TONE_4K         0x40    //启动4 khz蜂鸣信号
#define       TONE_2K         0x60    //启动2 khz蜂鸣信号

#define       COM0        0
#define       COM1        1
#define       COM2        2
#define       COM3        3
#define       COM4        4
#define       COM5        5
#define       COM6        6
#define       COM7        7

#define       DATA         PC_ODR_ODR5      //数据
#define       WR           PC_ODR_ODR6       //写
#define       CS           PE_ODR_ODR6          //片选
#define       CS_2         PE_ODR_ODR6          //片选

static u8 dispram[32];    //预设显示缓存，8x32

//dat 的高cnt 位写入HT1621，先发送高位
static void SendBit_HL(u8 dat,u8 cnt)  {
	u8 i;
	for(i=0; i<cnt;i++) {
		if(dat&0x80) {
			DATA = 1;
		} else {
			DATA = 0;
		}
		WR = 0;
		DelayUs(20);
		WR = 1;
		dat <<= 1;
	}
}

//data的低cnt位写入1621
static void SendBit_LH(u8 dat,u8 cnt) {
	u8 i;
	for(i=0; i<cnt;i++) {
		if(dat&0x01) {
			DATA = 1;
		} else {
			DATA = 0;
		}
		WR = 0;
		DelayUs(20);
		WR = 1;
		dat >>= 1;
	}
}
//指令模式
static void Sendcmd(u8 command)
{
	CS = 0; 
	//写入标志码"100"
	SendBit_HL(0x80,3); 
	//写入 9 位数据,其中前 8 位为 command 命令,最后 1 位任意
	SendBit_HL(command,9);  
	CS = 1;
}

static void Sendcmd_2(u8 command)
{
	CS_2 = 0;
	//写入标志码"100"
	SendBit_HL(0x80,3);  
	//写入 9 位数据,其中前 8 位为 command 命令,最后 1 位任意
	SendBit_HL(command,9);  
	CS_2 = 1;
}

//连续写单元 SEG(0~31)
static void W1622_success(u8 SEG,u8 cnt,u8 *p)
{
	u8 i,addr;
	addr = SEG * 2;
	CS = 0;
	//写入标志码"101"
	SendBit_HL(0xa0,3);  
	//写入 6 位 addr
	SendBit_HL(addr<<2,6); 
	for(i=0; i<cnt;i++)
	{
		SendBit_LH(*p,4);
		SendBit_LH(*p>>4,4);
		p++;
	}
	CS = 1;
}

//显示更新,所有8x32段全部刷新
void HtlcdRefresh(void)   
{
	W1622_success(0,32,dispram);
}

//com(0-7)  SEG(0-31)
void HT1622_dispON(u8 com,u8 SEG)
{
    dispram[SEG] |= (1<<com);
	//去掉注释后，写完就马上显示
   // W1622_byte(SEG,dispram[SEG]);     
}

//com(0-7)  SEG(0-31)
void HT1622_dispOFF(u8 com,u8 SEG)
{
    dispram[SEG] &= ~(1<<com);
	//去掉注释后，写完就马上显示
   // W1622_byte(SEG,dispram[SEG]);    
}

void HtlcdSetSisp(u8 com, u8 seg, u8 com_bit) {
    if(com_bit == 0) {
        HT1622_dispOFF(com,seg);
    } else {
        HT1622_dispON(com,seg);
    }
}

void HtlcdClean(void)
{
    u8  i;
	//清屏幕
    CS = 0;     
	//写入标志码"101"
    SendBit_HL(0xa0,3);  
	//写入 6 位 addr
    SendBit_HL(0,6); 
	//连续写入数据
    for(i =0; i <32; i ++) 
    {
        SendBit_LH(0,4);
        SendBit_LH(0,4);
    }
    CS = 1;
    for(i=0; i<32; i++)
    {
		//清空显存
        dispram[i] = 0x00;    
    }
}

void HtlcdDisAll(void)
{
    u8  i;
	//全显示屏幕
    CS = 0;      
	//写入标志码"101"
    SendBit_HL(0xa0,3);  
	//写入 6 位 addr
    SendBit_HL(0,6); 
	//连续写入数据
    for(i =0; i <32; i ++) 
    {
        SendBit_LH(0xff,4);
        SendBit_LH(0xff,4);
    }
    CS = 1;
    for(i=0; i<32; i++)
    {
		//填充显存
			dispram[i] = 0xff;    
    }

}


void HtcldSetAdr(u8 addr, u8 *num) {
    u8 set_i = 0;
    for( ; set_i < 7; set_i++) {
        HtlcdSetSisp(set_i,addr,num[set_i]);
    }
}

void HtcldSetAdr2(u8 addr, u8 *num) {
    u8 set_i = 0;
    for( ; set_i < 7; set_i++) {
        HtlcdSetSisp(set_i+1,addr,num[set_i]);
    }
}

void HtlcdAdrNum2(u8 addr,u8 data)//上面一行
{
    u8 num[10][7] = {
        1,1,1,0,1,1,1,
        0,1,0,0,1,0,0,
        1,1,0,1,0,1,1,
        1,1,0,1,1,0,1,
        0,1,1,1,1,0,0,
        1,0,1,1,1,0,1,
        1,0,1,1,1,1,1,
        1,1,0,0,1,0,0,
        1,1,1,1,1,1,1,
        1,1,1,1,1,0,1,
    };
    HtcldSetAdr2(addr,num[data]);
}

void HtlcdAdrNum(u8 addr,u8 data)//上面一行
{
    u8 num[10][7] = {
        1,1,1,0,1,1,1,
        0,1,0,0,1,0,0,
        1,1,0,1,0,1,1,
        1,1,0,1,1,0,1,
        0,1,1,1,1,0,0,
        1,0,1,1,1,0,1,
        1,0,1,1,1,1,1,
        1,1,0,0,1,0,0,
        1,1,1,1,1,1,1,
        1,1,1,1,1,0,1,
    };
    HtcldSetAdr(addr,num[data]);
}

void HtlcdAdrTime(u8 addr,u8 data)//显示时间的小位置
{
	switch(data)
	{
		case 0:
				HT1622_dispON(7,addr);HT1622_dispON(6,addr);
				HT1622_dispON(5,addr);HT1622_dispOFF(4,addr);
				HT1622_dispON(3,addr);HT1622_dispON(2,addr);
				HT1622_dispON(1,addr);
				break;
		case 1:
				HT1622_dispOFF(7,addr);HT1622_dispON(6,addr);
				HT1622_dispOFF(5,addr);HT1622_dispOFF(4,addr);
				HT1622_dispON(3,addr);HT1622_dispOFF(2,addr);
				HT1622_dispOFF(1,addr);
				break;
		case 2:
				HT1622_dispON(7,addr);HT1622_dispON(6,addr);
				HT1622_dispOFF(5,addr);HT1622_dispON(4,addr);
				HT1622_dispOFF(3,addr);HT1622_dispON(2,addr);
				HT1622_dispON(1,addr);
				break;
		case 3:
				HT1622_dispON(7,addr);HT1622_dispON(6,addr);
				HT1622_dispOFF(5,addr);HT1622_dispON(4,addr);
				HT1622_dispON(3,addr);HT1622_dispOFF(2,addr);
				HT1622_dispON(1,addr);
				break;
		case 4:
				HT1622_dispOFF(7,addr);HT1622_dispON(6,addr);
				HT1622_dispON(5,addr);HT1622_dispON(4,addr);
				HT1622_dispON(3,addr);HT1622_dispOFF(2,addr);
				HT1622_dispOFF(1,addr);
				break;
		case 5:
				HT1622_dispON(7,addr);HT1622_dispOFF(6,addr);
				HT1622_dispON(5,addr);HT1622_dispON(4,addr);
				HT1622_dispON(3,addr);HT1622_dispOFF(2,addr);
				HT1622_dispON(1,addr);
				break;
		case 6:
				HT1622_dispON(7,addr);HT1622_dispOFF(6,addr);
				HT1622_dispON(5,addr);HT1622_dispON(4,addr);
				HT1622_dispON(3,addr);HT1622_dispON(2,addr);
				HT1622_dispON(1,addr);
				break;
		case 7:
				HT1622_dispON(7,addr);HT1622_dispON(6,addr);
				HT1622_dispOFF(5,addr);HT1622_dispOFF(4,addr);
				HT1622_dispON(3,addr);HT1622_dispOFF(2,addr);
				HT1622_dispOFF(1,addr);
				break;
		case 8:
				HT1622_dispON(7,addr);HT1622_dispON(6,addr);
				HT1622_dispON(5,addr);HT1622_dispON(4,addr);
				HT1622_dispON(3,addr);HT1622_dispON(2,addr);
				HT1622_dispON(1,addr);
				break;
		case 9:
				HT1622_dispON(7,addr);HT1622_dispON(6,addr);
				HT1622_dispON(5,addr);HT1622_dispON(4,addr);
				HT1622_dispON(3,addr);HT1622_dispOFF(2,addr);
				HT1622_dispON(1,addr);
				break;
        case 10:
				HT1622_dispOFF(7,addr);HT1622_dispOFF(6,addr);
				HT1622_dispOFF(5,addr);HT1622_dispOFF(4,addr);
				HT1622_dispOFF(3,addr);HT1622_dispOFF(2,addr);
				HT1622_dispOFF(1,addr);
				break;
	}
}

void HtlcdAdrBigNum(u8 addr,u8 data)//显示速度的位置
{
	switch(data)
	{
		case 0:
				HT1622_dispON(5,addr);
				HT1622_dispON(4,addr+1);
				HT1622_dispON(1,addr+1);
				HT1622_dispON(0,addr+1);
				HT1622_dispON(0,addr);
				HT1622_dispON(3,addr);
				HT1622_dispOFF(2,addr);
				HT1622_dispOFF(3,addr+1);
				HT1622_dispOFF(4,addr);
				HT1622_dispOFF(5,addr+1);
				HT1622_dispOFF(2,addr+1);
				HT1622_dispOFF(1,addr);
				break;
		case 1:
				HT1622_dispOFF(5,addr);
				HT1622_dispON(4,addr+1);
				HT1622_dispON(1,addr+1);
				HT1622_dispOFF(0,addr+1);
				HT1622_dispOFF(0,addr);
				HT1622_dispOFF(3,addr);
				HT1622_dispOFF(2,addr);
				HT1622_dispOFF(3,addr+1);
				HT1622_dispOFF(4,addr);
				HT1622_dispOFF(5,addr+1);
				HT1622_dispOFF(2,addr+1);
				HT1622_dispOFF(1,addr);
				break;
		case 2:
				HT1622_dispON(5,addr);
				HT1622_dispON(4,addr+1);
				HT1622_dispOFF(1,addr+1);
				HT1622_dispON(0,addr+1);
				HT1622_dispON(0,addr);
				HT1622_dispOFF(3,addr);
				HT1622_dispON(2,addr);
				HT1622_dispON(3,addr+1);
				HT1622_dispOFF(4,addr);
				HT1622_dispOFF(5,addr+1);
				HT1622_dispOFF(2,addr+1);
				HT1622_dispOFF(1,addr);
				break;
		case 3:
				HT1622_dispON(5,addr);
				HT1622_dispON(4,addr+1);
				HT1622_dispON(1,addr+1);
				HT1622_dispON(0,addr+1);
				HT1622_dispOFF(0,addr);
				HT1622_dispOFF(3,addr);
				HT1622_dispON(2,addr);
				HT1622_dispON(3,addr+1);
				HT1622_dispOFF(4,addr);
				HT1622_dispOFF(5,addr+1);
				HT1622_dispOFF(2,addr+1);
				HT1622_dispOFF(1,addr);
				break;
		case 4:
				HT1622_dispOFF(5,addr);
				HT1622_dispON(4,addr+1);
				HT1622_dispON(1,addr+1);
				HT1622_dispOFF(0,addr+1);
				HT1622_dispOFF(0,addr);
				HT1622_dispON(3,addr);
				HT1622_dispON(2,addr);
				HT1622_dispON(3,addr+1);
				HT1622_dispOFF(4,addr);
				HT1622_dispOFF(5,addr+1);
				HT1622_dispOFF(2,addr+1);
				HT1622_dispOFF(1,addr);
				break;
		case 5:
				HT1622_dispON(5,addr);
				HT1622_dispOFF(4,addr+1);
				HT1622_dispON(1,addr+1);
				HT1622_dispON(0,addr+1);
				HT1622_dispOFF(0,addr);
				HT1622_dispON(3,addr);
				HT1622_dispON(2,addr);
				HT1622_dispON(3,addr+1);
				HT1622_dispOFF(4,addr);
				HT1622_dispOFF(5,addr+1);
				HT1622_dispOFF(2,addr+1);
				HT1622_dispOFF(1,addr);
				break;
		case 6:
				HT1622_dispON(5,addr);
				HT1622_dispOFF(4,addr+1);
				HT1622_dispON(1,addr+1);
				HT1622_dispON(0,addr+1);
				HT1622_dispON(0,addr);
				HT1622_dispON(3,addr);
				HT1622_dispON(2,addr);
				HT1622_dispON(3,addr+1);
				HT1622_dispOFF(4,addr);
				HT1622_dispOFF(5,addr+1);
				HT1622_dispOFF(2,addr+1);
				HT1622_dispOFF(1,addr);
				break;
		case 7:
				HT1622_dispON(5,addr);
				HT1622_dispON(4,addr+1);
				HT1622_dispON(1,addr+1);
				HT1622_dispOFF(0,addr+1);
				HT1622_dispOFF(0,addr);
				HT1622_dispOFF(3,addr);
				HT1622_dispOFF(2,addr);
				HT1622_dispOFF(3,addr+1);
				HT1622_dispOFF(4,addr);
				HT1622_dispOFF(5,addr+1);
				HT1622_dispOFF(2,addr+1);
				HT1622_dispOFF(1,addr);
				break;
		case 8:
				HT1622_dispON(5,addr);
				HT1622_dispON(4,addr+1);
				HT1622_dispON(1,addr+1);
				HT1622_dispON(0,addr+1);
				HT1622_dispON(0,addr);
				HT1622_dispON(3,addr);
				HT1622_dispON(2,addr);
				HT1622_dispON(3,addr+1);
				HT1622_dispOFF(4,addr);
				HT1622_dispOFF(5,addr+1);
				HT1622_dispOFF(2,addr+1);
				HT1622_dispOFF(1,addr);
				break;
		case 9:
				HT1622_dispON(5,addr);
				HT1622_dispON(4,addr+1);
				HT1622_dispON(1,addr+1);
				HT1622_dispON(0,addr+1);
				HT1622_dispOFF(0,addr);
				HT1622_dispON(3,addr);
				HT1622_dispON(2,addr);
				HT1622_dispON(3,addr+1);
				HT1622_dispOFF(4,addr);
				HT1622_dispOFF(5,addr+1);
				HT1622_dispOFF(2,addr+1);
				HT1622_dispOFF(1,addr);
				break;
	}
}

void HtlcdAdrDoubleNum(u8 addr,u8 data)//写好了 最后一行
{
	switch(data)
	{
		case 0:
				HT1622_dispON(0,addr);//a
				HT1622_dispON(3,addr);//b
				HT1622_dispON(6,addr);//c
				HT1622_dispON(7,addr);//d
				HT1622_dispOFF(7,addr-1);//e
				HT1622_dispOFF(4,addr-1);//f
				HT1622_dispOFF(2,addr-1);//g
				HT1622_dispON(2,addr);//h
				HT1622_dispOFF(1,addr-1);//i
				HT1622_dispOFF(1,addr);//j
				HT1622_dispOFF(3,addr-1);//k
				HT1622_dispOFF(4,addr);//l
                HT1622_dispOFF(6,addr-1);//m
                HT1622_dispOFF(5,addr);//n
                HT1622_dispOFF(0,addr-1);//o
                HT1622_dispON(5,addr-1);//p
				break;
		case 1:
				HT1622_dispOFF(0,addr);//a
				HT1622_dispON(3,addr);//b
				HT1622_dispON(6,addr);//c
				HT1622_dispOFF(7,addr);//d
				HT1622_dispOFF(7,addr-1);//e
				HT1622_dispOFF(4,addr-1);//f
				HT1622_dispOFF(2,addr-1);//g
				HT1622_dispOFF(2,addr);//h
				HT1622_dispOFF(1,addr-1);//i
				HT1622_dispOFF(1,addr);//j
				HT1622_dispOFF(3,addr-1);//k
				HT1622_dispOFF(4,addr);//l
                HT1622_dispOFF(6,addr-1);//m
                HT1622_dispOFF(5,addr);//n
                HT1622_dispOFF(0,addr-1);//o
                HT1622_dispOFF(5,addr-1);//p
				break;
		case 2:
				HT1622_dispON(0,addr);//a
				HT1622_dispON(3,addr);//b
				HT1622_dispOFF(6,addr);//c
				HT1622_dispON(7,addr);//d
				HT1622_dispOFF(7,addr-1);//e
				HT1622_dispOFF(4,addr-1);//f
				HT1622_dispOFF(2,addr-1);//g
				HT1622_dispOFF(2,addr);//h
				HT1622_dispOFF(1,addr-1);//i
				HT1622_dispOFF(1,addr);//j
				HT1622_dispOFF(3,addr-1);//k
				HT1622_dispON(4,addr);//l
                HT1622_dispOFF(6,addr-1);//m
                HT1622_dispOFF(5,addr);//n
                HT1622_dispOFF(0,addr-1);//o
                HT1622_dispON(5,addr-1);//p
				break;
		case 3:
				HT1622_dispON(0,addr);//a
				HT1622_dispON(3,addr);//b
				HT1622_dispON(6,addr);//c
				HT1622_dispON(7,addr);//d
				HT1622_dispOFF(7,addr-1);//e
				HT1622_dispOFF(4,addr-1);//f
				HT1622_dispOFF(2,addr-1);//g
				HT1622_dispOFF(2,addr);//h
				HT1622_dispOFF(1,addr-1);//i
				HT1622_dispOFF(1,addr);//j
				HT1622_dispOFF(3,addr-1);//k
				HT1622_dispON(4,addr);//l
                HT1622_dispOFF(6,addr-1);//m
                HT1622_dispOFF(5,addr);//n
                HT1622_dispOFF(0,addr-1);//o
                HT1622_dispOFF(5,addr-1);//p
				break;
		case 4:
				HT1622_dispOFF(0,addr);//a
				HT1622_dispON(3,addr);//b
				HT1622_dispON(6,addr);//c
				HT1622_dispOFF(7,addr);//d
				HT1622_dispOFF(7,addr-1);//e
				HT1622_dispOFF(4,addr-1);//f
				HT1622_dispOFF(2,addr-1);//g
				HT1622_dispON(2,addr);//h
				HT1622_dispOFF(1,addr-1);//i
				HT1622_dispOFF(1,addr);//j
				HT1622_dispOFF(3,addr-1);//k
				HT1622_dispON(4,addr);//l
                HT1622_dispOFF(6,addr-1);//m
                HT1622_dispOFF(5,addr);//n
                HT1622_dispOFF(0,addr-1);//o
                HT1622_dispOFF(5,addr-1);//p
				break;
		case 5:
				HT1622_dispON(0,addr);//a
				HT1622_dispOFF(3,addr);//b
				HT1622_dispON(6,addr);//c
				HT1622_dispON(7,addr);//d
				HT1622_dispOFF(7,addr-1);//e
				HT1622_dispOFF(4,addr-1);//f
				HT1622_dispOFF(2,addr-1);//g
				HT1622_dispON(2,addr);//h
				HT1622_dispOFF(1,addr-1);//i
				HT1622_dispOFF(1,addr);//j
				HT1622_dispOFF(3,addr-1);//k
				HT1622_dispON(4,addr);//l
                HT1622_dispOFF(6,addr-1);//m
                HT1622_dispOFF(5,addr);//n
                HT1622_dispOFF(0,addr-1);//o
                HT1622_dispOFF(5,addr-1);//p
				break;
		case 6:
				HT1622_dispON(0,addr);//a
				HT1622_dispOFF(3,addr);//b
				HT1622_dispON(6,addr);//c
				HT1622_dispON(7,addr);//d
				HT1622_dispOFF(7,addr-1);//e
				HT1622_dispOFF(4,addr-1);//f
				HT1622_dispOFF(2,addr-1);//g
				HT1622_dispON(2,addr);//h
				HT1622_dispOFF(1,addr-1);//i
				HT1622_dispOFF(1,addr);//j
				HT1622_dispOFF(3,addr-1);//k
				HT1622_dispON(4,addr);//l
                HT1622_dispOFF(6,addr-1);//m
                HT1622_dispOFF(5,addr);//n
                HT1622_dispOFF(0,addr-1);//o
                HT1622_dispON(5,addr-1);//p
				break;
		case 7:
				HT1622_dispON(0,addr);//a
				HT1622_dispON(3,addr);//b
				HT1622_dispON(6,addr);//c
				HT1622_dispOFF(7,addr);//d
				HT1622_dispOFF(7,addr-1);//e
				HT1622_dispOFF(4,addr-1);//f
				HT1622_dispOFF(2,addr-1);//g
				HT1622_dispOFF(2,addr);//h
				HT1622_dispOFF(1,addr-1);//i
				HT1622_dispOFF(1,addr);//j
				HT1622_dispOFF(3,addr-1);//k
				HT1622_dispOFF(4,addr);//l
                HT1622_dispOFF(6,addr-1);//m
                HT1622_dispOFF(5,addr);//n
                HT1622_dispOFF(0,addr-1);//o
                HT1622_dispOFF(5,addr-1);//p
				break;
		case 8:
				HT1622_dispON(0,addr);//a
				HT1622_dispON(3,addr);//b
				HT1622_dispON(6,addr);//c
				HT1622_dispON(7,addr);//d
				HT1622_dispOFF(7,addr-1);//e
				HT1622_dispOFF(4,addr-1);//f
				HT1622_dispOFF(2,addr-1);//g
				HT1622_dispON(2,addr);//h
				HT1622_dispOFF(1,addr-1);//i
				HT1622_dispOFF(1,addr);//j
				HT1622_dispOFF(3,addr-1);//k
				HT1622_dispON(4,addr);//l
                HT1622_dispOFF(6,addr-1);//m
                HT1622_dispOFF(5,addr);//n
                HT1622_dispOFF(0,addr-1);//o
                HT1622_dispON(5,addr-1);//p
				break;
		case 9:
				HT1622_dispON(0,addr);//a
				HT1622_dispON(3,addr);//b
				HT1622_dispON(6,addr);//c
				HT1622_dispON(7,addr);//d
				HT1622_dispOFF(7,addr-1);//e
				HT1622_dispOFF(4,addr-1);//f
				HT1622_dispOFF(2,addr-1);//g
				HT1622_dispON(2,addr);//h
				HT1622_dispOFF(1,addr-1);//i
				HT1622_dispOFF(1,addr);//j
				HT1622_dispOFF(3,addr-1);//k
				HT1622_dispON(4,addr);//l
                HT1622_dispOFF(6,addr-1);//m
                HT1622_dispOFF(5,addr);//n
                HT1622_dispOFF(0,addr-1);//o
                HT1622_dispOFF(5,addr-1);//p
				break;
        case 10:
				HT1622_dispON(0,addr);//a
				HT1622_dispON(3,addr);//b
				HT1622_dispON(6,addr);//c
				HT1622_dispON(7,addr);//d
				HT1622_dispOFF(7,addr-1);//e
				HT1622_dispON(4,addr-1);//f
				HT1622_dispON(2,addr-1);//g
				HT1622_dispON(2,addr);//h
				HT1622_dispOFF(1,addr-1);//i
				HT1622_dispOFF(1,addr);//j
				HT1622_dispOFF(3,addr-1);//k
				HT1622_dispOFF(4,addr);//l
                HT1622_dispOFF(6,addr-1);//m
                HT1622_dispOFF(5,addr);//n
                HT1622_dispOFF(0,addr-1);//o
                HT1622_dispON(5,addr-1);//p
				break;
        case 11:
				HT1622_dispOFF(0,addr);//a
				HT1622_dispON(3,addr);//b
				HT1622_dispON(6,addr);//c
				HT1622_dispOFF(7,addr);//d
				HT1622_dispOFF(7,addr-1);//e
				HT1622_dispOFF(4,addr-1);//f
				HT1622_dispOFF(2,addr-1);//g
				HT1622_dispON(2,addr);//h
				HT1622_dispOFF(1,addr-1);//i
				HT1622_dispOFF(1,addr);//j
				HT1622_dispOFF(3,addr-1);//k
				HT1622_dispOFF(4,addr);//l
                HT1622_dispOFF(6,addr-1);//m
                HT1622_dispOFF(5,addr);//n
                HT1622_dispOFF(0,addr-1);//o
                HT1622_dispON(5,addr-1);//p
				break;
        case 12:
				HT1622_dispOFF(0,addr);//a
				HT1622_dispOFF(3,addr);//b
				HT1622_dispOFF(6,addr);//c
				HT1622_dispOFF(7,addr);//d
				HT1622_dispOFF(7,addr-1);//e
				HT1622_dispOFF(4,addr-1);//f
				HT1622_dispOFF(2,addr-1);//g
				HT1622_dispOFF(2,addr);//h
				HT1622_dispOFF(1,addr-1);//i
				HT1622_dispOFF(1,addr);//j
				HT1622_dispON(3,addr-1);//k
				HT1622_dispON(4,addr);//l
                HT1622_dispOFF(6,addr-1);//m
                HT1622_dispOFF(5,addr);//n
                HT1622_dispOFF(0,addr-1);//o
                HT1622_dispOFF(5,addr-1);//p
				break;
	}
}

void HtlcdTime(u8 hour, u8 min, u8 sec) {//显示时间
	if( (hour/10)==0 ) {
		HT1622_dispOFF(4,12);
		HT1622_dispOFF(5,12);
		HT1622_dispOFF(6,12);
	} else if( (hour/10)==1 ) {
		HT1622_dispOFF(4,12);
		HT1622_dispON(5,12);
		HT1622_dispON(6,12);
	} else if( (hour/10)==2 ) {
		HT1622_dispON(4,12);
		HT1622_dispOFF(5,12);
		HT1622_dispON(6,12);
	}
	HT1622_dispON(0,10);//小数点
	HT1622_dispON(0,9);//小数点
	
	HtlcdAdrTime(11,hour%10);//小时 个位
	
	HtlcdAdrTime(10,min/10);//分钟 十位
	HtlcdAdrTime(9,min%10);//分钟 个位
	
	HtlcdAdrTime(8,sec/10);//秒 十位
	HtlcdAdrTime(7,sec%10);//秒 十位
}

void HtlcdTimeNull(u8 bit) {//显示时间
    if(bit == 0) {
        HT1622_dispOFF(4,12);
        HT1622_dispOFF(5,12);
        HT1622_dispOFF(6,12);
        HtlcdAdrTime(11,10);//小时 个位
	} else if(bit == 1) {
        HtlcdAdrTime(10,10);//分钟 十位
        HtlcdAdrTime(9,10);//分钟 个位
	} else if(bit ==2) {
        HtlcdAdrTime(8,10);//秒 十位
        HtlcdAdrTime(7,10);//秒 十位
	} else {
	
    }
}

void HtlcdSetTotalMileage(u8 num1, u8 num2, u8 num3, u8 num4, 
                          u8 num5, u8 num6, u8 num7) {
    HtlcdAdrNum(13,num1);
    HtlcdAdrNum(14,num2);
    HtlcdAdrNum(15,num3);
    HtlcdAdrNum(16,num4);
    HtlcdAdrNum(17,num5);
    HtlcdAdrNum(18,num6);
    HtlcdAdrNum(19,num7);
}

/*
void HtlcdRegion2(u8 bit, u16 data) {
    HtlcdAdrBigNum(1,0);
    HtlcdAdrBigNum(3,0);
    HtlcdAdrNum(5,0);
    HT1622_dispON(2,0);//km
    HT1622_dispON(3,0);//h
}*/

void HtlcdSetSpeed(u8 num1, u8 num2, u8 num3) {
    HtlcdAdrBigNum(1,num1);
    HtlcdAdrBigNum(3,num2);
    HtlcdAdrNum2(5,num3);
    HT1622_dispON(2,0);//km
    HT1622_dispON(3,0);//h
}

void HtlcdSetStalls(u8 bit, u8 num) {//最下面一行
    switch(bit) {
        case 0:
            HtlcdAdrDoubleNum(21, num);
        break;
        case 1:
            HtlcdAdrDoubleNum(23, num);
        break;
        case 2:
            HtlcdAdrDoubleNum(25, num);
        break;
        case 3:
            HtlcdAdrDoubleNum(27, num);
        break;
        default:break;
    }
}

#define BACKLIGHT1 PB_ODR_ODR2

void HtlcdInit(void) {
    u8  j = 0;
    //CS PE6
    PE_DDR |= BIT(6);
    PE_CR1 |= BIT(6); 
    PE_CR2 &= ~BIT(6);
    //RD PC7
    PC_DDR |= BIT(7);
    PC_CR1 |= BIT(7); 
    PC_CR2 &= ~BIT(7);
    //WR PC6
    PC_DDR |= BIT(6);
    PC_CR1 |= BIT(6); 
    PC_CR2 &= ~BIT(6);
    //DATA PC5
    PC_DDR |= BIT(5);
    PC_CR1 |= BIT(5); 
    PC_CR2 &= ~BIT(5);
    //背光 PC1 PC2 PC3 
    PB_DDR |= BIT(2);
    PB_CR1 |= BIT(2); 
    PB_CR2 |= BIT(2);
    
    PC_ODR_ODR7 = 1;
    
    for(j=0; j<32; j++) {
        dispram[j] = 0;
    }
    Sendcmd(0x05); 
    //RC32k
    Sendcmd(RC32K);   
    //打开系统振荡器
    Sendcmd(SYS_EN);  
    //打开 LCD偏压发生器
    Sendcmd(LCD_ON);  
    //RC32k
    Sendcmd_2(RC32K);   
    //打开系统振荡器
    Sendcmd_2(SYS_EN);  
    //时钟分屏
    Sendcmd_2(0xa0);  
    //打开 LCD偏压发生器
    Sendcmd_2(LCD_ON);
    //清空
    HtlcdClean();
    
    HT1622_dispON(2,6);//电池
    HT1622_dispON(0,5);
    HT1622_dispON(0,11);

    HT1622_dispON(4,0);//RS
    HT1622_dispON(6,2);//FS
    //HtlcdRegion2(1,1);
    //HtlcdRefresh();
}
void HtlcdOpenBacklight(void) {
    BACKLIGHT1 = 0;
   // Sendcmd(LCD_ON);  
}

void HtlcdCloseBacklight(void) {
    BACKLIGHT1 = 1;
  //  Sendcmd(LCD_OFF);  
}