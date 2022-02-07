#include "regs240x.h"

void cleardog() /*inline*/
{
	*WDKEY = 0x055;  
	*WDKEY = 0x0AA;  
}