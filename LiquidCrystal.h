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
// @file LiquidCrystal.h
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
#ifndef LiquidCrystal_4bit_h
#define LiquidCrystal_4bit_h

#include <inttypes.h>
#include <LCD.h>

/*!
    @defined 
    @abstract   Enables disables fast waits for write operations for LCD
    @discussion If defined, the library will avoid doing un-necessary waits.
    this can be done, because the time taken by Arduino's slow digitalWrite
    operations. If fast digitalIO operations, comment this line out or undefine
    the mode.
*/
#define FAST_MODE

class LiquidCrystal : public LCD
{
public:
   /*!
    @method     
    @abstract   8 bit LCD constructors.
    @discussion Defines the pin assignment that the LCD will have.
    The constructor does not initialize the LCD.
    */
   LiquidCrystal(uint8_t rs, uint8_t enable,
                 uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                 uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
   LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
                 uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                 uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
   
   /*!
    @method     
    @abstract   4 bit LCD constructors.
    @discussion Defines the pin assignment that the LCD will have.
    The constructor does not initialize the LCD.
    */
   LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
                 uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
   LiquidCrystal(uint8_t rs, uint8_t enable,
                 uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
   
   /*!
    @function
    @abstract   LCD initialization.
    @discussion Initializes the LCD to a given size (col, row). This methods
    initializes the LCD, therefore, it MUST be called prior to using any other
    method from this class or parent class.
    
    @param      cols[in] the number of columns that the display has
    @param      rows[in] the number of rows that the display has
    @param      charsize: size of the characters of the LCD: LCD_5x8DOTS or
    LCD_5x10DOTS.
    */
   virtual void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);   
   
   /*!
    @function
    @abstract   Send a particular value to the LCD.
    @discussion Sends a particular value to the LCD for writing to the LCD or
    as an LCD command.
    
    Users should never call this method.
    
    @param      value Value to send to the LCD.
    @result     mode LOW - write to the LCD CGRAM, HIGH - write a command to
    the LCD.
    */
   virtual void send(uint8_t value, uint8_t mode);
   
   
private:
   
   /*!
    @method     
    @abstract   Initializes the LCD pin allocation
    @discussion Initializes the LCD pin allocation and configuration.
    */
   void init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
             uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
             uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
   
   /*!
    @method     
    @abstract   Writes an 8 bit value to the LCD.
    @discussion Writes 8 bits to the LCD control data lines.
    */
   void write8bits(uint8_t value);
   
   /*!
    @method     
    @abstract   Writes an 4 bit value to the LCD.
    @discussion Writes 4 bits (the least significant) to the LCD control data lines.
    */   
   void write4bits(uint8_t value);
   
   /*!
    @method     
    @abstract   Pulse the LCD enable line (En).
    @discussion Sends a pulse of 1 uS to the Enable pin to execute an command
    or write operation.
    */ 
   void pulseEnable();
   
   uint8_t _rs_pin;       // LOW: command.  HIGH: character.
   uint8_t _rw_pin;       // LOW: write to LCD.  HIGH: read from LCD.
   uint8_t _enable_pin;   // activated by a HIGH pulse.
   uint8_t _data_pins[8]; // Data pins.
};

#endif
