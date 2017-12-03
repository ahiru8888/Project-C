#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "myiic.h"
#include "ds1307.h"  

//Slave address
//#define ADDRESS		0xD0 // 1101 000 0
const u8 TEXT_Buffer[]={"STM32F4 I2C TEST"};
#define SIZE sizeof(TEXT_Buffer)	 
	
int main(void)
{ 
	
	u16 i=0,ii=0;
	//u8 data[] = {0, 1, 2};
	char str[100];
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);	
	
	LED_Init();	LED1=0;	 
	//KEY_Init();	 
//	/* Initialize I2C, SCL: PB6 and SDA: PB7 with 100kHt serial clock */
//	IIC_Init(I2C1, I2C_PinsPack_1, 100000);

//	/* Write "5" at location 0x00 to slave with address ADDRESS */
//	I2C_Write(I2C1, ADDRESS, 0x00, 5);
//	
//	/**
//	 * Write multi bytes to slave with address ADDRESS
//	 * Write to registers starting from 0x00, get data in variable "data" and write 3 bytes
//	 */
//	I2C_WriteMulti(I2C1, ADDRESS, 0x00, data, 3);
//	
//	/* Read single byte from slave with 0xD0 (1101 000 0) address and register location 0x00 */
//	data[0] = I2C_Read(I2C1, ADDRESS, 0x00);
//	
//	/**
//	 * Read 3 bytes of data from slave with 0xD0 address
//	 * First register to read from is at 0x00 location 
//	 * Store received data to "data" variable
//	 */
//	I2C_ReadMulti(I2C1, 0xD0, 0x00, data, 3);
	
	/* Initialize LCD */
	if (LCD_Init() != LCD_Result_Ok) {
		/* Red LED on */
		LED1=1;
		while (1);
	}

	  LCDI2C_init(0x7E,16,2);
  // ------- Quick 3 blinks of backlight  -------------
  
//  for( ii = 0; ii< 3; ii++)
//  {
//    LCDI2C_backlight();
//    delay_ms(250);
//    LCDI2C_noBacklight();
//    delay_ms(250);
//  }
  LCDI2C_backlight(); // finish with backlight on

  LCDI2C_clear();

   LCDI2C_setCursor(2,0);
   LCDI2C_write_String("Hello !");


  delay_ms(2000);
	
	while (1) {
		i++;
		
	
		/* Toggle GREEN led if needed */
		if (i%100==0) {
			
			/* Toggle GREEN LED */
			LED0=!LED0;
		}
	} 	    
}
