/**
 * LCD Library for STM32F4xx
 
 * |----------------------------------------------------------------------
 * | 
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 *
 * 	Version 1.0
 * 	- Added basic routines to set and get hours
 * 	Version 1.1
 * 	- Added 2 new functions to work with SQW/OUT pin on LCD
 * 		LCD_EnableOutputPin
 * 		LCD_DisableOutputPin
 * 	Version 1.2
 *	- 21.09.2014
 * 	- Added support for check if LCD module is connected to I2C on initialization
 * 		
 * Default pinout
 * 
 * 	LCD	STM32F4XX	DESCRIPTION
 *	VCC		5V			Positive voltage
 *	GND		GND			Ground
 *	SDA		PB7			I2C1, PinsPack 1
 *	SCL		PB6			I2C1, PinsPack 1
 *	
 * Change pinout in your defines.h file
 * 
 *	//Set your I2C, to see corresponding pins, look for TM I2C library
 * 	#define LCD_I2C				I2C1
 *	#define LCD_I2C_PINSPACK		I2C_PinsPack_1
 */
#ifndef LCD_H
#define LCD_H 120
/**
 * Library dependencies
 * - STM32F4xx
 * - STM32F4xx RCC
 * - STM32F4xx GPIO
 * - STM32F4xx I2C
 * - TM I2C
 * - defines.h
 */
/**
 * Includes
 */
#include "stm32f4xx.h"
#include "myiic.h"

//I2C settings for I2C library
#ifndef LCD_I2C
#define LCD_I2C					I2C1
#define LCD_I2C_PINSPACK			I2C_PinsPack_1
#endif

#ifndef LCD_I2C_CLOCK
#define LCD_I2C_CLOCK			100000
#endif

//I2C slave address for LCD
#define LCD_I2C_ADDR			0x7E

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04  // Enable bit
#define Rw 0x02  // Read/Write bit
#define Rs 0x01  // Register select bit

typedef struct
{
  uint8_t Addr;
  uint8_t displayfunction;
  uint8_t displaycontrol;
  uint8_t displaymode;
  uint8_t numlines;
  uint8_t cols;
  uint8_t rows;
  uint8_t backlightval;
} LiquidCrystal_I2C_Def;


/**
 * Result typedef
 *
 * Parameters:
 * 	- LCD_Result_Ok:
 * 		Everything OK
 * 	- LCD_Result_Error:
 * 		An error occured
 * 	- LCD_Result_DeviceNotConnected:
 *		Device is not connected
 */
typedef enum {
	LCD_Result_Ok = 0,
	LCD_Result_Error,
	LCD_Result_DeviceNotConnected
} LCD_Result_t;

/**
 * Initialize LCD RTC library
 *
 * No parameters
 *
 * No returns
 */
extern LCD_Result_t LCD_Init(void);


  void LCDI2C_write(uint8_t value);
  void LCDI2C_init(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows);
  void LCDI2C_begin(uint8_t cols, uint8_t rows);//, uint8_t charsize = LCD_5x8DOTS ); если у вас другой размер символов - поправьте тут и в описании функции
  void LCDI2C_clear(void);
  void LCDI2C_home(void);
  void LCDI2C_noDisplay(void);
  void LCDI2C_display(void);
  void LCDI2C_noBlink(void);
  void LCDI2C_blink(void);
  void LCDI2C_noCursor(void);
  void LCDI2C_cursor(void);
  void LCDI2C_scrollDisplayLeft(void);
  void LCDI2C_scrollDisplayRight(void);
  void LCDI2C_printLeft(void);
  void LCDI2C_printRight(void);
  void LCDI2C_leftToRight(void);
  void LCDI2C_rightToLeft(void);
  void LCDI2C_shiftIncrement(void);
  void LCDI2C_shiftDecrement(void);
  void LCDI2C_noBacklight(void);
  void LCDI2C_backlight(void);
  void LCDI2C_autoscroll(void);
  void LCDI2C_noAutoscroll(void);
  void LCDI2C_createChar(uint8_t location, uint8_t charmap[]);
  void LCDI2C_setCursor(uint8_t col, uint8_t row);
  void LCDI2C_write_String(char* str);
  void LCDI2C_command(uint8_t value);
//  void LCDI2C_init(void);

////compatibility API function aliases
void LCDI2C_blink_on(void);						// alias for blink(void)
void LCDI2C_blink_off(void);       					// alias for noBlink(void)
void LCDI2C_cursor_on(void);      	 					// alias for cursor(void)
void LCDI2C_cursor_off(void);      					// alias for noCursor(void)
void LCDI2C_setBacklight(uint8_t new_val);				// alias for backlight(void) and nobacklight(void)
void LCDI2C_load_custom_character(uint8_t char_num, uint8_t *rows);	// alias for createChar(void)
void LCDI2C_printstr(const char[]);

//void LCDI2C_init_priv(void);
void LCDI2C_send(uint8_t, uint8_t);
void LCDI2C_write4bits(uint8_t);
void LCDI2C_expanderWrite(uint8_t);
void LCDI2C_pulseEnable(uint8_t);

#endif

