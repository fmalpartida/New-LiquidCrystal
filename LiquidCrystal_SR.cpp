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
// Modified to work serially with the shiftOut() function, an 8-bit
// unlatched, no-tristate, unidirectional SIPO (Serial-In-Parallel-Out)
// shift register (IE a very simple SR), and an LCD in 4-bit mode.
// Any such shift register should do (pref. 74LS family IC's for 2-wire).
// I used 74LS164, for the reason that's what I had at hand.
//
// Connection description:
//
// SR output:
// Bit  #0   - N/C - not connected, used to hold a zero
// Bit  #1   - N/C
// Bit  #2   - connects to RS (Register Select) on the LCD
// Bits #3-6 - connects to LCD data inputs D4 - D7.
// Bit  #7   - enables the LCD enable-puls (via the diode-resistor AND "gate")
//
// 2 or 3 Pins required from the Arduino for Data, Clock and (optional) Enable
// If not using Enable, the Data pin is used for the enable signal by defining
// the same pin for Enable as for Data. Data and Clock outputs/pins goes to
// the shiftregister.
// LCD RW-pin hardwired to LOW (only writing to LCD).
// Busy Flag (BF, data bit D7) is not read.
//
//  Original project homepage: http://code.google.com/p/arduinoshiftreglcd/
//
//
// History
// 2012.01.16  Florian Fida - faster digitalWrite/shiftOut
// 2011.10.29  fmalpartida - adaption of the library to the LCD class hierarchy.
// 2011.07.02  Fixed a minor flaw in setCursor function. No functional change, 
//             just a bit more memory efficient.
//             Thanks to CapnBry (from google code and github) who noticed it.
//             URL to his version of shiftregLCD:
//             https://github.com/CapnBry/HeaterMeter/commit/c6beba1b46b092ab0b33bcbd0a30a201fd1f28c1
// 2009.07.30  raron - minor corrections to the comments.
//             Fixed timing to datasheet safe. Fixed keyword highlights.
// 2009.07.28  Mircho / raron - a new modification to the schematics, and a
//             more streamlined interface
// 2009.07.27  Thanks to an excellent suggestion from mircho at the Arduino
//             playgrond forum, the number of wires now required is only two!
// 2009.07.25  raron - Fixed comments. I really messed up the comments before 
//             posting this, so I had to fix it.
//             Renamed a function, but no improvements or functional changes.
// 2009.07.23  Incorporated some proper initialization routines
//             inspired (lets say copy-paste-tweaked) from LiquidCrystal
//             library improvements from LadyAda.
// 2009.05.23  raron - first version, but based mostly (as in almost verbatim)
//             on the "official" LiquidCrystal library.
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
#include "LiquidCrystal_SR.h"

#include "FastIO.h"


// CONSTRUCTORS
// ---------------------------------------------------------------------------
// Assuming 1 line 8 pixel high font
LiquidCrystal_SR::LiquidCrystal_SR ( uint8_t srdata, uint8_t srclock, 
                                     uint8_t enable ) 
{
	init ( srdata, srclock, enable, 1, 0 );
}


// PRIVATE METHODS
// ---------------------------------------------------------------------------

//
// init
void LiquidCrystal_SR::init(uint8_t srdata, uint8_t srclock, uint8_t enable, uint8_t lines, uint8_t font)
{
   // Initialise private variables
   _two_wire = 0;

   _srDataRegister = fio_pinToOutputRegister(srdata);
   _srDataBit = fio_pinToBit(srdata);
   _srClockRegister = fio_pinToOutputRegister(srclock);
   _srClockBit = fio_pinToBit(srclock);

   if (enable == TWO_WIRE)
   {
      _two_wire = 1;
      _srEnableRegister = _srDataRegister;
      _srEnableBit = _srDataBit;
   }
   else
   {
      _srEnableRegister = fio_pinToOutputRegister(enable);
      _srEnableBit = fio_pinToBit(enable);
   }

   // Configure control pins as outputs
   // ------------------------------------------------------------------------

   _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x10DOTS;
}

// shiftIt
void LiquidCrystal_SR::shiftIt(uint8_t val)
{
   if (_two_wire)
   {
      // Clear to get Enable LOW
      fio_shiftOut(_srDataRegister, _srDataBit, _srClockRegister, _srClockBit);
   }
   fio_shiftOut(_srDataRegister, _srDataBit, _srClockRegister, _srClockBit, val, MSBFIRST);

   // LCD ENABLE PULSE
   //
   // While this library is written with a shift register without an output
   // latch in mind, it can work in 3-wire mode with a shiftregister with a
   // latch. The shiftregister latch pin (STR, RCL or similar) is then
   // connected to the LCD enable pin. The LCD is (very likely) slower
   // to read the Enable pulse, and then reads the new contents of the SR.

   fio_digitalWrite_HIGH(_srEnableRegister, _srEnableBit);
   waitUsec(1);                 // enable pulse must be >450ns
   fio_digitalWrite_SWITCHTO(_srEnableRegister, _srEnableBit, LOW);
   waitUsec(40);                // commands need > 37us to settle
}

// PUBLIC METHODS
// ---------------------------------------------------------------------------


/************ low level data pushing commands **********/

// send
void LiquidCrystal_SR::send(uint8_t value, uint8_t mode)
{
   // Divide byte in two nibbles include the RS signal
   // and format it for shiftregister output wiring to the LCD
   mode = mode ? SR_RS_BIT : 0; // RS bit; LOW: command.  HIGH: character.
   shiftIt(mode | SR_EN_BIT | ((value >> 1) & 0x78)); // upper nibble
   shiftIt(mode | SR_EN_BIT | ((value << 3) & 0x78)); // lower nibble
}

//
// setBacklight
void LiquidCrystal_SR::setBacklight ( uint8_t mode ) 
{ }


void LiquidCrystal_SR::setBacklightPin ( uint8_t pin )
{ }
