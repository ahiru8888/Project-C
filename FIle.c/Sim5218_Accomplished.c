#include <stm32f4xx.h>
#include "usart.h"	
#include "delay.h"	
#include "string.h"
#include <stdlib.h>
#include <stdio.h>
#include "adc1.h"
#include "defines.h"
#include "tm_stm32f4_i2c.h"


int answer;
uint8_t sms=0;
const int buffer_size=300;
uint8_t array=0;
char buffer[buffer_size];
char ON1[]="ON1";
char ON2[]="ON2";
char OFF1[]="OFF1";
char OFF2[]="OFF2";

/*****	HTTP *****/
char url[] = "api.thingspeak.com";
int port = 80;
char request[ ]="GET https://api.thingspeak.com/update?api_key=1S9BBY02QEAYG2LT&field1=%d\r\nHost: thingspeak.com\r\nContent-Length: 0\r\n\r\n";

void InitGPIOD(void);

uint8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout)
{
	uint8_t x=0,  answer=0;
	uint8_t c;
  char response[100];
	
	memset(response, '\0', 100);    // Initialize the string
	delay_ms(100);	
	//while(!(USART_BufferEmpty(USART2))) {USART_ClearBuffer(USART2);} // Clean the input buffer
	while( USART_Getc(USART2) > 0) ; 
	USART_Puts(USART2,ATcommand);
	x=0;
	do
	{
		delay_ms(1);
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

void http_protocol(uint8_t data)
{
	// request the url
	char buff[200], buff_data[400];
	sprintf(buff, "AT+CHTTPACT= \"%s\",%d\r\n", url, port);
	 do { answer = sendATcommand(buff,"+CHTTPACT: REQUEST",60000); }
	 while (answer==0);
	if(answer==1)
	{
		delay_ms(10);
		sprintf(buff_data,request,data);
		USART_Puts(USART2,buff_data); delay_ms(10);
		USART_Puts(USART2,"\x1A"); // Send 0x1A
	}
}

void power_on(void)
{
	uint8_t answer=0;
	answer = sendATcommand("AT\r\n","OK",2000);
	if(answer==0)
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_12); GPIO_ResetBits(GPIOD,GPIO_Pin_13);
		GPIO_SetBits(GPIOD,GPIO_Pin_8); delay_ms(3000);
		GPIO_ResetBits(GPIOD,GPIO_Pin_8);	
//				delay_ms(2000);
		while(answer==0) {answer = sendATcommand("AT\r\n","OK",2000);}
		USART_Puts(USART3,"Starting . . .\r\n");
		delay_ms(10000);
	}
	if(answer==1)
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_13); GPIO_ResetBits(GPIOD,GPIO_Pin_12);
		USART_Puts(USART3,"Start success\r\n");
	}
}

void send_sms(char* sms_text,char* phone_number)
{
//char phone_number[] = "0984095710";
	char buffer[20];
	sprintf(buffer,"AT+CMGS=\"%s\"\r\n", phone_number);	
	 do { answer = sendATcommand(buffer,">",2000);}
	 while(answer==0);    // Send the SMS number 
	if (answer == 1)
	{
		USART_Puts(USART2,sms_text);	delay_ms(10);
		USART_Puts(USART2,"\x1A");	// Send 0x1A
		answer=0;
	}
}

void config_sms(void)
{
	sendATcommand("AT+CMGF=1\r\n","OK",2000);					/* Config SMS text */
	sendATcommand("AT+CPMS=\"SM\"\r\n","OK",2000);    /* Config SMS memory */
}

void del_sms(void)
{
		sendATcommand("AT+CMGD=,4\r\n","OK",2000);
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
		del_sms();
	}
	else {USART_Puts(USART3,"Error");}
}

void Taikhoanchinh(void)
{
do { answer = sendATcommand("AT+CUSD=1","OK",5000);} while(answer==0);
do { answer = sendATcommand("ATD*101#","OK",5000);} while(answer==0);
delay_ms(8000);
}

void test(void)
{
	char buf1[100],buf2[100];
	
	if(USART_Gets(USART3,buf1,100))
	{
	  USART_Puts(USART2,buf1);
		memset(buf1, '\0', 100);
	}
//	delay_ms(1000);
	if(USART_Gets(USART2,buf2,100))
	{
	  USART_Puts(USART3,buf2);
		memset(buf2, '\0', 100);
	}
	
}
int main(void)
{
	SystemInit();	delay_init(168);	
	USARTx_Init(USART2, Pins_PD5PD6, 9600);	//115200 - Sim
	USARTx_Init(USART3, Pins_PC10PC11, 9600);	//115200 - Computer
	InitGPIOD();	delay_ms(3000);
	GPIO_SetBits(GPIOD,GPIO_Pin_14);	
	power_on();	delay_ms(5000); 
	config_sms();	delay_ms(5000);
	send_sms("Over the moon","0984095710"); delay_ms(5000);
	GPIO_SetBits(GPIOD,GPIO_Pin_15);
	uint8_t bien = 35; http_protocol(bien);
	GPIO_ResetBits(GPIOD,GPIO_Pin_15);
  while(1)
	{
//		receive_sms();
//		delay_ms(2000);
//		sendATcommand("AT+CUSD=1","OK",5000);	delay_ms(2000);	
//		sendATcommand("ATD*101#;","OK",5000);	delay_ms(2000);
		test();
	}		
}

void InitGPIOD(void)		
{
	GPIO_InitTypeDef GPIO_InitStruct; 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13|GPIO_Pin_12|GPIO_Pin_8 ; 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 	
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; 	
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL; 
	GPIO_Init(GPIOD, &GPIO_InitStruct);

	GPIO_SetBits(GPIOD,GPIO_Pin_8);
}
