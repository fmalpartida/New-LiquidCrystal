// ---------------------------------------------------------------------------
// Created on 11/02/2012 by Florian Fida
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
LiquidCrystal_SR1::LiquidCrystal_SR1(uint8_t srsig)
{
   // Initialise signal pin
   _srSignalRegister = fio_pinToOutputRegister(srsig, SKIP);
   _srSignalBit = fio_pinToBit(srsig);
   fio_shiftOut1_init(_srSignalRegister, _srSignalBit);

   // all outputs of shift register to low
   fio_shiftOut1(_srSignalRegister, _srSignalBit, 0x00);

   _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x10DOTS;
}

// PUBLIC METHODS
// ---------------------------------------------------------------------------

#define SR_RS_BIT		   B00000100	/* = Qc - RS Pin */
#define SR_EN_BIT		   B10000000	/* = Qh - Enable Pin */
#define SR_UNUSED_BITS	B00000011	/* = Qa + Qb */
#define SR_SREN_BIT		B00001000	/* = Qd - Enable Pin */

void LiquidCrystal_SR1::send(uint8_t value, uint8_t mode)
{
   uint8_t val;

   // Divide byte in two nibbles (val1 and val2), include the RS signal
   // and format it for shiftregister output wiring to the LCD
   mode = mode ? SR_RS_BIT : 0; // RS bit; LOW: command.  HIGH: character.

#if 0 /* 1 = Roman's circuit */
   //trying to be compatible to Roman Black's circuit - not working, but why?
   val = mode | (value /* & 0xF0 */);// upper nibble
   val |= SR_UNUSED_BITS;// unused outputs to high (faster)
   shiftIt_rb(val,SR_SREN_BIT);

   val = mode | SR_SREN_BIT | ((value << 4) /* & 0xF0 */);// lower nibble
   val |= SR_UNUSED_BITS;// unused outputs to high (faster)
   shiftIt_rb(val,SR_SREN_BIT);
#endif
   //using different outputs on the shift register seems to make it work...
   val = mode | ((value >> 1) /* & 0x78 */); // upper nibble
   val |= SR_UNUSED_BITS; // unused outputs to high (faster)
   shiftIt(val, SR_EN_BIT);

   val = mode | ((value << 3) /* & 0x78 */); // lower nibble
   val |= SR_UNUSED_BITS; // unused outputs to high (faster)
   shiftIt(val, SR_EN_BIT);
}
// PRIVATE METHODS
// ---------------------------------------------------------------------------
void LiquidCrystal_SR1::shiftIt(uint8_t val, uint8_t enableBit)
{
   val |= enableBit; // Enable Bit HIGH
   fio_shiftOut1(_srSignalRegister, _srSignalBit, val);
   waitUsec(1); // should be > 450ns
   val &= ~enableBit; // Enable Bit LOW
   fio_shiftOut1(_srSignalRegister, _srSignalBit, val);
   waitUsec(40); // should be > 37us
}
