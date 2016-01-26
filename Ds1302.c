

#include "Ds1302.h"
#include "Delay.h"


#define DS13CLK 	PB_ODR_ODR2		    //1302 CLK
#define DS13IO	    PB_DDR_DDR1		    //IO����0Ϊ���룬1Ϊ���
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

/*��DS1302дһ���ֽ�*/
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
/*��DS1302��һ���ֽ�*/
static u8 _rds13byte(void) {
    u8 i,_code=0;
    DS13CLK =0;
    //PB1_CR2=0;//��ֹ�ж�
    DS13IO=0;//��Ϊ����
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
    DS13IO=0;//��Ϊ����
    return _code;
}
/*������ _code����������   */
static u8 readds1302(u8 _code) {
    DS13CS =0;         /*�ر�DS1302*/ 
    DS13CLK =0;
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    DS13CS =1;          /*ʹ��DS1302*/
    NOP();NOP();NOP();NOP();NOP();NOP();
    _wds13byte(_code);   /*������*/
    _code=_rds13byte(); /*���ض�ȡ����*/
    DS13CLK =1;
    DS13CS =0;         /*�ر�DS1302*/ 
    return _code;
}
/*д���� fpд�ĵ�ַ��_codeд������*/
static void writeds1302(u8 fp,u8 _code) {
    DS13CS =0;         /*�ر�DS1302*/ 
    DS13CLK =0;
    DS13CS =1;          /*ʹ��DS1302*/
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    _wds13byte(fp);     /*д��������*/
    _wds13byte(_code); /*д������*/
    DS13CLK=1;
    DS13CS =0;          /*�ر�DS1302*/ 
}

static u8 cur_time[3];
u8* Ds1302GetTime(u8 bit) {
	u8 i,t;
    
	u8 add=0x81;
	for(i=0;i<3;i++) {
		t=readds1302(add);	
		cur_time[i]=((t&0x70)>>4)*10+(t&0x0F);//����������ת��						 
		add+=2;
	}
    return cur_time;
}
//��1302����ʱ��
void Ds1302SetTime(u8 set_hour, u8 set_min) {
    u8 tempdata = 0;
    writeds1302(0x8e,0x00); //���д����
    writeds1302(0x80,0);//���0����һ������ʱ����,����������������1302
    tempdata=set_min/10;
    tempdata<<=4;
    tempdata|=(set_min%10);
    writeds1302(0x82,tempdata);//д֮ǰҪ��ʱ����ʮλ��λ���룬10λ�Ÿ�3λ����λ�ŵ�4λ��
    tempdata=set_hour/10;
    tempdata<<=4;
    tempdata|=(set_hour%10);
    writeds1302(0x84,tempdata);
    writeds1302(0x8e,0x80); //��λд����
}
