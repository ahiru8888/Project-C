#include "sys.h"  


//WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//Đóng tất cả các ngắt
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//Mở tất cả các ngắt
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//Đặt địa chỉ ngăn xếp
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}
















