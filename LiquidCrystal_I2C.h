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
// @file LiquidCrystal_I2C.h
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
#ifndef LiquidCrystal_I2C_h
#define LiquidCrystal_I2C_h
#include <inttypes.h>
#include <Print.h>

#include "I2CIO.h"
#include "LCD.h"

// Default library configuration parameters used by class constructor with
// only the I2C address field.
// ---------------------------------------------------------------------------
/*!
 @defined 
 @abstract   Enable bit of the LCD
 @discussion Defines the IO of the expander connected to the LCD Enable
 */
#define EN B01000000  // Enable bit

/*!
 @defined 
 @abstract   Read/Write bit of the LCD
 @discussion Defines the IO of the expander connected to the LCD Rw pin
 */
#define RW B00100000  // Read/Write bit

/*!
 @defined 
 @abstract   Register bit of the LCD
 @discussion Defines the IO of the expander connected to the LCD Register select pin
 */
#define RS B00010000  // Register select bit


class LiquidCrystal_I2C : public LCD 
{
public:
   
   /*!
    @method     
    @abstract   Class constructor. 
    @discussion Initializes class variables and defines the I2C address of the
    LCD. The constructor does not initialize the LCD.
    
    @param      lcd_Addr[in] I2C address of the IO expansion module. For I2CLCDextraIO,
    the address can be configured using the on board jumpers.
    */
   LiquidCrystal_I2C (uint8_t lcd_Addr);
   
   /*!
    @method     
    @abstract   Class constructor. 
    @discussion Initializes class variables and defines the I2C address of the
    LCD. The constructor does not initialize the LCD.
    
    @param      lcd_Addr[in] I2C address of the IO expansion module. For I2CLCDextraIO,
    the address can be configured using the on board jumpers.
    @param      En[in] LCD En (Enable) pin connected to the IO extender module
    @param      Rw[in] LCD Rw (Read/write) pin connected to the IO extender module
    @param      Rs[in] LCD Rs (Reset) pin connected to the IO extender module
    */
   LiquidCrystal_I2C( uint8_t lcd_Addr, uint8_t En, uint8_t Rw, uint8_t Rs);
   
   
   /*!
    @method     
    @abstract   Class constructor. 
    @discussion Initializes class variables and defines the I2C address of the
    LCD. The constructor does not initialize the LCD.
    
    @param      lcd_Addr[in] I2C address of the IO expansion module. For I2CLCDextraIO,
    the address can be configured using the on board jumpers.
    @param      En[in] LCD En (Enable) pin connected to the IO extender module
    @param      Rw[in] LCD Rw (Read/write) pin connected to the IO extender module
    @param      Rs[in] LCD Rs (Reset) pin connected to the IO extender module
    @param      d0[in] LCD data 0 pin map on IO extender module
    @param      d1[in] LCD data 1 pin map on IO extender module
    @param      d2[in] LCD data 2 pin map on IO extender module
    @param      d3[in] LCD data 3 pin map on IO extender module
    */
   LiquidCrystal_I2C( uint8_t lcd_Addr, uint8_t En, uint8_t Rw, uint8_t Rs, 
                     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3 );
   
   /*!
    @function
    @abstract   LCD initialization and associated HW.
    @discussion Initializes the LCD to a given size (col, row). This methods
    initializes the LCD, therefore, it MUST be called prior to using any other
    method from this class or parent class.
    
    The begin method can be overloaded if necessary to initialize any HW that 
    is implemented by a library and can't be done during construction, here
    we use the Wire class.
    
    @param      cols[in] the number of columns that the display has
    @param      rows[in] the number of rows that the display has
    @param      charsize[in] size of the characters of the LCD: LCD_5x8DOTS or
    LCD_5x10DOTS.
    */
   virtual void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);   
   
   /*!
    @function
    @abstract   Send a particular value to the LCD.
    @discussion Sends a particular value to the LCD for writing to the LCD or
    as an LCD command.
    
    Users should never call this method.
    
    @param      value[in] Value to send to the LCD.
    @param      mode[in] DATA - write to the LCD CGRAM, COMMAND - write a 
    command to the LCD.
    */
   virtual void send(uint8_t value, uint8_t mode);
   
   /*!
    @function
    @abstract   Switch-off the LCD backlight.
    @discussion Switch-off the LCD backlight, this method is not supported by
    the I2CLCDextraIO, it needs an extra IO pin to drive the LCD backlight
    
    @param      mode: backlight mode (HIGH|LOW)
    */
   void setBacklight ( uint8_t mode );
   
   
private:
   
   /*!
    @method     
    @abstract   Initializes the LCD class
    @discussion Initializes the LCD class and IO expansion module.
    */
   int  init();
   
   /*!
    @method     
    @abstract   Writes an 4 bit value to the LCD.
    @discussion Writes 4 bits (the least significant) to the LCD control data lines.
    @param      value[in] Value to write to the LCD
    @param      more[in]  Value to distinguish between command and data.
    COMMAND == command, DATA == data.
    */
   void write4bits(uint8_t value, uint8_t mode);
   
   /*!
    @method     
    @abstract   Pulse the LCD enable line (En).
    @discussion Sends a pulse of 1 uS to the Enable pin to execute an command
    or write operation.
    */
   void pulseEnable(uint8_t);
   
   
   uint8_t _Addr;             // I2C Address of the IO expander
   uint8_t _backlightPin;     // Backlight IO pin
   uint8_t _backlightMask;    // Backlight status mask
   I2CIO   _i2cio;            // I2CIO PCF8574* expansion module driver I2CLCDextraIO
   uint8_t _En;               // LCD expander word for enable pin
   uint8_t _Rw;               // LCD expander word for R/W pin
   uint8_t _Rs;               // LCD expander word for Register Select pin
   uint8_t _data_pins[4];     // LCD data lines
   
};

#endif
