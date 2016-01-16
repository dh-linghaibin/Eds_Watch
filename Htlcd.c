

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
#define       CS           PD_ODR_ODR0          //片选
#define       CS_2         PD_ODR_ODR0          //片选


u8   dispram[32];    //预设显示缓存，8x32

//dat 的高cnt 位写入HT1621，先发送高位
void SendBit_HL(u8 dat,u8 cnt) 
{
	u8 i;
	for(i=0; i<cnt;i++)
	{
		if(dat&0x80)
		{
			DATA = 1;
		}
		else
		{
			DATA = 0;
		}
		WR = 0;
		DelayUs(20);
		WR = 1;
		dat <<= 1;
	}
}

//data的低cnt位写入1621
void SendBit_LH(u8 dat,u8 cnt)    
{
	u8 i;
	for(i=0; i<cnt;i++)
	{
		if(dat&0x01)
		{
			DATA = 1;
		}
		else
		{
			DATA = 0;
		}
		WR = 0;
		DelayUs(20);
		WR = 1;
		dat >>= 1;
	}
}
//指令模式
void Sendcmd(u8 command)
{
	CS = 0; 
	//写入标志码"100"
	SendBit_HL(0x80,3); 
	//写入 9 位数据,其中前 8 位为 command 命令,最后 1 位任意
	SendBit_HL(command,9);  
	CS = 1;
}

//写半个字节
void W1622_q(u8 addr,u8 dat)
{
	CS = 0; 
	//写入标志码"101"
	SendBit_HL(0xa0,3); 
	//写入 6 位 addr
	SendBit_HL(addr<<2,6); 
	SendBit_LH(dat,4);
	CS = 1;
}


void Sendcmd_2(u8 command)
{
	CS_2 = 0;
	//写入标志码"100"
	SendBit_HL(0x80,3);  
	//写入 9 位数据,其中前 8 位为 command 命令,最后 1 位任意
	SendBit_HL(command,9);  
	CS_2 = 1;
}
//写半个字节
void W1622_q_2(u8 addr,u8 dat)
{
	CS_2 = 0;
	//写入标志码"101"
	SendBit_HL(0xa0,3);  
	//写入 6 位 addr
	SendBit_HL(addr<<2,6); 
	SendBit_LH(dat,4);
	CS_2 = 1;
}

//写SEG脚上所有的位，SEG( 0~31 )
void W1622_byte(u8 SEG,u8 dat)
{
	u8 i,addr;
	addr = SEG * 2;
	for(i=0;i<2;i++)
	{
		W1622_q(addr,dat);
		addr += 1;
		dat >>= 4;
	}
}

void W1622_byte_2(u8 SEG,u8 dat)
{
	u8 i,addr;
	addr = SEG * 2;
	for(i=0;i<2;i++)
	{
		W1622_q_2(addr,dat);
		addr += 1;
		dat >>= 4;
	}
}
//连续写单元 SEG(0~31)
void W1622_success(u8 SEG,u8 cnt,u8 *p)
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
void W1622_success_2(u8 SEG,u8 cnt,u8 *p)
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
void HT1622_disp_renew(void)   
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


void HT1622_clean(void)
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

void HT1622_disp_All(void)
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
void HT1622_duan(u8 addr,u8 data)
{
	switch(data)
	{
		case 0:
				HT1622_dispON(0,addr);HT1622_dispON(1,addr);
				HT1622_dispON(2,addr);HT1622_dispOFF(3,addr);
				HT1622_dispON(4,addr);HT1622_dispON(5,addr);
				HT1622_dispON(6,addr);
				break;
		case 1:
				HT1622_dispOFF(0,addr);HT1622_dispON(1,addr);
				HT1622_dispOFF(2,addr);HT1622_dispOFF(3,addr);
				HT1622_dispON(4,addr);HT1622_dispOFF(5,addr);
				HT1622_dispOFF(6,addr);
				break;
		case 2:
				HT1622_dispON(0,addr);HT1622_dispON(1,addr);
				HT1622_dispOFF(2,addr);HT1622_dispON(3,addr);
				HT1622_dispOFF(4,addr);HT1622_dispON(5,addr);
				HT1622_dispON(6,addr);
				break;
		case 3:
				HT1622_dispON(0,addr);HT1622_dispON(1,addr);
				HT1622_dispOFF(2,addr);HT1622_dispON(3,addr);
				HT1622_dispON(4,addr);HT1622_dispOFF(5,addr);
				HT1622_dispON(6,addr);
				break;
		case 4:
				HT1622_dispOFF(0,addr);HT1622_dispON(1,addr);
				HT1622_dispON(2,addr);HT1622_dispON(3,addr);
				HT1622_dispON(4,addr);HT1622_dispOFF(5,addr);
				HT1622_dispOFF(6,addr);
				break;
		case 5:
				HT1622_dispON(0,addr);HT1622_dispOFF(1,addr);
				HT1622_dispON(2,addr);HT1622_dispON(3,addr);
				HT1622_dispON(4,addr);HT1622_dispOFF(5,addr);
				HT1622_dispON(6,addr);
				break;
		case 6:
				HT1622_dispON(0,addr);HT1622_dispOFF(1,addr);
				HT1622_dispON(2,addr);HT1622_dispON(3,addr);
				HT1622_dispON(4,addr);HT1622_dispON(5,addr);
				HT1622_dispON(6,addr);
				break;
		case 7:
				HT1622_dispON(0,addr);HT1622_dispON(1,addr);
				HT1622_dispOFF(2,addr);HT1622_dispOFF(3,addr);
				HT1622_dispON(4,addr);HT1622_dispOFF(5,addr);
				HT1622_dispOFF(6,addr);
				break;
		case 8:
				HT1622_dispON(0,addr);HT1622_dispON(1,addr);
				HT1622_dispON(2,addr);HT1622_dispON(3,addr);
				HT1622_dispON(4,addr);HT1622_dispON(5,addr);
				HT1622_dispON(6,addr);
				break;
		case 9:
				HT1622_dispON(0,addr);HT1622_dispON(1,addr);
				HT1622_dispON(2,addr);HT1622_dispON(3,addr);
				HT1622_dispON(4,addr);HT1622_dispOFF(5,addr);
				HT1622_dispON(6,addr);
				break;
	}
}

void HT1622_duan_min(u8 addr,u8 data)
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
	}
}

void HT1622_duan_big(u8 addr,u8 data)
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

void HT1622_duan_big_t(u8 addr,u8 data)
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

extern u8 cur_time[3];

void HT1622_show_time(void)
{
	u8 hour = 0;
	u8 min = 0;
	u8 sec = 0;
	
	get_time();//获取时间
	
	hour = cur_time[2];
	min = cur_time[1];
	sec = cur_time[0];
	
	if( (hour/10)==0 )
	{
		HT1622_dispOFF(4,12);
		HT1622_dispOFF(5,12);
		HT1622_dispOFF(6,12);
	}
	else if( (hour/10)==1 )
	{
		HT1622_dispOFF(4,12);
		HT1622_dispON(5,12);
		HT1622_dispON(6,12);
	}
	else if( (hour/10)==2 )
	{
		HT1622_dispON(4,12);
		HT1622_dispOFF(5,12);
		HT1622_dispON(6,12);
	}
	
	HT1622_dispON(0,10);//小数点
	HT1622_dispON(0,9);//小数点
	
	HT1622_duan_min(11,hour%10);//小时 个位
	
	HT1622_duan_min(10,min/10);//分钟 十位
	HT1622_duan_min(9,min%10);//分钟 个位
	
	HT1622_duan_min(8,sec/10);//秒 十位
	HT1622_duan_min(7,sec%10);//秒 十位
	
	HT1622_disp_renew();
}

void HT1622_show_setp(u8 behind_setp,u8 rear_setp)
{
	HT1622_duan_big(1,behind_setp);
	HT1622_duan_big(3,rear_setp);
}
/*
void disp_All_2(void)
{
    u8  i;
	//全显示屏幕 
    CS_2 = 0;       
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
    CS_2 = 1;
    for(i=0; i<32; i++)
    {	
		//填充显存
        dispram[i] = 0xff;    
    }

}*/
void HT1622_Init(void)
{
    u8  j = 0;
    for(j=0; j<32; j++)
    {
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
		HT1622_clean();
		
		HT1622_dispON(2,6);//电池
		HT1622_dispON(0,5);
		HT1622_dispON(0,11);
		
		HT1622_dispON(2,0);//km
		HT1622_dispON(3,0);//h
		
		HT1622_dispON(4,0);//RS
		HT1622_dispON(6,2);//FS
}

