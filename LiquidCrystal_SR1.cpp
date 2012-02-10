// ---------------------------------------------------------------------------
// Copyright 2011 - Under creative commons license 3.0:
//        Attribution-ShareAlike CC BY-SA
//
// This software is furnished "as is", without technical support, and with no 
// warranty, express or implied, as to its usefulness for any purpose.
//
// TODO: history, comments, etc...
// ---------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#if (ARDUINO <  100)
#include <WProgram.h>
#else
#include <Arduino.h>
#endif
#include "LiquidCrystal_SR1.h"

#include "FastIO.h"


// CONSTRUCTORS
// ---------------------------------------------------------------------------
// Assuming 1 line 8 pixel high font
LiquidCrystal_SR1::LiquidCrystal_SR1 ( uint8_t srdata, uint8_t srclock,
                                     uint8_t enable ) 
{
	init ( srdata, srclock, enable, 1, 0 );
}


// PRIVATE METHODS
// ---------------------------------------------------------------------------

//
// init
void LiquidCrystal_SR1::init( uint8_t srdata, uint8_t srclock, uint8_t enable,
                             uint8_t lines, uint8_t font )
{
   // Initialise private variables
   _two_wire    = 0;

   _srDataRegister = fio_pinToOutputRegister(srdata);
   _srDataBit = fio_pinToBit(srdata);
   _srClockRegister = fio_pinToOutputRegister(srclock);
   _srClockBit = fio_pinToBit(srclock);

   if (enable == TWO_WIRE)
   {
      _two_wire   = 1;
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

// PUBLIC METHODS
// ---------------------------------------------------------------------------


/************ low level data pushing commands **********/

// send
void LiquidCrystal_SR1::send(uint8_t value, uint8_t mode)
{
   uint8_t val1, val2;
   
   // If _two_wire - clear the shiftregister first.
   // ----------------------------------------------
   if ( _two_wire ) 
   {
	   // clear fast
	   fio_shiftOut(_srDataRegister, _srDataBit, _srClockRegister, _srClockBit);
   }
   
   // Divide byte in two nibbles (val1 and val2), include the RS signal
   // and format it for shiftregister output wiring to the LCD
   mode = mode ? SR_RS_BIT : 0; // RS bit; LOW: command.  HIGH: character.
   val1 = mode | SR_EN_BIT | ((value >> 1) & 0x78); // upper nibble
   val2 = mode | SR_EN_BIT | ((value << 3) & 0x78); // lower nibble
   
   fio_shiftOut(_srDataRegister, _srDataBit, _srClockRegister, _srClockBit, val1, MSBFIRST);
   
   // LCD ENABLE PULSE
   //
   // While this library is written with a shift register without an output
   // latch in mind, it can work in 3-wire mode with a shiftregister with a
   // latch. The shiftregister latch pin (STR, RCL or similar) is then
   // connected to the LCD enable pin. The LCD is (very likely) slower
   // to read the Enable pulse, and then reads the new contents of the SR.
   fio_digitalWrite_HIGH(_srEnableRegister, _srEnableBit);
   waitUsec( 1 );                 // enable pulse must be >450ns
   fio_digitalWrite_SWITCHTO(_srEnableRegister, _srEnableBit,LOW);

   // clear shiftregister
   // ---------------------------
   if ( _two_wire ) 
   {
	   // Clear fast
	   fio_shiftOut(_srDataRegister, _srDataBit, _srClockRegister, _srClockBit);
   }
   fio_shiftOut(_srDataRegister, _srDataBit, _srClockRegister, _srClockBit, val2, MSBFIRST);
   
   // LCD ENABLE PULSE, se above comment
   fio_digitalWrite_SWITCHTO(_srEnableRegister, _srEnableBit,HIGH);
   waitUsec( 1 );                 // enable pulse must be >450ns
   fio_digitalWrite_SWITCHTO(_srEnableRegister, _srEnableBit,LOW);
   waitUsec( 40 );                // commands need > 37us to settle
}

//
// write4bits
void LiquidCrystal_SR1::write4bits(uint8_t value)
{
   uint8_t val1;
   
   // clear shiftregister
   // --------------------------
   if ( _two_wire ) 
   {
	   // clear fast
	   fio_shiftOut(_srDataRegister, _srDataBit, _srClockRegister, _srClockBit);
   }
   fio_digitalWrite(_srEnableRegister, _srEnableBit, LOW);

   // Discard lower nibble
   // and format it for shiftregister output wiring to the LCD
   val1 = SR_EN_BIT | ((value >> 1) & 0x78);
   fio_shiftOut(_srDataRegister, _srDataBit, _srClockRegister, _srClockBit, val1, MSBFIRST);

   // LCD ENABLE PULSE
   //
   // While this library is written with a shift register without an output
   // latch in mind, it can work in 3-wire mode with a shiftregister with a
   // latch. The shiftregister latch pin (STR, RCL or similar) is then
   // connected to the LCD enable pin. The LCD is (very likely) slower
   // to read the Enable pulse, and then reads the new contents of the SR.
   
   fio_digitalWrite(_srEnableRegister, _srEnableBit, HIGH);

   waitUsec( 1 );                 // enable pulse must be >450ns
   fio_digitalWrite(_srEnableRegister, _srEnableBit, LOW);
   
   waitUsec( 40 );               // commands need > 37us to settle
}
