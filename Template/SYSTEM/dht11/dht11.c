/**------------ DHT11 Functions ----------------------*
--------------------------------------------------------*/

#include "delay.h"
#include "dht11.h"
#include "tm_stm32f4_delay.h"

/*-------- Signal Pin : PDO   --------
						+ : 5V
*/

unsigned char values[5];
void output_GPIOD(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void input_GPIOD(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
	GPIO_Init(GPIOD, &GPIO_InitStructure); 	
}
void start_signal(void)
{
	output_GPIOD();
	GPIO_SetBits(GPIOD,GPIO_Pin_0);
	GPIO_ResetBits(GPIOD,GPIO_Pin_0);
	Delay_us(18000); //18000
	GPIO_SetBits(GPIOD,GPIO_Pin_0);
	Delay_us(26); // 26
	input_GPIOD();	
}

uint8_t Read_Byte()
{
	uint8_t i, data = 0;
	for(i=0;i<8;i++)
	{
		data <<= 1;
		while(!GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0));
		Delay_us(30);
		if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0))
		{
			data |= 1;
		}
		while(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0));
	}
	return data;
}

unsigned char get_data(void)
{
	unsigned char s = 0; 
  unsigned char check_sum = 0;
	start_signal();
	if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0))
	{
		return 1; // Success connect to sensor
	}
	Delay_us(80);
	if(!GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0))
	{
		return 2; // No sensor found
	}
	Delay_us(80);
	
	for(s = 0; s <= 4; s += 1) 
	{
		values[s] = Read_Byte(); 
	}
	
	output_GPIOD();
	GPIO_SetBits(GPIOD,GPIO_Pin_0);
	
	for(s = 0; s < 4; s += 1) 
   { 
       check_sum += values[s]; 
   } 
    
   if(check_sum != values[4]) 
   { 
      return 3;  // Check_sum error
   } 
   else 
   { 
      return 0; 
   } 
}
uint8_t Humd_int(void)
{
	uint8_t hud;
	hud = values[0];
	return hud;
}
uint8_t Humd_float(void)
{
	uint8_t hud;
	hud = values[1];
	return hud;
}
uint8_t Temp_int(void)
{
	uint8_t temp;
	temp = values[2];
	return temp;
}
uint8_t Temp_float(void)
{
	uint8_t temp;
	temp = values[3];
	return temp;
}
