#include <stm32f4xx.h>
#include "usart.h"	
#include "delay.h"	
#include "string.h"
#include <stdlib.h>
#include <stdio.h>
#include "defines.h"
#include "tm_stm32f4_i2c.h"
#include "tm_stm32f4_ssd1306.h"
#include "tm_stm32f4_delay.h"
#include "dht11.h"

#define BH1750 0x46
#define H_resolution 0x10 //Continuously H-resolution mode

int count =0; int config_finish=0;
unsigned char state = 0;
unsigned char dht11[5];
char do_am[15] = "Do am:"; char nhiet_do[15] = "Nhiet do:"; char anh_sang[15] = "Anh Sang:";
uint8_t receive[2]; // Bien cua lux
uint16_t lux;
float temp;
 
int pwm=0;

uint8_t get_c =0; 
int answer;
uint8_t sms=0; int flag_sms=0; 
//const int buffer_size=300;
//char buffer[buffer_size];
char ON1[]="ON1"; char ON2[]="ON2"; char OFF1[]="OFF1"; char OFF2[]="OFF2";
char request[] = "api.thingspeak.com/update?api_key=048GYNIUKCM0SZ0Y&field1=%d&field2=%d";
/*---- Send AT command --------
--------------------------------*/
uint8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout)
{
	uint8_t x=0,  answer=0;
	uint8_t c;
  char response[100];
	
	memset(response, '\0', 100);    // Initialize the string
	Delay_us(100000);	
	//while(!(USART_BufferEmpty(USART2))) {USART_ClearBuffer(USART2);} // Clean the input buffer
	while( USART_Getc(USART2) > 0) ; 
	USART_Puts(USART2,ATcommand);
	x=0;
	do
	{
		Delay_us(1000);
		c= USART_Getc(USART2); USART_Putc(USART3,c);
		if( c != 0 )
		{
			response[x] = c;
			x++;
			/* check if the desired answer is in the response of the module */
      if (strstr(response, expected_answer) != NULL)  answer = 1;	    						
		}
	}
	while((answer == 0) && (--timeout>0)); // Waits for the answer with time out
	c=0;
	return answer;
}

void InitGPIOD(void)		
{
	GPIO_InitTypeDef GPIO_InitStruct; 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13; 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 	
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; 	
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL; 
	GPIO_Init(GPIOD, &GPIO_InitStruct);
//	GPIO_SetBits(GPIOD,GPIO_Pin_8);
}
/******* Get Sensor data and print to Computer *********
*********************************************************/
void config_BH1750(void)
{
	uint8_t check =0;
	TM_I2C_Init(I2C1, TM_I2C_PinsPack_1, 100000); //PB6 -- 
	check = TM_I2C_IsDeviceConnected(I2C1, BH1750);
	if (check)
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_13);
	}
	else while(1);
}
void get_Dht11(void)
{
	state = get_data();
		dht11[0] = Humd_int();
		dht11[1] = Humd_float();
		dht11[2] = Temp_int();
		dht11[3] = Temp_float();
}
void print_DHTandBHT(void)
{
	char buffer[100];
	int lux;
	lux = (int) temp;
//	sprintf(buffer,"%s %d.%d\r\n%s %d.%d\r\n%s %f\r\n",do_am,dht11[0],dht11[1],nhiet_do,dht11[2],dht11[3],anh_sang,temp);
	sprintf(buffer,"S-%d-%d-%d-a",dht11[2],dht11[0],lux);
	USART_Puts(USART3,buffer);
}

void get_bht1750(void)
{
	TM_I2C_Write(I2C1, BH1750, H_resolution, 0);
	Delay_us(180000);
	TM_I2C_ReadMulti(I2C1,BH1750,0,receive,2);
	Delay_us(10000);
	lux = (uint16_t) ((receive[0]<<8) | receive[1]);		
	temp =(float) lux /1.2;
}


/******** Config Sim and Upload data to web server ****************
********************************************************************/
void config_sms(void)
{
	sendATcommand("AT+CMGF=1\r\n","OK",2000000);         /* Config SMS text */
  sendATcommand("AT+CNMI=2,2\r\n","OK",2000000);    /* Config SMS memory */
  sendATcommand("AT+CMGDA=\"DEL ALL\"\r\n","OK",2000000);
  sendATcommand("AT+CLIP=1\r\n","OK",2000000);   
}
void config_http(void)
{
	sendATcommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n","OK",2000000);// Delay_us(1000000);
	sendATcommand("AT+SAPBR=3,1,\"APN\",\"m_wap\"\r\n","OK",2000000); USART_Puts(USART3,"Config APN");
	USART_Puts(USART2,"AT+SAPBR=1,1\r\n");  Delay_us(4000000); USART_Puts(USART3,"Config SAPBR");
	USART_Puts(USART2,"AT+HTTPINIT\r\n");  Delay_us(1000000); USART_Puts(USART3,"Config HTTP");
	USART_Puts(USART2,"AT+HTTPPARA=\"CID\",1\r\n");  Delay_us(1000000); USART_Puts(USART3,"Config CID");
}
void send_sms(char* sms_text,char* phone_number)
{
	//char phone_number[] = "0984095710";
	char buffer[20];
	sprintf(buffer,"AT+CMGS=\"%s\"\r\n", phone_number);	
	 do { answer = sendATcommand(buffer,">",2000000);}
	 while(answer==0);    // Send the SMS number 
	if (answer == 1)
	{
		USART_Puts(USART3,"Sending SMS ...\r\n");
		USART_Puts(USART2,sms_text);	Delay_us(10000);
		USART_Puts(USART2,"\x1A");	// Send 0x1A
		answer=0;
	}
}

void up_data(uint8_t temp,uint8_t humd)
{
	
	// request the url
	char buff[100], buff_data[100];
	sprintf(buff,request,temp,humd);
	sprintf(buff_data,"AT+HTTPPARA=\"URL\",\"%s\"\r\n",buff);
	 do { answer = sendATcommand(buff_data,"OK",10000000); }
	 while (answer==0);
	if(answer==1)
	{
		USART_Puts(USART3,"Uploading data...\r\n");
		sendATcommand("AT+HTTPACTION=0\r\n","OK",100000); USART_Puts(USART3,"Uploading success!\r\n");
		answer =0;
	}
}
void call(char *number)
{
	char buffer[20];
	sprintf(buffer,"ATD%s;r\n", number);
	do { answer = sendATcommand(buffer,"OK",2000000);}
	 while(answer==0);    // Calling  
	if (answer == 1)
	{
		USART_Puts(USART3,"Call success\r\n");	answer=0;
	}
}
void receive_sms(void)
{
	char buf_SMS[100];
	answer = sendATcommand("AT+CMGR=0\r\n","+CMGR:",2000);
	if(answer ==1)
	{
		answer=0;
		while(!(USART_Gets(USART2,buf_SMS,100)));
		do
		{
			if(USART_Gets(USART2,buf_SMS,100))
			{
				/*	Bat chuoi thuc hien dieu khien thiet bi	*/
				if (strstr(buf_SMS, ON1) != NULL)				GPIO_SetBits(GPIOD,GPIO_Pin_12);
				else if (strstr(buf_SMS, ON2) != NULL)	GPIO_SetBits(GPIOD,GPIO_Pin_13);
				else if (strstr(buf_SMS, OFF1) != NULL)	GPIO_ResetBits(GPIOD,GPIO_Pin_12);				
				else if (strstr(buf_SMS, OFF2) != NULL)	GPIO_ResetBits(GPIOD,GPIO_Pin_13);	
								
				if (strstr(buf_SMS, "OK") != NULL)    
        {
          answer = 1;
					USART_Puts(USART3,buf_SMS);
					memset(buf_SMS, '\0', 100);
        }
			}
		}
		while (answer==0);
	}
	else {USART_Puts(USART3,"Error");}
}

/******** Control PWM of LED ****************
************************************************/
void TIM_PWM_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

  /* TIM4 and GPIOD clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD , ENABLE);
	/* GPIOD Configuration*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
	/* Connect TIM4 pins to AF */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 83; // 84-1
  TIM_TimeBaseStructure.TIM_Period = 19999; //20000-1  (SystemClock/2)/84 ||84 la Prescaler || Tao ra chu ki T =0.02s
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//  TIM_OCInitStructure.TIM_Pulse = 10 * 20000 / 100;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

	 TIM_ARRPreloadConfig(TIM4, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM4, ENABLE);
}

void TM_DELAY_1usHandler(void)
{
	if(config_finish==1)
	{
		count++;
		if(count==20000000)
		{
			up_data(dht11[2],dht11[0]);
			count=0;
		}
	}	
}
int main(void)
{
	/* Initialize system and Initialize delay */
//	delay_init(168);
  SystemInit();
  TM_DELAY_Init();
	InitGPIOD(); Delay_us(1000000);
	TIM_PWM_Configuration();
	USARTx_Init(USART2, Pins_PD5PD6, 9600);	//115200 - Sim
	USARTx_Init(USART3, Pins_PC10PC11, 9600);	//115200 - Computer
	config_BH1750(); //PB6-PB7
	config_sms();
	config_http();
//	send_sms("Hello World","0984095710"); Delay_us(2000000);
	up_data(30,40); Delay_us(2000000);	
	GPIO_SetBits(GPIOD,GPIO_Pin_15);
//	config_finish =1;
  while(1)
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_14);		
		TIM4->CCR1 = pwm * 20000 / 100;
		get_Dht11(); get_bht1750();
		print_DHTandBHT();
	}		
}



