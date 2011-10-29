// ---------------------------------------------------------------------------
// Created by Francisco Malpartida on 20/08/11.
// Copyright 2011 - Under creative commons license 3.0:
//        Attribution-ShareAlike CC BY-SA
//LiquidCrystal
// This software is furnished "as is", without technical support, and with no 
// warranty, express or implied, as to its usefulness for any purpose.
//
// Thread Safe: No
// Extendable: Yes
//
// @file LiquidCrystal.cpp
// This file implements a basic liquid crystal library that comes as standard
// in the Arduino SDK.
// 
// @brief 
// This is a basic implementation of the LiquidCrystal library of the
// Arduino SDK. The original library has been reworked in such a way that 
// this class implements the all methods to command an LCD based
// on the Hitachi HD44780 and compatible chipsets using the parallel port of
// the LCD (4 bit and 8 bit).
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
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <Arduino.h>
#include <LiquidCrystal.h>

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
LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
                             uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                             uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
   init(0, rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t enable,
                             uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                             uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
   init(0, rs, 255, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
                             uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
   init(1, rs, rw, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

LiquidCrystal::LiquidCrystal(uint8_t rs,  uint8_t enable,
                             uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
   init(1, rs, 255, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

// PRIVATE METHODS
// ---------------------------------------------------------------------------

//
// init
void LiquidCrystal::init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
                         uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                         uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
   uint8_t i;
   
   // Initialize the IO pins
   // -----------------------
   
   _rs_pin = rs;
   _rw_pin = rw;
   _enable_pin = enable;
   
   _data_pins[0] = d0;
   _data_pins[1] = d1;
   _data_pins[2] = d2;
   _data_pins[3] = d3; 
   _data_pins[4] = d4;
   _data_pins[5] = d5;
   _data_pins[6] = d6;
   _data_pins[7] = d7;
   
   // Initialize the IO port direction to OUTPUT
   // ------------------------------------------
   
   for ( i = 0; i < 4; i++ )
   {
      pinMode ( _data_pins[i], OUTPUT );
   }
   
   // Initialize the rest of the ports if it is an 8bit controlled LCD
   // ----------------------------------------------------------------
   
   if ( !fourbitmode )
   {
      for ( i = 4; i < 7; i++ )
      {
         pinMode ( _data_pins[i], OUTPUT );
      }
   }
   pinMode(_rs_pin, OUTPUT);
   
   // we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
   if (_rw_pin != 255) 
   { 
      pinMode(_rw_pin, OUTPUT);
   }
   
   pinMode(_enable_pin, OUTPUT);
   
   // Initialise displaymode functions to defaults: LCD_1LINE and LCD_5x8DOTS
   // -------------------------------------------------------------------------
   if (fourbitmode)
      _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
   else 
      _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
}

// PUBLIC METHODS
// ---------------------------------------------------------------------------

//
// begin
void LiquidCrystal::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) 
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
   
   // Now we pull both RS and R/W low to begin commands
   digitalWrite(_rs_pin, LOW);
   digitalWrite(_enable_pin, LOW);
   
   if (_rw_pin != 255) 
   { 
      digitalWrite(_rw_pin, LOW);
   }
   
   //put the LCD into 4 bit or 8 bit mode
   // -------------------------------------
   if (! (_displayfunction & LCD_8BITMODE)) 
   {
      // this is according to the hitachi HD44780 datasheet
      // figure 24, pg 46
      
      // we start in 8bit mode, try to set 4 bit mode
      write4bits(0x03);
      delayMicroseconds(4500); // wait min 4.1ms
      
      // second try
      write4bits(0x03);
      delayMicroseconds(4500); // wait min 4.1ms
      
      // third go!
      write4bits(0x03); 
      delayMicroseconds(150);
      
      // finally, set to 4-bit interface
      write4bits(0x02); 
   } 
   else 
   {
      // this is according to the hitachi HD44780 datasheet
      // page 45 figure 23
      
      // Send function set command sequence
      command(LCD_FUNCTIONSET | _displayfunction);
      delayMicroseconds(4500);  // wait more than 4.1ms
      
      // second try
      command(LCD_FUNCTIONSET | _displayfunction);
      delayMicroseconds(150);
      
      // third go
      command(LCD_FUNCTIONSET | _displayfunction);
   }
   
   // finally, set # lines, font size, etc.
   command(LCD_FUNCTIONSET | _displayfunction);  
   
   // turn the display on with no cursor or blinking default
   _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
   display();
   
   // clear the LCD
   clear();
   
   // Initialize to default text direction (for romance languages)
   _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
   // set the entry mode
   command(LCD_ENTRYMODESET | _displaymode);
   
}

/************ low level data pushing commands **********/

// send
void LiquidCrystal::send(uint8_t value, uint8_t mode) 
{
   digitalWrite(_rs_pin, mode);
   
   // if there is a RW pin indicated, set it low to Write
   // ---------------------------------------------------
   if (_rw_pin != 255) 
   { 
      digitalWrite(_rw_pin, LOW);
   }
   
   if (_displayfunction & LCD_8BITMODE)
   {
      write8bits(value); 
   } 
   else 
   {
      write4bits(value>>4);
      write4bits(value);
   }
}

//
// pulseEnable
void LiquidCrystal::pulseEnable(void) 
{
   digitalWrite(_enable_pin, LOW);
   delayMicroseconds(1);
   
   digitalWrite(_enable_pin, HIGH);
   delayMicroseconds(1);           // enable pulse must be > 450ns
   
   digitalWrite(_enable_pin, LOW);
   delayMicroseconds(100);         // commands need > 37us to settle
}

//
// write4bits
void LiquidCrystal::write4bits(uint8_t value) 
{
   for (uint8_t i = 0; i < 4; i++) 
   {
      digitalWrite(_data_pins[i], (value >> i) & 0x01);
   }
   
   pulseEnable();
}

//
// write8bits
void LiquidCrystal::write8bits(uint8_t value) 
{
   for (uint8_t i = 0; i < 8; i++) 
   {
      digitalWrite(_data_pins[i], (value >> i) & 0x01);
   }
   
   pulseEnable();
}
