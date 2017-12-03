	#include "I2C_LCD.h"
	#include "delay.h"

	LCD_I2C_TypeDef LCD_I2C_Structure;
	
	void LCDI2C_write(uint8_t value){
		LCDI2C_send(value, Rs);
	}	
	
	void LCDI2C_Init(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows)
	{
		LCD_I2C_Structure._Addr = lcd_Addr;
		LCD_I2C_Structure._cols = lcd_cols;
		LCD_I2C_Structure._rows = lcd_rows;
		LCD_I2C_Structure._backlightval = LCD_NOBACKLIGHT;
		LCDI2C_init_priv();		
	}
	void LCDI2C_init_priv(void)
	{
		TM_I2C_Init(LCD_I2C,LCD_I2C_PINSPACK,100000); // Wire.begin() Initialize I2C
		LCD_I2C_Structure._displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
		LCDI2C_begin(LCD_I2C_Structure._Addr, LCD_I2C_Structure._rows,0); 
	}
	void LCDI2C_begin(uint8_t cols, uint8_t lines, uint8_t dotsize)
	{
		if (lines > 1) {
			LCD_I2C_Structure._displayfunction |= LCD_2LINE;
		}
		LCD_I2C_Structure._numlines = lines;

		// for some 1 line displays you can select a 10 pixel high font
		if ((dotsize != 0) && (lines == 1)) {
			LCD_I2C_Structure._displayfunction |= LCD_5x10DOTS;
		}

		// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
		// according to datasheet, we need at least 40ms after power rises above 2.7V
		// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
		delay_ms(50); 
		
		// Now we pull both RS and R/W low to begin commands
		LCDI2C_expanderWrite(LCD_I2C_Structure._backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
		delay_ms(1000);

			//put the LCD into 4 bit mode
		// this is according to the hitachi HD44780 datasheet
		// figure 24, pg 46
		
			// we start in 8bit mode, try to set 4 bit mode
		 LCDI2C_write4bits(0x03 << 4);
		 delay_us(4500); // wait min 4.1ms
		 
		 // second try
		 LCDI2C_write4bits(0x03 << 4);
		 delay_us(4500); // wait min 4.1ms
		 
		 // third go!
		 LCDI2C_write4bits(0x03 << 4); 
		 delay_us(150);
		 
		 // finally, set to 4-bit interface
		 LCDI2C_write4bits(0x02 << 4); 


		// set # lines, font size, etc.
		LCDI2C_command(LCD_FUNCTIONSET | LCD_I2C_Structure._displayfunction);  
		
		// turn the display on with no cursor or blinking default
		LCD_I2C_Structure._displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
		LCDI2C_display();
		
		// clear it off
		LCDI2C_clear();
		
		// Initialize to default text direction (for roman languages)
		LCD_I2C_Structure._displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
		
		// set the entry mode
		LCDI2C_command(LCD_ENTRYMODESET | LCD_I2C_Structure._displaymode);
		
		LCDI2C_home();
	}
	void LCDI2C_clear(void)
	{
		LCDI2C_command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
		delay_us(2000);  // this command takes a long time!
	}
	void LCDI2C_home(void)
	{
		LCDI2C_command(LCD_RETURNHOME);  // set cursor position to zero
		delay_us(2000);  // this command takes a long time!
	}
	void LCDI2C_setCursor(uint8_t col, uint8_t row)
	{
		int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
		if ( row > LCD_I2C_Structure._numlines ) {
			row = LCD_I2C_Structure._numlines-1;    // we count rows starting w/0
		}
		LCDI2C_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
	}

	// Turn the display on/off (quickly)
	void LCDI2C_noDisplay(void)
	{
		LCD_I2C_Structure._displaycontrol &= ~LCD_DISPLAYON;
		LCDI2C_command(LCD_DISPLAYCONTROL | LCD_I2C_Structure._displaycontrol);
	}
	void LCDI2C_display(void)
	{
		LCD_I2C_Structure._displaycontrol |= LCD_DISPLAYON;
		LCDI2C_command(LCD_DISPLAYCONTROL | LCD_I2C_Structure._displaycontrol);
	}

	// Turns the underline cursor on/off
	void LCDI2C_noCursor(void)
	{
		LCD_I2C_Structure._displaycontrol &= ~LCD_CURSORON;
		LCDI2C_command(LCD_DISPLAYCONTROL | LCD_I2C_Structure._displaycontrol);
	}
	void LCDI2C_cursor(void)
	{
		LCD_I2C_Structure._displaycontrol |= LCD_CURSORON;
		LCDI2C_command(LCD_DISPLAYCONTROL | LCD_I2C_Structure._displaycontrol);
	}

	// Turn on and off the blinking cursor
	void LCDI2C_noBlink(void)
	{
		LCD_I2C_Structure._displaycontrol &= ~LCD_BLINKON;
		LCDI2C_command(LCD_DISPLAYCONTROL | LCD_I2C_Structure._displaycontrol);
	}
	void LCDI2C_blink(void)
	{
		LCD_I2C_Structure._displaycontrol |= LCD_BLINKON;
		LCDI2C_command(LCD_DISPLAYCONTROL | LCD_I2C_Structure._displaycontrol);
	}

	// These commands scroll the display without changing the RAM
	void LCDI2C_scrollDisplayLeft(void)
	{
		LCDI2C_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
	}
	void LCDI2C_scrollDisplayRight(void)
	{
		LCDI2C_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
	}

	// This is for text that flows Left to Right
	void LCDI2C_leftToRight(void)
	{
		LCD_I2C_Structure._displaymode |= LCD_ENTRYLEFT;
		LCDI2C_command(LCD_ENTRYMODESET | LCD_I2C_Structure._displaymode);
	}

	// This is for text that flows Right to Left
	void LCDI2C_rightToLeft(void) {
		LCD_I2C_Structure._displaymode &= ~LCD_ENTRYLEFT;
		LCDI2C_command(LCD_ENTRYMODESET | LCD_I2C_Structure._displaymode);
	}

	// This will 'right justify' text from the cursor
	void LCDI2C_autoscroll(void) {
		LCD_I2C_Structure._displaymode |= LCD_ENTRYSHIFTINCREMENT;
		LCDI2C_command(LCD_ENTRYMODESET | LCD_I2C_Structure._displaymode);
	}

	// This will 'left justify' text from the cursor
	void LCDI2C_noAutoscroll(void) {
		LCD_I2C_Structure._displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
		LCDI2C_command(LCD_ENTRYMODESET | LCD_I2C_Structure._displaymode);
	}

	// Allows us to fill the first 8 CGRAM locations
	// with custom characters
	void LCDI2C_createChar(uint8_t location, uint8_t charmap[]) {
		location &= 0x7; // we only have 8 locations 0-7
		LCDI2C_command(LCD_SETCGRAMADDR | (location << 3));
		for (int i=0; i<8; i++) {
			LCDI2C_write(charmap[i]);
		}
	}

	// Turn the (optional) backlight off/on
	void LCDI2C_noBacklight(void) {
		LCD_I2C_Structure._backlightval=LCD_NOBACKLIGHT;
		LCDI2C_expanderWrite(0);
	}

	void LCDI2C_backlight(void) {
		LCD_I2C_Structure._backlightval=LCD_BACKLIGHT;
		LCDI2C_expanderWrite(0);
	}

	/*********** mid level commands, for sending data/cmds */
	void LCDI2C_command(uint8_t value) {
		LCDI2C_send(value, 0);
	}
	// write either command or data
	void LCDI2C_send(uint8_t value, uint8_t mode)
	{
		uint8_t highnib = value&0xf0;
		uint8_t lownib =(value<<4)&0xf0;
		LCDI2C_write4bits((highnib)|mode);
		LCDI2C_write4bits((lownib)|mode);
	}
	void LCDI2C_write4bits(uint8_t value)
	{
		LCDI2C_expanderWrite(value);
		LCDI2C_pulseEnable(value);
	}
	void LCDI2C_expanderWrite(uint8_t _data)		// chua hieu ham nay
	{
		TM_I2C_Write(LCD_I2C, LCD_I2C_Structure._Addr, 0x00, (int)(_data) | LCD_I2C_Structure._backlightval);
	}
	void LCDI2C_pulseEnable(uint8_t _data){
		LCDI2C_expanderWrite(_data | En);	// En high
		delay_us(1);		// enable pulse must be >450ns

		LCDI2C_expanderWrite(_data & ~En);	// En low
		delay_us(50);		// commands need > 37us to settle
	}

	// Alias functions

	void LCDI2C_cursor_on(){
		LCDI2C_cursor();
	}

	void LCDI2C_cursor_off(){
		LCDI2C_noCursor();
	}

	void LCDI2C_blink_on(){
		LCDI2C_blink();
	}

	void LCDI2C_blink_off(){
		LCDI2C_noBlink();
	}

	void LCDI2C_load_custom_character(uint8_t char_num, uint8_t *rows){
			LCDI2C_createChar(char_num, rows);
	}

	void LCDI2C_setBacklight(uint8_t new_val){
		if(new_val){
			LCDI2C_backlight();		// turn backlight on
		}else{
			LCDI2C_noBacklight();		// turn backlight off
		}
	}

	void LCDI2C_write_String(char *Text)
		{
			char *c;
			c = Text;
			while((c != 0) && (*c != 0))
			{
				LCDI2C_send(*c,Rs);
				c++;
		}
	}
