

#include "Htlcd.h"
#include "Delay.h"


#define       LCD_ON          0x03    //����ƫѹ������
#define       LCD_OFF         0x02    //�ر�ƫѹ������
#define       SYS_DIS         0x00    //�ر�������LCD����
#define       SYS_EN          0x01    //����ϵͳ����
#define       RC32K           0x18    //�����ڲ�32KRC
#define       TONE_OFF        0x08    //�رշ�����
#define       TONE_4K         0x40    //����4 khz�����ź�
#define       TONE_2K         0x60    //����2 khz�����ź�

#define       COM0        0
#define       COM1        1
#define       COM2        2
#define       COM3        3
#define       COM4        4
#define       COM5        5
#define       COM6        6
#define       COM7        7

#define       DATA         PC_ODR_ODR5      //����
#define       WR           PC_ODR_ODR6       //д
#define       CS           PD_ODR_ODR0          //Ƭѡ
#define       CS_2         PD_ODR_ODR0          //Ƭѡ

static u8 dispram[32];    //Ԥ����ʾ���棬8x32

//dat �ĸ�cnt λд��HT1621���ȷ��͸�λ
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

//data�ĵ�cntλд��1621
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
//ָ��ģʽ
static void Sendcmd(u8 command)
{
	CS = 0; 
	//д���־��"100"
	SendBit_HL(0x80,3); 
	//д�� 9 λ����,����ǰ 8 λΪ command ����,��� 1 λ����
	SendBit_HL(command,9);  
	CS = 1;
}

static void Sendcmd_2(u8 command)
{
	CS_2 = 0;
	//д���־��"100"
	SendBit_HL(0x80,3);  
	//д�� 9 λ����,����ǰ 8 λΪ command ����,��� 1 λ����
	SendBit_HL(command,9);  
	CS_2 = 1;
}

//����д��Ԫ SEG(0~31)
static void W1622_success(u8 SEG,u8 cnt,u8 *p)
{
	u8 i,addr;
	addr = SEG * 2;
	CS = 0;
	//д���־��"101"
	SendBit_HL(0xa0,3);  
	//д�� 6 λ addr
	SendBit_HL(addr<<2,6); 
	for(i=0; i<cnt;i++)
	{
		SendBit_LH(*p,4);
		SendBit_LH(*p>>4,4);
		p++;
	}
	CS = 1;
}

//��ʾ����,����8x32��ȫ��ˢ��
void HtlcdRefresh(void)   
{
	W1622_success(0,32,dispram);
}

//com(0-7)  SEG(0-31)
void HT1622_dispON(u8 com,u8 SEG)
{
    dispram[SEG] |= (1<<com);
	//ȥ��ע�ͺ�д���������ʾ
   // W1622_byte(SEG,dispram[SEG]);     
}

//com(0-7)  SEG(0-31)
void HT1622_dispOFF(u8 com,u8 SEG)
{
    dispram[SEG] &= ~(1<<com);
	//ȥ��ע�ͺ�д���������ʾ
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
	//����Ļ
    CS = 0;     
	//д���־��"101"
    SendBit_HL(0xa0,3);  
	//д�� 6 λ addr
    SendBit_HL(0,6); 
	//����д������
    for(i =0; i <32; i ++) 
    {
        SendBit_LH(0,4);
        SendBit_LH(0,4);
    }
    CS = 1;
    for(i=0; i<32; i++)
    {
		//����Դ�
        dispram[i] = 0x00;    
    }
}

void HtlcdDisAll(void)
{
    u8  i;
	//ȫ��ʾ��Ļ
    CS = 0;      
	//д���־��"101"
    SendBit_HL(0xa0,3);  
	//д�� 6 λ addr
    SendBit_HL(0,6); 
	//����д������
    for(i =0; i <32; i ++) 
    {
        SendBit_LH(0xff,4);
        SendBit_LH(0xff,4);
    }
    CS = 1;
    for(i=0; i<32; i++)
    {
		//����Դ�
			dispram[i] = 0xff;    
    }

}


void HtcldSetAdr(u8 addr, u8 *num) {
    u8 set_i = 0;
    for( ; set_i < 7; set_i++) {
        HtlcdSetSisp(set_i,addr,num[set_i]);
    }
}

void HtlcdAdrNum(u8 addr,u8 data)//����һ��
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

void HtlcdAdrTime(u8 addr,u8 data)//��ʾʱ���Сλ��
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

void HtlcdAdrBigNum(u8 addr,u8 data)//��ʾ�ٶȵ�λ��
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

void HtlcdAdrDoubleNum(u8 addr,u8 data)//д���� ���һ��
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

void HtlcdTime(u8 hour, u8 min, u8 sec) {//��ʾʱ��
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
	HT1622_dispON(0,10);//С����
	HT1622_dispON(0,9);//С����
	
	HtlcdAdrTime(11,hour%10);//Сʱ ��λ
	
	HtlcdAdrTime(10,min/10);//���� ʮλ
	HtlcdAdrTime(9,min%10);//���� ��λ
	
	HtlcdAdrTime(8,sec/10);//�� ʮλ
	HtlcdAdrTime(7,sec%10);//�� ʮλ
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
    HtlcdAdrNum(5,num3);
}

void HtlcdSetStalls(u8 bit, u8 num) {//������һ��
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

#define BACKLIGHT1 PC_ODR_ODR1
#define BACKLIGHT2 PC_ODR_ODR2
#define BACKLIGHT3 PC_ODR_ODR3

void HtlcdInit(void) {
    u8  j = 0;
    //CS PD0
    PD_DDR |= BIT(0);
    PD_CR1 |= BIT(0); 
    PD_CR2 &= ~BIT(0);
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
    //���� PC1 PC2 PC3 
    PC_DDR |= BIT(1);
    PC_CR1 |= BIT(1); 
    PC_CR2 |= BIT(1);

    PC_DDR |= BIT(2);
    PC_CR1 |= BIT(2); 
    PC_CR2 |= BIT(2);

    PC_DDR |= BIT(3);
    PC_CR1 |= BIT(3); 
    PC_CR2 |= BIT(3);
    for(j=0; j<32; j++) {
        dispram[j] = 0;
    }
    Sendcmd(0x05); 
    //RC32k
    Sendcmd(RC32K);   
    //��ϵͳ����
    Sendcmd(SYS_EN);  
    //�� LCDƫѹ������
    Sendcmd(LCD_ON);  
    //RC32k
    Sendcmd_2(RC32K);   
    //��ϵͳ����
    Sendcmd_2(SYS_EN);  
    //ʱ�ӷ���
    Sendcmd_2(0xa0);  
    //�� LCDƫѹ������
    Sendcmd_2(LCD_ON);
    //���
    HtlcdClean();
    
    HT1622_dispON(2,6);//���
    HT1622_dispON(0,5);
    HT1622_dispON(0,11);

    HT1622_dispON(4,0);//RS
    HT1622_dispON(6,2);//FS
    //HtlcdRegion2(1,1);
    //HtlcdRefresh();
}
void HtlcdOpenBacklight(void) {
    BACKLIGHT1 = 0;
    BACKLIGHT2 = 0;
    BACKLIGHT3 = 0;
    Sendcmd(LCD_ON);  
}

void HtlcdCloseBacklight(void) {
    BACKLIGHT1 = 1;
    BACKLIGHT2 = 1;
    BACKLIGHT3 = 1;
    Sendcmd(LCD_OFF);  
}