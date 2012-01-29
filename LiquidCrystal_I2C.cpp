// ---------------------------------------------------------------------------
// Created by Francisco Malpartida on 20/08/11.
// Copyright 2011 - Under creative commons license 3.0:
//        Attribution-ShareAlike CC BY-SA
//
// This software is furnished "as is", without technical support, and with no 
// warranty, express or implied, as to its usefulness for any purpose.
//
// Thread Safe: No
// Extendable: Yes
//
// @file LiquidCrystal_I2C.c
// This file implements a basic liquid crystal library that comes as standard
// in the Arduino SDK but using an I2C IO extension board.
// 
// @brief 
// This is a basic implementation of the LiquidCrystal library of the
// Arduino SDK. The original library has been reworked in such a way that 
// this class implements the all methods to command an LCD based
// on the Hitachi HD44780 and compatible chipsets using I2C extension
// backpacks such as the I2CLCDextraIO with the PCF8574* I2C IO Expander ASIC.
//
// The functionality provided by this class and its base class is identical
// to the original functionality of the Arduino LiquidCrystal library.
//
//
// This library is only compatible with Arduino's SDK version 1.0
//
//
// @author F. Malpartida - fmalpartida@gmail.com
// ---------------------------------------------------------------------------
#if (ARDUINO <  100)
#include <WProgram.h>
#else
#include <Arduino.h>
#endif
#include <inttypes.h>
#include "I2CIO.h"
#include "LiquidCrystal_I2C.h"



// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal_I2C constructor is called).
// A call to begin() will reinitialize the LCD.

// CONSTRUCTORS
// ---------------------------------------------------------------------------
LiquidCrystal_I2C::LiquidCrystal_I2C( uint8_t lcd_Addr )
{
   _Addr = lcd_Addr;
   
   _backlightval = LCD_NOBACKLIGHT;
   _En = EN;
   _Rw = RW;
   _Rs = RS;
   
   // Initialise default values data[0] pin 0, data[1] pin 1, ...
   for ( uint8_t i = 0; i < 4; i++ )
   {
      _data_pins[i] = ( 1 << i );
   }
}

LiquidCrystal_I2C::LiquidCrystal_I2C( uint8_t lcd_Addr, uint8_t En, uint8_t Rw,
                                     uint8_t Rs)
{
   _Addr = lcd_Addr;
   
   _backlightval = LCD_NOBACKLIGHT;
   _En = ( 1 << En );
   _Rw = ( 1 << Rw );
   _Rs = ( 1 << Rs );
   
   // Initialise default values data[0] pin 0, data[1] pin 1, ...
   for ( uint8_t i = 0; i < 4; i++ )
   {
      _data_pins[i] = ( 1 << i );
   }
}

LiquidCrystal_I2C::LiquidCrystal_I2C( uint8_t lcd_Addr, uint8_t En, uint8_t Rw,
                                     uint8_t Rs, uint8_t d0, uint8_t d1,
                                     uint8_t d2, uint8_t d3 )
{
   _Addr = lcd_Addr;
   
   _backlightval = LCD_NOBACKLIGHT;
   _En = ( 1 << En );
   _Rw = ( 1 << Rw );
   _Rs = ( 1 << Rs );
   
   // Initialise pin mapping
   _data_pins[0] = ( 1 << d0 );
   _data_pins[1] = ( 1 << d1 );
   _data_pins[2] = ( 1 << d2 );
   _data_pins[3] = ( 1 << d3 );
}

// PRIVATE METHODS
// ---------------------------------------------------------------------------

//
// init
int LiquidCrystal_I2C::init()
{
   int status = 0;
   
   // initialize the backpack IO expander
   // and display functions.
   // ------------------------------------------------------------------------
   if ( _i2cio.begin ( _Addr ) == 1 )
   {
      _i2cio.portMode ( OUTPUT );  // Set the entire IO extender to OUTPUT
      _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
      status = 1;
   }
   return ( status );
}


// PUBLIC METHODS
// ---------------------------------------------------------------------------

//
// begin
void LiquidCrystal_I2C::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) 
{
   
   init();     // Initialise the I2C expander interface
   
   if (lines > 1) 
   {
      _displayfunction |= LCD_2LINE;
   }
   _numlines = lines;
   _cols = cols;
   
   // for some 1 line displays you can select a 10 pixel high font
   if ((dotsize != 0) && (lines == 1)) {
      _displayfunction |= LCD_5x10DOTS;
   }
   
   // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
   // according to datasheet, we need at least 40ms after power rises above 2.7V
   // before sending commands. Arduino can turn on way before 4.5V so we'll wait 50
   delayMicroseconds(50000); 
   
   // Now we pull both RS and R/W low to begin commands
   expanderWrite ( _backlightval );   // reset expander and turn backlight off (Bit 8 =1)
   delay(1000);
   
   //put the LCD into 4 bit mode
   // this is according to the hitachi HD44780 datasheet
   // figure 24, pg 46
   
   // we start in 8bit mode, try to set 4 bit mode
   write4bits ( 0x03, LOW );
   delayMicroseconds(4500); // wait min 4.1ms
   
   // second try
   write4bits ( 0x03, LOW );
   delayMicroseconds(4500); // wait min 4.1ms
   
   // third go!
   write4bits ( 0x03, LOW ); 
   delayMicroseconds(150);
   
   // finally, set to 4-bit interface
   write4bits ( 0x02, LOW ); 
   
   
   // set # lines, font size, etc.
   command(LCD_FUNCTIONSET | _displayfunction);  
   
   // turn the display on with no cursor or blinking default
   _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
   display();
   
   // clear it off
   clear();
   
   // Initialize to default text direction (for roman languages)
   _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
   
   // set the entry mode
   command(LCD_ENTRYMODESET | _displaymode);
   
   home();
   
}


// User commands - users can expand this section
//----------------------------------------------------------------------------

// Turn the (optional) backlight off/on
void LiquidCrystal_I2C::noBacklight(void) 
{
   _backlightval=LCD_NOBACKLIGHT;
   expanderWrite(0);
}

void LiquidCrystal_I2C::backlight(void) 
{
   _backlightval=LCD_BACKLIGHT;
   expanderWrite(0);
}

// PRIVATE METHODS
// ---------------------------------------------------------------------------

// low level data pushing commands
//----------------------------------------------------------------------------

//
// send - write either command or data
void LiquidCrystal_I2C::send(uint8_t value, uint8_t mode) 
{
   // No need to use the delay routines since the time taken to write takes
   // longer that what is needed both for toggling and enable pin an to execute
   // the command.
   write4bits( (value >> 4), mode );
   write4bits( (value & 0x0F), mode);
}

//
// write4bits
void LiquidCrystal_I2C::write4bits ( uint8_t value, uint8_t mode ) 
{
   uint8_t pinMapValue = 0;
   
   // Map the value to LCD pin mapping
   // --------------------------------
   for ( uint8_t i = 0; i < 4; i++ )
   {
      if ( ( value & 0x1 ) == 1 )
      {
         pinMapValue |= _data_pins[i];
      }
      value = ( value >> 1 );
   }
   
   // Is it a command or data
   // -----------------------
   if ( mode == DATA )
   {
      mode = _Rs;
   }
   
   expanderWrite ( pinMapValue | mode );
   pulseEnable ( pinMapValue | mode );
}

//
// write4bits
void LiquidCrystal_I2C::pulseEnable (uint8_t _data)
{
   expanderWrite (_data | _En);   // En HIGH
   expanderWrite(_data & ~_En);   // En LOW
} 

//
// expanderWrite
void LiquidCrystal_I2C::expanderWrite(uint8_t _data)
{                                        
   _i2cio.write ( _data );
}




