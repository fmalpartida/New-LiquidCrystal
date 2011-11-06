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
// @file LiquidCrystal_SR.h
//  Connects an LCD using 2 or 3 pins from the Arduino, via an 8-bit 
// ShiftRegister (SR from now on).
// 
// @brief 
// This is a port of the ShiftRegLCD library from raron and ported to the
// LCD library.
//
// The functionality provided by this class and its base class is identical
// to the original functionality of the Arduino LiquidCrystal library and can
// be used as such.
//
// Modified to work serially with the shiftOut() function, an 8-bit shiftregister 
// (SR) and an LCD in 4-bit mode.
//
// Shiftregister connection description (NEW as of 2009.07.27)
//
// Bit  #0 - N/C - not connected, used to hold a zero
// Bits #1 - N/C
// Bit  #2 - connects to RS (Register Select) on the LCD
// Bits #3 - #6 from SR connects to LCD data inputs D4 - D7.
// Bit  #7 - is used to enabling the enable-puls (via a diode-resistor AND "gate")
//
// 2 or 3 Pins required from the Arduino for Data, Clock, and Enable (optional). 
// If not using Enable, the Data pin is used for the enable signal by defining 
// the same pin for Enable as for Data. Data and Clock outputs/pins goes to the 
// shiftregister.
// LCD RW-pin hardwired to LOW (only writing to LCD). Busy Flag (BF, data bit D7) 
// is not read.
//
// Any shift register should do. I used 74LS164, for the reason that's what I 
// had at hand.
//
//  Original project homepage: http://code.google.com/p/arduinoshiftreglcd/
//
// History
// 2011.10.29  fmalpartida - adaption of the library to the LCD class hierarchy.
// 2009.05.23  raron - but; based mostly (as in almost verbatim) on the 
//             "official" LiquidCrystal library.
// 2009.07.23  Incorporated some proper initialization routines
//             inspired (lets say copy-paste-tweaked) from LiquidCrystal library 
//             improvements from LadyAda
// 2009.07.25  raron - Fixed comments. I really messed up the comments before 
//             posting this, so I had to fix it.
//             Also renamed a function, but no improvements or functional changes.
// 2009.07.27  Thanks to an excellent suggestion from mircho at the Arduiono 
//             playgrond forum, the number of wires now required is only two!
// 2009.07.28  Mircho / raron - a new modification to the schematics, and a more 
//             streamlined interface
// 2009.07.30  raron - minor corrections to the comments. Fixed keyword highlights. 
//             Fixed timing to datasheet safe.
// 2011.07.02  Fixed a minor flaw in setCursor function. No functional change, 
//             just a bit more memory efficient.
//             Thanks to CapnBry (from google code and github) who noticed it. 
//             URL to his version of shiftregLCD:
//https://github.com/CapnBry/HeaterMeter/commit/c6beba1b46b092ab0b33bcbd0a30a201fd1f28c1
//
//
// This library is only compatible with Arduino's SDK version 1.0
//
//
// @author F. Malpartida - fmalpartida@gmail.com
// ---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#if (ARDUINO <  100)
#include <WProgram.h>
#else
#include <Arduino.h>
#endif
#include <LiquidCrystal_SR.h>

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
// LiquidCrystal constructor is called).
// A call to begin() will reinitialize the LCD.


// CONSTRUCTORS
// ---------------------------------------------------------------------------
// Assuming 1 line 8 pixel high font
LiquidCrystal_SR::LiquidCrystal_SR ( uint8_t srdata, uint8_t srclock, 
                                     uint8_t enable ) 
{
	init ( srdata, srclock, enable, 1, 0 );
}
// Set nr. of lines, assume 8 pixel high font
LiquidCrystal_SR::LiquidCrystal_SR ( uint8_t srdata, uint8_t srclock, 
                                     uint8_t enable, uint8_t lines ) 
{
	init ( srdata, srclock, enable, lines, 0 );
}

// Set nr. of lines and font
LiquidCrystal_SR::LiquidCrystal_SR ( uint8_t srdata, uint8_t srclock, 
                                     uint8_t enable, uint8_t lines, 
                                     uint8_t font ) 
{
	init ( srdata, srclock, enable, lines, font );
}


// PRIVATE METHODS
// ---------------------------------------------------------------------------

//
// init
void LiquidCrystal_SR::init( uint8_t srdata, uint8_t srclock, uint8_t enable, 
                             uint8_t lines, uint8_t font )
{
   // Initialise private variables
   _two_wire    = 0;
   _srdata_pin  = srdata; 
   _srclock_pin = srclock; 
   _enable_pin  = enable;
   
   if (enable == TWO_WIRE)
   {
      _enable_pin = _srdata_pin;
      _two_wire   = 1;
   }
   
   // Configure control pins as outputs
   // ------------------------------------------------------------------------
   pinMode(_srclock_pin, OUTPUT);
   pinMode(_srdata_pin, OUTPUT);
   pinMode(_enable_pin, OUTPUT);
   
   _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x10DOTS;
}

// PUBLIC METHODS
// ---------------------------------------------------------------------------

//
// begin
void LiquidCrystal_SR::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) 
{
   
   if (lines > 1) 
   {
      _displayfunction |= LCD_2LINE;
   }

   _numlines = lines;
   
   // for some 1 line displays you can select a 10 pixel high font
   // ------------------------------------------------------------
   if ((dotsize != 0) && (lines == 1)) 
   {
      _displayfunction |= LCD_5x10DOTS;
   }
   
   // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
   // according to datasheet, we need at least 40ms after power rises above 2.7V
   // before sending commands. Arduino can turn on way before 4.5V so we'll wait 
   // 50
   // ---------------------------------------------------------------------------
   delayMicroseconds(50000);
   init4bits(LCD_FUNCTIONSET | LCD_8BITMODE);
   delayMicroseconds(4500);  // wait more than 4.1ms
   
   // Second try
   init4bits(LCD_FUNCTIONSET | LCD_8BITMODE);
   delayMicroseconds(150);
   // Third go
   init4bits(LCD_FUNCTIONSET | LCD_8BITMODE);
   
   // And finally, set to 4-bit interface
   init4bits(LCD_FUNCTIONSET | LCD_4BITMODE);
   
   // Set # lines, font size, etc.
   command(LCD_FUNCTIONSET | _displayfunction);
   // Turn the display on with no cursor or blinking default
   _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
   display();
   // Clear it off
   clear();
   // Initialize to default text direction (for romance languages)
   _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
   // set the entry mode
   command(LCD_ENTRYMODESET | _displaymode);
   home();
   
}

/************ low level data pushing commands **********/

// send
void LiquidCrystal_SR::send(uint8_t value, uint8_t mode) 
{
   uint8_t val1, val2;
   
   // If _two_wire - clear the shiftregister first.
   // ----------------------------------------------
   if ( _two_wire ) 
   {
      shiftOut ( _srdata_pin, _srclock_pin, MSBFIRST, 0x00 );
   }
   digitalWrite( _enable_pin, LOW );
   
   mode = mode ? SR_RS_BIT : 0; // RS bit; LOW: command.  HIGH: character.
   val1 = mode | SR_EN_BIT | ((value >> 1) & 0x78); // upper nibble
   val2 = mode | SR_EN_BIT | ((value << 3) & 0x78); // lower nibble
   
   shiftOut ( _srdata_pin, _srclock_pin, MSBFIRST, val1 );
   
   digitalWrite( _enable_pin, HIGH );
   delayMicroseconds(1);                 // enable pulse must be >450ns
   digitalWrite( _enable_pin, LOW );

   // clear shiftregister
   // ---------------------------
   if ( _two_wire ) 
   {
      shiftOut ( _srdata_pin, _srclock_pin, MSBFIRST, 0x00 ); 
   }
   shiftOut ( _srdata_pin, _srclock_pin, MSBFIRST, val2 );
   
   digitalWrite( _enable_pin, HIGH );
   delayMicroseconds(1);                 // enable pulse must be >450ns
   digitalWrite( _enable_pin, LOW );
   delayMicroseconds(40);                // commands need > 37us to settle
}

//
// init4bits
void LiquidCrystal_SR::init4bits(uint8_t value) 
{
   uint8_t val1;
   
   // clear shiftregister
   // --------------------------
   if ( _two_wire ) 
   {
      shiftOut ( _srdata_pin, _srclock_pin, MSBFIRST, 0x00 ); 
   }
   digitalWrite( _enable_pin, LOW );
   
   val1 = SR_EN_BIT | ((value >> 1) & 0x78);
   shiftOut ( _srdata_pin, _srclock_pin, MSBFIRST, val1 );
   
   digitalWrite( _enable_pin, HIGH );
   delayMicroseconds(1);                 // enable pulse must be >450ns
   digitalWrite( _enable_pin, LOW );
   
   delayMicroseconds(40);               // commands need > 37us to settle
}



