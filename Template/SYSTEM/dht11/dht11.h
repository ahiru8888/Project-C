#ifndef DHT11_H
#define DHT11_H 

#include "stm32f4xx.h"
void output_GPIOD(void);
void input_GPIOD(void);
void start_signal(void);
unsigned char get_data(void);
uint8_t Humd_int(void);
uint8_t Humd_float(void);
uint8_t Temp_int(void);
uint8_t Temp_float(void);
#endif
