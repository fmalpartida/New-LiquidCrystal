// ---------------------------------------------------------------------------
// Originally Created by Francisco Malpartida on 2011/08/20.
// Copyright 2011 - Under creative commons license 3.0:
//        Attribution-ShareAlike CC BY-SA
//
// This software is furnished "as is", without technical support, and with no 
// warranty, express or implied, as to its usefulness for any purpose.
//
// 2012/01/21 - Marc MERLIN
// This library, LiquidCrystal_SR_LCD3, was forked off LiquidCrystal_SR which
// used a different wiring than the Pebble and Pebblev2 (just released by
// freetronics in the arduino miniconf as part of linux.conf.au 2012) and
// therefore this code organizes the output data differently.
//
// Upstream source for this module is
// https://github.com/marcmerlin/NewLiquidCrystal
//
// Thread Safe: No
// Extendable: Yes
//
// @file LiquidCrystal_SR_LCD3.h
//
//  Connects an LCD using 3 pins from the Arduino, via an 8-bit 
//  ShiftRegister (SR from now on).
//  The original port source for this module is https://github.com/marcmerlin/NewLiquidCrystal
//  The 'FastIO' merge has madethis code 4 times faster.
// 
// @brief 
// This is a port of the ShiftRegLCD library from raron and ported to the
// LCD library.
//
// The functionality provided by this class and its base class is identical
// to the original functionality of the Arduino LiquidCrystal library and can
// be used as such.
//
// Pinout for this code is used by derivatives of the original LCD3Wire page:
// http://www.arduino.cc/playground/Code/LCD3wires
//
// This includes the LCA (linux.conf.au) Arduino Miniconf Pebble: 
// http://shieldlist.org/luke-weston/pebble
// https://github.com/lukeweston/Pebble
//
// It also includes the Pebble v2:
// http://www.arduinominiconf.org/index.php/Pebble_V2.0_Instructions
// http://www.freetronics.com/pages/pebble-v2
// https://github.com/lukeweston/pebble20/blob/master/README.md
// https://github.com/lukeweston/pebble20/blob/master/pebble-sch.pdf
//
// Shiftregister connection description:
// MC14094 input:  Arduino digital pin 2=Clock, pin 3=Data, pin 4=Strobe
// MC14094 output: Q8=DB4, Q7=DB5, Q6=DB6, Q5=DB7, Q4=E, Q3=RW, Q2=RS, Q1=None
//
//   +--------------------------------------------+
//   |    Arduino (ATMega 168 or 328)             |
//   |    D02           D03           D04         |
//   +----+-------------+-------------+-----------+
//        |4            |5            |6
//        |1            |2            |3
//   +----+-------------+-------------+-----------+
//   |    Strobe        Data          Clock       |
//   |    MC14094 8-bit shift/latch register      |
//   |    Q8   Q7   Q6   Q5   Q4   Q3   Q2   Q1   |
//   +----+----+----+----+----+----+----+----+----+
//        |11  |12  |13  |14  |7   |6   |5   |4
//        |11  |12  |13  |14  |6   |5   |4
//   +----+----+----+----+----+----+----+---------+
//   |    DB4  DB5  DB6  DB7  E    RW   RS        |
//   |               LCD KS0066                   |
//   +--------------------------------------------+
//
// 3 Pins required from the Arduino for Data, Clock, and Enable/Strobe.
//
// This code was inspired from LiquidCrystal_SR from
// http://code.google.com/p/arduinoshiftreglcd/
// but was written for implementing LiquidCrystal support for the Pebble
// and Pebblev2 (see below).
// The Pebbles's LCD and shift register wiring were inspired from this
// original page:
// http://www.arduino.cc/playground/Code/LCD3wires
// 
// Pebbles and the LCD3Wires design are compatible hardware-wise, but
// the LCD3Wire code does not work with arduino 1.0 anymore and is generally
// quite limited in functionality compared to this framework that provides the
// entire LiquidDisplay functionality.
// Why not just use the LiquidCrystal_SR pinout?
// - LCD3Wire was first and therefore have hardware that was designed with 
//   incompatible (IMO better if you don't mind 3 wires) pinout.
// - The pinout used here is same saner (the 4 bits for the LCD are all in one 
//   nibble of the shift register, not spread across 2 like in the
//   LiquidCrystal_SR pinout)
//
// Note however that LiquidCrystal_SR while a bit more complex wiring and code
// wise, supports non latching shift registers and it a few percent faster than
// this code since it can address the LCD enable pin without having to send 
// a pulse through the shift register like the LCD3Wires setup requires.
//  
// This code makes sure to properly follow the specifications when talking
// to the LCD while using minimal delays (it's faster than the LCD3wire and aiko
// pebble code).
//
// @author Marc MERLIN - marc_soft<at>merlins.org.
// ---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#if (ARDUINO <  100)
#include <WProgram.h>
#else
#include <Arduino.h>
#endif
#include "FastIO.h"
#include "LiquidCrystal_SR_LCD3.h"


// STATIC helper functions
// ---------------------------------------------------------------------------


// CONSTRUCTORS
// ---------------------------------------------------------------------------
// Assuming 1 line 8 pixel high font
LiquidCrystal_SR_LCD3::LiquidCrystal_SR_LCD3 ( uint8_t srdata, uint8_t srclock, 
                                              uint8_t strobe )
{
	init ( srdata, srclock, strobe, 1, 0 );
}


// PRIVATE METHODS
// ---------------------------------------------------------------------------

//
// init
void LiquidCrystal_SR_LCD3::init( uint8_t srdata, uint8_t srclock, uint8_t strobe, 
                                 uint8_t lines, uint8_t font )
{
	// Initialise private variables
	// translate all pins to bits and registers
	// pinMode to OUTPUT, Output LOW
   
   _srdata_bit = fio_pinToBit(srdata);
   _srdata_register = fio_pinToOutputRegister(srdata);
   _srclock_bit = fio_pinToBit(srclock);
   _srclock_register = fio_pinToOutputRegister(srclock);
   _strobe_bit = fio_pinToBit(strobe);
   _strobe_register = fio_pinToOutputRegister(strobe);
   
   // Little trick to force a pulse of the LCD enable bit and make sure it is
   // low before we start further writes since this is assumed.
   
   write4bits(0);
   
   _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x10DOTS;
}

// PUBLIC METHODS
// ---------------------------------------------------------------------------


/************ low level data pushing commands **********/

// Code below was borrowed from LCD3Wire from
// http://www.arduino.cc/playground/Code/LCD3wires

// bitmasks for control bits on shift register
#define SR_EN_BIT B00010000   // LCD Data enable bit.
#define SR_RW_BIT B00100000   // RW can be pinned low since we only send
#define SR_RS_BIT B01000000   // LOW: command. HIGH: character.

void LiquidCrystal_SR_LCD3::send(uint8_t value, uint8_t mode) 
{
   uint8_t nibble;
   
   mode = mode ? SR_RS_BIT : 0; // RS bit; LOW: command.  HIGH: character.
   
   nibble = value >> 4; // Get high nibble.
   write4bits(nibble | mode);
   
   //delay(1); // This was in the LCD3 code but does not seem needed -- merlin
   
   nibble = value & 15; // Get low nibble
   write4bits(nibble | mode);
}

void LiquidCrystal_SR_LCD3::write4bits(uint8_t nibble) 
{
   nibble &= ~SR_RW_BIT; // set RW LOW (we do this always since we only write).
   
   // Send a High transition to display the data that was pushed
   nibble |=  SR_EN_BIT; // LCD Data Enable HIGH
   _pushOut(nibble);
   nibble &= ~SR_EN_BIT; // LCD Data Enable LOW
   _pushOut(nibble); 
}

// push byte to shift register and on to LCD
void LiquidCrystal_SR_LCD3::_pushOut(uint8_t nibble) 
{
   // Make data available for pushing to the LCD.
   fio_shiftOut(_srdata_register, _srdata_bit, _srclock_register, _srclock_bit, nibble, LSBFIRST);
   
   // Make new data active.
   ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
   {
      fio_digitalWrite_HIGH(_strobe_register, _strobe_bit);
      waitUsec( 1 ); // strobe pulse must be >450ns (old code had 10ms)
      fio_digitalWrite_SWITCHTO(_strobe_register, _strobe_bit,LOW);
   }
   waitUsec( 40 ); // commands need > 37us to settle
}
