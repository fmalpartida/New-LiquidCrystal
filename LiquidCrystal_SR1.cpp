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
LiquidCrystal_SR1::LiquidCrystal_SR1 ( uint8_t srsig )
{
	init ( srsig );
}


// PRIVATE METHODS
// ---------------------------------------------------------------------------

//
// init
void LiquidCrystal_SR1::init( uint8_t srsig )
{
   // Initialise private variables
	_srSignalRegister = fio_pinToOutputRegister(srsig,SKIP);
	_srSignalBit = fio_pinToBit(srsig);
	fio_shiftOut1_init(_srSignalRegister,_srSignalBit);
	// all outputs of shift register to low
	fio_shiftOut1(_srSignalRegister, _srSignalBit, 0x00);

   _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x10DOTS;
}

// PUBLIC METHODS
// ---------------------------------------------------------------------------


/************ low level data pushing commands **********/

// send
void LiquidCrystal_SR1::send(uint8_t value, uint8_t mode)
{
	uint8_t val1, val2;


	// Divide byte in two nibbles (val1 and val2), include the RS signal
	// and format it for shiftregister output wiring to the LCD
	mode = mode ? SR_RS_BIT : 0; // RS bit; LOW: command.  HIGH: character.
	val1 = mode | SR_EN_BIT | ((value >> 1) & 0x78); // upper nibble
	val2 = mode | SR_EN_BIT | ((value << 3) & 0x78); // lower nibble

	// unused outputs to high (faster)
	val1 |= SR_UNUSED_BITS;
	val2 |= SR_UNUSED_BITS;

//	val1 |= SR_EN_BIT; // Enable high
	fio_shiftOut1(_srSignalRegister,_srSignalBit,val1);
	waitUsec(1);
//	delay(1);

	val1 &= ~SR_EN_BIT; // Enable low
	fio_shiftOut1(_srSignalRegister,_srSignalBit,val1);
	waitUsec(40);
	delay(1);

//	val2 |= SR_EN_BIT; // Enable high
	fio_shiftOut1(_srSignalRegister,_srSignalBit,val2);
	waitUsec(1);
//	delay(1);

	val2 &= ~SR_EN_BIT; // Enable low
	fio_shiftOut1(_srSignalRegister,_srSignalBit,val2);
	waitUsec(40);
	delay(1);
}
