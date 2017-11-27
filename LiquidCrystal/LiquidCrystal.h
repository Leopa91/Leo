#ifndef LiquidCrystal_h
#define LiquidCrystal_h


#include "ch.h"
#include "hal.h"


 
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
//#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x80
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x40
#define LCD_5x8DOTS 0x00

uint8_t _data_pins[4];
uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;
uint8_t _initialized;
uint8_t _numlines,_currline;
  


void LCDInit(void);
void LCDBegin(uint8_t cols, uint8_t lines);
void LCDClear(void);
void LCDHome(void);
void LCDSetCursor(uint8_t col, uint8_t row);
void LCDNoDisplay(void);
void LCDDisplay(void);
void LCDNoCursor(void);
void LCDCursor(void);
void LCDNoBlink(void);
void LCDBlink(void);
void LCDScrollDisplayLeft(void);
void LCDScrollDisplayRight(void);
void LCDLeftToRight(void);
void LCDRightToLeft(void);
void LCDAutoscroll(void);
void LCDNoAutoscroll(void);
void LCDCreateChar(uint8_t location, uint8_t charmap[]);
void LCDCommand(uint8_t value);
void LCDWrite(uint8_t value);
void LCDPrintRow(uint8_t row_number ,uint8_t row_string []);
void LCDPrintRow2(uint8_t row_number ,uint8_t row_string []);
void LCDSend(uint8_t value, uint8_t mode);
void LCDPulseEnable(void);
void LCDWrite4bits(uint8_t value);

#endif
