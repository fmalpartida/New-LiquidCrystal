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

// CONSTANT  definitions
// ---------------------------------------------------------------------------

// flags for backlight control
#define LCD_NOBACKLIGHT 0x00
#define LCD_BACKLIGHT   0xFF


// CONSTRUCTORS
// ---------------------------------------------------------------------------
LiquidCrystal_I2C::LiquidCrystal_I2C( uint8_t lcd_Addr )
{
   _Addr = lcd_Addr;
   
   _backlightPin  = 0x0;
   _backlightMask = LCD_NOBACKLIGHT;
   
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
   
   _backlightPin  = 0;
   _backlightMask = LCD_NOBACKLIGHT;
   
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
   
   _backlightPin  = 0;
   _backlightMask = LCD_NOBACKLIGHT;

   _En = ( 1 << En );
   _Rw = ( 1 << Rw );
   _Rs = ( 1 << Rs );
   
   // Initialise pin mapping
   _data_pins[0] = ( 1 << d0 );
   _data_pins[1] = ( 1 << d1 );
   _data_pins[2] = ( 1 << d2 );
   _data_pins[3] = ( 1 << d3 );
}

// PUBLIC METHODS
// ---------------------------------------------------------------------------

//
// begin
void LiquidCrystal_I2C::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) 
{
   
   init();     // Initialise the I2C expander interface
   LCD::begin ( cols, lines, dotsize );   
}


// User commands - users can expand this section
//----------------------------------------------------------------------------

// Turn the (optional) backlight off/on
void LiquidCrystal_I2C::setBacklight( uint8_t mode ) 
{
   if ( mode == HIGH )
   {
      _backlightMask = _backlightPin & LCD_BACKLIGHT;
      
   }
   else 
   {
      _backlightMask = _backlightPin & LCD_NOBACKLIGHT;
   }
   _i2cio.write( _backlightMask );
}

void LiquidCrystal_I2C::setBacklightPin ( uint8_t pin )
{
   _backlightPin = ( 1 << pin );
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
      _i2cio.write(0);  // Set the entire port to LOW
   }
   return ( status );
}

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
   
   pinMapValue |= mode | _backlightMask;
   _i2cio.write ( pinMapValue );
   pulseEnable ( pinMapValue );
}

//
// write4bits
void LiquidCrystal_I2C::pulseEnable (uint8_t _data)
{
   _i2cio.write (_data | _En);   // En HIGH
   _i2cio.write (_data & ~_En);  // En LOW
}