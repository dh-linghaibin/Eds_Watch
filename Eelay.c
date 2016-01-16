/*
*The use of the program cycle to achieven the role delay
*/
#include "Delay.h"

/*
*time delay use count 
*Execution time: one ms
*Note:blocking program,imprecise
*/
void DelayMs(u16 ms) {						
	u16 i;
	while(ms--) {
		for(i=0;i<1125;i++);//2M crystal cycle 1us, i = 140; just 1ms, when 16M, i = 1125
	}
}
/*
*time delay use count 
*Execution time: one ms
*Note:blocking program,imprecise
*/
void DelayUs(u16 us) {	
	while(us--);
}
