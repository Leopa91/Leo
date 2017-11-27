#include "ch.h"
#include "hal.h"
#include "LiquidCrystal.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "chsprintf.h"


#define  LOW 0
#define  HIGH  1
#define  RS 6   // pin 4
#define  RW 5 
#define  EP 4  // pin 6
#define  D4 3   // pin 11   
#define  D5 2   // pin 12   
#define  D6 1  // pin 13 
#define  D7 0  // pin 14 


void LCDInit(void){
// only 4 bit mode

     _data_pins[0] = D4;
     _data_pins[1] = D5;
     _data_pins[2] = D6;
     _data_pins[3] = D7;

//  allways 4 BIT MODE
palSetPadMode(GPIOD, RW , PAL_MODE_OUTPUT_PUSHPULL); //RS
/////////////     
palSetPadMode(GPIOD, RS , PAL_MODE_OUTPUT_PUSHPULL); //RS
//we can save 1 pin by not using RW.
palSetPadMode(GPIOD, EP , PAL_MODE_OUTPUT_PUSHPULL); //EP

_displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
// configures data_pin
palSetPadMode(GPIOD, _data_pins[0] , PAL_MODE_OUTPUT_PUSHPULL);
palSetPadMode(GPIOD, _data_pins[1] , PAL_MODE_OUTPUT_PUSHPULL);
palSetPadMode(GPIOD, _data_pins[2] , PAL_MODE_OUTPUT_PUSHPULL);
palSetPadMode(GPIOD, _data_pins[3] , PAL_MODE_OUTPUT_PUSHPULL);

}

void LCDBegin(uint8_t cols, uint8_t lines)
{
   if (lines > 1) {
       _displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
     }

  _numlines = lines;
  _currline = 0;

  // for some 1 line displays you can select a 10 pixel high font
//   if ((dotsize != 0) && (lines == 1)) {
//     _displayfunction |= LCD_5x10DOTS;
//   }

    //for some 1 line displays you can select a 10 pixel high font
    //SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    //according to datasheet, we need at least 40ms after power rises above 2.7V
    //before sending commands.
   chThdSleepMilliseconds(50);
    // Now we pull RS low to begin commands
   palClearPad(GPIOD, RW);
   palClearPad(GPIOD, RS);
   palClearPad(GPIOD, EP);

    //put the LCD into 4 bit or 8 bit mode
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46
    // we start in 8bit mode, try to set 4 bit mode
   LCDWrite4bits(0x03);
   chThdSleepMicroseconds(4500);
    // second try
   LCDWrite4bits(0x03);
   chThdSleepMicroseconds(4500);
    // third go!
   LCDWrite4bits(0x03);
    chThdSleepMicroseconds(150);
    // finally, set to 4-bit interface
    LCDWrite4bits(0x02);
    // finally, set # lines, font size, etc.
    


    LCDCommand(LCD_FUNCTIONSET | _displayfunction);
    


    // turn the display on with no cursor or blinking default
    _displaycontrol = LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKOFF;
    LCDDisplay();
    // clear it off
    LCDClear();
    // Initialize to default text direction (for romance languages)
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    // set the entry mode
    LCDCommand(LCD_ENTRYMODESET | _displaymode);
}

/********** high level commands, for the user! */
void LCDClear(void)
{
  LCDCommand(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  chThdSleepMicroseconds(2000);  // this command takes a long time!
}

void LCDHome(void)
{
  LCDCommand(LCD_RETURNHOME);    // set cursor position to zero
  chThdSleepMicroseconds(2000);  // this command takes a long time!
}

void LCDSetCursor(uint8_t col, uint8_t row)
{
  int row_offsets[4] = { 0x00, 0x40, 0x08, 0x54 };
  // if ( row > _numlines ) {
  //      row = _numlines-1;    // we count rows starting w/0
  // }
	
  //LCDCommand(LCD_SETDDRAMADDR | ( row_offsets[row]));
  LCDCommand(LCD_SETDDRAMADDR | (row_offsets[row]+col));
 
  
}

// Turn the display on/off (quickly)
void LCDNoDisplay(void)
{
  _displaycontrol &= ~LCD_DISPLAYON;
  LCDCommand(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCDDisplay(void)
{
  _displaycontrol |= LCD_DISPLAYON;
  LCDCommand(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LCDNoCursor(void)
{
  _displaycontrol &= ~LCD_CURSORON;
  LCDCommand(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCDCursor(void)
{
  _displaycontrol |= LCD_CURSORON;
  LCDCommand(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LCDNoBlink(void)
{
  _displaycontrol &= ~LCD_BLINKON;
  LCDCommand(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCDBlink(void)
{
  _displaycontrol |= LCD_BLINKON;
  LCDCommand(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LCDScrollDisplayLeft(void)
{
  LCDCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LCDScrollDisplayRight(void)
{
  LCDCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LCDLeftToRight(void)
{
  _displaymode |= LCD_ENTRYLEFT;
  LCDCommand(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LCDRightToLeft(void)
{
  _displaymode &= ~LCD_ENTRYLEFT;
  LCDCommand(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LCDAutoscroll(void)
{
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  LCDCommand(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LCDNoAutoscroll(void)
{
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  LCDCommand(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LCDCreateChar(uint8_t location, uint8_t charmap[])
{ uint8_t i;
  location &= 0x7; // we only have 8 locations 0-7
  LCDCommand(LCD_SETCGRAMADDR | (location << 3));
  for (i=0; i<8; i++) {
  LCDWrite(charmap[i]);
  }
}

/*********** mid level commands, for sending data/cmds */

inline void LCDCommand(uint8_t value)
{
  LCDSend(value, LOW);
}

inline void LCDWrite(uint8_t value)
{
  LCDSend(value, HIGH);
}

inline void LCDPrintRow(uint8_t row_number ,uint8_t row_string [])
{
   uint8_t i;
    LCDSetCursor(0x00, row_number);
    for (i=0; i<16; i++)
    {
    LCDSend(row_string[i], HIGH);
    }
}

inline void LCDPrintRow2(uint8_t row_number ,uint8_t row_string [])
{
   uint8_t i;
    LCDSetCursor(0x40, row_number);
    for (i=0; i<16; i++)
    {
    LCDSend(row_string[i], HIGH);
    }
}



/************ low level data pushing commands **********/

// write either command or data, with automatic 4-bit selection
void LCDSend(uint8_t value, uint8_t mode)
{
   palWritePad(GPIOD, RW, 0);
   palWritePad(GPIOD, RS, mode);
   LCDWrite4bits(value>>4);
   LCDWrite4bits(value);
}

void LCDPulseEnable(void)
{
   palWritePad(GPIOD, EP, LOW);
   chThdSleepMicroseconds(100);
   palWritePad(GPIOD, EP, HIGH);
   chThdSleepMicroseconds(100);
   palWritePad(GPIOD, EP, LOW);
   chThdSleepMicroseconds(100);
}

void LCDWrite4bits(uint8_t value)
{
      //palSetPadMode(GPIOD, _data_pins[i] , PAL_MODE_OUTPUT_PUSHPULL);
      palWritePad(GPIOD, _data_pins[0], (value >> 0) & 0x01);
      palWritePad(GPIOD, _data_pins[1], (value >> 1) & 0x01);
      palWritePad(GPIOD, _data_pins[2], (value >> 2) & 0x01);
      palWritePad(GPIOD, _data_pins[3], (value >> 3) & 0x01);
      LCDPulseEnable();
}


