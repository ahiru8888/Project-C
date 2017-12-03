#include "delay.h"
#include "sys.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//Èç¹ûÊ¹ÓÃOS,Ôò°üÀ¨ÏÂÃæµÄÍ·ÎÄ¼ş£¨ÒÔucosÎªÀı£©¼´¿É.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//Ö§³ÖOSÊ±£¬Ê¹ÓÃ	  
#endif
//////////////////////////////////////////////////////////////////////////////////  
//delay_us
//delay_ms

////////////////////////////////////////////////////////////////////////////////// 

static u8  fac_us=0;							//usÑÓÊ±±¶³ËÊı			   
static u16 fac_ms=0;							//msÑÓÊ±±¶³ËÊı,ÔÚosÏÂ,´ú±íÃ¿¸ö½ÚÅÄµÄmsÊı
	
#if SYSTEM_SUPPORT_OS							//Èç¹ûSYSTEM_SUPPORT_OS¶¨ÒåÁË,ËµÃ÷ÒªÖ§³ÖOSÁË(²»ÏŞÓÚUCOS).
//µ±delay_us/delay_msĞèÒªÖ§³ÖOSµÄÊ±ºòĞèÒªÈı¸öÓëOSÏà¹ØµÄºê¶¨ÒåºÍº¯ÊıÀ´Ö§³Ö
//Ê×ÏÈÊÇ3¸öºê¶¨Òå:
//    delay_osrunning:ÓÃÓÚ±íÊ¾OSµ±Ç°ÊÇ·ñÕıÔÚÔËĞĞ,ÒÔ¾ö¶¨ÊÇ·ñ¿ÉÒÔÊ¹ÓÃÏà¹Øº¯Êı
//delay_ostickspersec:ÓÃÓÚ±íÊ¾OSÉè¶¨µÄÊ±ÖÓ½ÚÅÄ,delay_init½«¸ù¾İÕâ¸ö²ÎÊıÀ´³õÊ¼¹şsystick
// delay_osintnesting:ÓÃÓÚ±íÊ¾OSÖĞ¶ÏÇ¶Ì×¼¶±ğ,ÒòÎªÖĞ¶ÏÀïÃæ²»¿ÉÒÔµ÷¶È,delay_msÊ¹ÓÃ¸Ã²ÎÊıÀ´¾ö¶¨ÈçºÎÔËĞĞ
//È»ºóÊÇ3¸öº¯Êı:
//  delay_osschedlock:ÓÃÓÚËø¶¨OSÈÎÎñµ÷¶È,½ûÖ¹µ÷¶È
//delay_osschedunlock:ÓÃÓÚ½âËøOSÈÎÎñµ÷¶È,ÖØĞÂ¿ªÆôµ÷¶È
//    delay_ostimedly:ÓÃÓÚOSÑÓÊ±,¿ÉÒÔÒıÆğÈÎÎñµ÷¶È.

//±¾Àı³Ì½ö×÷UCOSIIºÍUCOSIIIµÄÖ§³Ö,ÆäËûOS,Çë×ÔĞĞ²Î¿¼×ÅÒÆÖ²
//Ö§³ÖUCOSII
#ifdef 	OS_CRITICAL_METHOD						//OS_CRITICAL_METHOD¶¨ÒåÁË,ËµÃ÷ÒªÖ§³ÖUCOSII				
#define delay_osrunning		OSRunning			//OSÊÇ·ñÔËĞĞ±ê¼Ç,0,²»ÔËĞĞ;1,ÔÚÔËĞĞ
#define delay_ostickspersec	OS_TICKS_PER_SEC	//OSÊ±ÖÓ½ÚÅÄ,¼´Ã¿Ãëµ÷¶È´ÎÊı
#define delay_osintnesting 	OSIntNesting		//ÖĞ¶ÏÇ¶Ì×¼¶±ğ,¼´ÖĞ¶ÏÇ¶Ì×´ÎÊı
#endif

//Ö§³ÖUCOSIII
#ifdef 	CPU_CFG_CRITICAL_METHOD					//CPU_CFG_CRITICAL_METHOD¶¨ÒåÁË,ËµÃ÷ÒªÖ§³ÖUCOSIII	
#define delay_osrunning		OSRunning			//OSÊÇ·ñÔËĞĞ±ê¼Ç,0,²»ÔËĞĞ;1,ÔÚÔËĞĞ
#define delay_ostickspersec	OSCfg_TickRate_Hz	//OSÊ±ÖÓ½ÚÅÄ,¼´Ã¿Ãëµ÷¶È´ÎÊı
#define delay_osintnesting 	OSIntNestingCtr		//ÖĞ¶ÏÇ¶Ì×¼¶±ğ,¼´ÖĞ¶ÏÇ¶Ì×´ÎÊı
#endif


//us¼¶ÑÓÊ±Ê±,¹Ø±ÕÈÎÎñµ÷¶È(·ÀÖ¹´ò¶Ïus¼¶ÑÓ³Ù)
void delay_osschedlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD   			//Ê¹ÓÃUCOSIII
	OS_ERR err; 
	OSSchedLock(&err);						//UCOSIIIµÄ·½Ê½,½ûÖ¹µ÷¶È£¬·ÀÖ¹´ò¶ÏusÑÓÊ±
#else										//·ñÔòUCOSII
	OSSchedLock();							//UCOSIIµÄ·½Ê½,½ûÖ¹µ÷¶È£¬·ÀÖ¹´ò¶ÏusÑÓÊ±
#endif
}

//us¼¶ÑÓÊ±Ê±,»Ö¸´ÈÎÎñµ÷¶È
void delay_osschedunlock(void)
{	
#ifdef CPU_CFG_CRITICAL_METHOD   			//Ê¹ÓÃUCOSIII
	OS_ERR err; 
	OSSchedUnlock(&err);					//UCOSIIIµÄ·½Ê½,»Ö¸´µ÷¶È
#else										//·ñÔòUCOSII
	OSSchedUnlock();						//UCOSIIµÄ·½Ê½,»Ö¸´µ÷¶È
#endif
}

//µ÷ÓÃOS×Ô´øµÄÑÓÊ±º¯ÊıÑÓÊ±
//ticks:ÑÓÊ±µÄ½ÚÅÄÊı
void delay_ostimedly(u32 ticks)
{
#ifdef CPU_CFG_CRITICAL_METHOD
	OS_ERR err; 
	OSTimeDly(ticks,OS_OPT_TIME_PERIODIC,&err);//UCOSIIIÑÓÊ±²ÉÓÃÖÜÆÚÄ£Ê½
#else
	OSTimeDly(ticks);						//UCOSIIÑÓÊ±
#endif 
}
 
//systickÖĞ¶Ï·şÎñº¯Êı,Ê¹ÓÃOSÊ±ÓÃµ½
void SysTick_Handler(void)
{	
	
	if(delay_osrunning==1)					//OS¿ªÊ¼ÅÜÁË,²ÅÖ´ĞĞÕı³£µÄµ÷¶È´¦Àí
	{
		OSIntEnter();						//½øÈëÖĞ¶Ï
		OSTimeTick();       				//µ÷ÓÃucosµÄÊ±ÖÓ·şÎñ³ÌĞò               
		OSIntExit();       	 				//´¥·¢ÈÎÎñÇĞ»»ÈíÖĞ¶Ï
	}
}
#endif
			   
//³õÊ¼»¯ÑÓ³Ùº¯Êı
//µ±Ê¹ÓÃOSµÄÊ±ºò,´Ëº¯Êı»á³õÊ¼»¯OSµÄÊ±ÖÓ½ÚÅÄ
//SYSTICKµÄÊ±ÖÓ¹Ì¶¨ÎªAHBÊ±ÖÓµÄ1/8
//SYSCLK:ÏµÍ³Ê±ÖÓÆµÂÊ
void delay_init(u8 SYSCLK)
{
#if SYSTEM_SUPPORT_OS 						//Èç¹ûĞèÒªÖ§³ÖOS.
	u32 reload;
#endif
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
	fac_us=SYSCLK/8;						//²»ÂÛÊÇ·ñÊ¹ÓÃOS,fac_us¶¼ĞèÒªÊ¹ÓÃ
#if SYSTEM_SUPPORT_OS 						//Èç¹ûĞèÒªÖ§³ÖOS.
	reload=SYSCLK/8;						//Ã¿ÃëÖÓµÄ¼ÆÊı´ÎÊı µ¥Î»ÎªM	   
	reload*=1000000/delay_ostickspersec;	//¸ù¾İdelay_ostickspersecÉè¶¨Òç³öÊ±¼ä
											//reloadÎª24Î»¼Ä´æÆ÷,×î´óÖµ:16777216,ÔÚ168MÏÂ,Ô¼ºÏ0.7989s×óÓÒ	
	fac_ms=1000/delay_ostickspersec;		//´ú±íOS¿ÉÒÔÑÓÊ±µÄ×îÉÙµ¥Î»	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//¿ªÆôSYSTICKÖĞ¶Ï
	SysTick->LOAD=reload; 					//Ã¿1/delay_ostickspersecÃëÖĞ¶ÏÒ»´Î	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; 	//¿ªÆôSYSTICK    
#else
	fac_ms=(u16)fac_us*1000;				//·ÇOSÏÂ,´ú±íÃ¿¸ömsĞèÒªµÄsystickÊ±ÖÓÊı   
#endif
}								    
/////////////////há»— trá»£ OS//////////////////////////////////////////////////////////////
#if SYSTEM_SUPPORT_OS 						//Èç¹ûĞèÒªÖ§³ÖOS.
//nus: sá»‘ us	
//nus:0~204522252(2^32/fac_us@fac_us=21)	    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOADµÄÖµ	    	 
	ticks=nus*fac_us; 						//ĞèÒªµÄ½ÚÅÄÊı 
	delay_osschedlock();					//×èÖ¹OSµ÷¶È£¬·ÀÖ¹´ò¶ÏusÑÓÊ±
	told=SysTick->VAL;        				//¸Õ½øÈëÊ±µÄ¼ÆÊıÆ÷Öµ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//ÕâÀï×¢ÒâÒ»ÏÂSYSTICKÊÇÒ»¸öµİ¼õµÄ¼ÆÊıÆ÷¾Í¿ÉÒÔÁË.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//Ê±¼ä³¬¹ı/µÈÓÚÒªÑÓ³ÙµÄÊ±¼ä,ÔòÍË³ö.
		}  
	};
	delay_osschedunlock();					//»Ö¸´OSµ÷¶È											    
}  

//nms:sá»‘ ms
//nms:0~65535
void delay_ms(u16 nms)
{	
	if(delay_osrunning&&delay_osintnesting==0)//Èç¹ûOSÒÑ¾­ÔÚÅÜÁË,²¢ÇÒ²»ÊÇÔÚÖĞ¶ÏÀïÃæ(ÖĞ¶ÏÀïÃæ²»ÄÜÈÎÎñµ÷¶È)	    
	{		 
		if(nms>=fac_ms)						//ÑÓÊ±µÄÊ±¼ä´óÓÚOSµÄ×îÉÙÊ±¼äÖÜÆÚ 
		{ 
   			delay_ostimedly(nms/fac_ms);	//OSÑÓÊ±
		}
		nms%=fac_ms;						//OSÒÑ¾­ÎŞ·¨Ìá¹©ÕâÃ´Ğ¡µÄÑÓÊ±ÁË,²ÉÓÃÆÕÍ¨·½Ê½ÑÓÊ±    
	}
	delay_us((u32)(nms*1000));				//ÆÕÍ¨·½Ê½ÑÓÊ±
}
//////////////khÃ´ng há»— trá»£ OS//////////////////////////////////////////////////////////////////////////
#else  //²»ÓÃucosÊ±
//ÑÓÊ±nus
//nusÎªÒªÑÓÊ±µÄusÊı.	
//nus 798915us(2^24/fac_us@fac_us=21)
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 				//Ê±¼ä¼ÓÔØ	  		 
	SysTick->VAL=0x00;        				//Çå¿Õ¼ÆÊıÆ÷
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //¿ªÊ¼µ¹Êı 	 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//µÈ´ıÊ±¼äµ½´ï   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //¹Ø±Õ¼ÆÊıÆ÷
	SysTick->VAL =0X00;       				//Çå¿Õ¼ÆÊıÆ÷ 
}
// Trá»… nMS
// SysTick->LOAD  thanh ghi 24-bit, do Ä‘Ã³, sá»± cháº­m trá»… tá»‘i Ä‘a lÃ :
// nMS <= 0xffffff * 8 * 1000 / SYSCLK
// SYSCLK lÃ  Hz, nMS sá»‘ ms
// 168M, NMS <= 798ms
void delay_xms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;			//Ê±¼ä¼ÓÔØ(SysTick->LOADÎª24bit)
	SysTick->VAL =0x00;           			//Çå¿Õ¼ÆÊıÆ÷
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //¿ªÊ¼µ¹Êı 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//µÈ´ıÊ±¼äµ½´ï   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //¹Ø±Õ¼ÆÊıÆ÷
	SysTick->VAL =0X00;     		  		//Çå¿Õ¼ÆÊıÆ÷	  	    
} 
//nms 
//nms:0~65535
void delay_ms(u16 nms)
{	 	 
	u8 repeat=nms/540;						//ÕâÀïÓÃ540,ÊÇ¿¼ÂÇµ½Ä³Ğ©¿Í»§¿ÉÄÜ³¬ÆµÊ¹ÓÃ,
											//±ÈÈç³¬Æµµ½248MµÄÊ±ºò,delay_xms×î´óÖ»ÄÜÑÓÊ±541ms×óÓÒÁË
	u16 remain=nms%540;
	while(repeat)
	{
		delay_xms(540);
		repeat--;
	}
	if(remain)delay_xms(remain);
} 
#endif
			 

