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
#include <Wire.h>
#include <I2CIO.h>
#include <LiquidCrystal.h>

// flags for backlight control
#define LCD_BACKLIGHT   0x00
#define LCD_NOBACKLIGHT 0x80

/*!
   @warning These definitions will have to be changed should the IOExpander be 
   connected differently to this default.
*/

/*!
    @defined 
    @abstract   Enable bit of the LCD
    @discussion Defines the IO of the expander connected to the LCD Enable
*/
#define En B01000000  // Enable bit

/*!
 @defined 
 @abstract   Read/Write bit of the LCD
 @discussion Defines the IO of the expander connected to the LCD Rw pin
 */
#define Rw B00100000  // Read/Write bit

/*!
 @defined 
 @abstract   Register bit of the LCD
 @discussion Defines the IO of the expander connected to the LCD Register select pin
 */
#define Rs B00010000  // Register select bit


class LiquidCrystal_I2C : public LiquidCrystal 
{
public:
   
   /*!
    @method     
    @abstract   Class constructor. 
    @discussion Initializes class variables and defines the I2C address of the
    LCD. The constructor does not initialize the LCD.
    
    @param      lcd_Addr: I2C address of the IO expansion module. For I2CLCDextraIO,
    the address can be configured using the on board jumpers.
    */
   LiquidCrystal_I2C (uint8_t lcd_Addr);
   
   /*!
    @function
    @abstract   LCD initialization.
    @discussion Initializes the LCD to a given size (col, row). This methods
    initializes the LCD, therefore, it MUST be called prior to using any other
    method from this class.
    
    @param      cols: the number of columns that the display has
    @param      rows: the number of rows that the display has
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
    
    @param      value: Value to send to the LCD.
    @result     mode: LOW - write to the LCD CGRAM, HIGH - write a command to
    the LCD.
    */
   virtual void send(uint8_t value, uint8_t mode);
   
   /*!
    @function
    @abstract   Switch-off the LCD backlight.
    @discussion Switch-off the LCD backlight, this method is not supported by
    the I2CLCDextraIO, it needs an extra IO pin to drive the LCD backlight
    
    @param      none
    */
   void noBacklight();

   /*!
    @function
    @abstract   Switch-on the LCD backlight.
    @discussion Switch-on the LCD backlight, this method is not supported by
    the I2CLCDextraIO, it needs an extra IO pin to drive the LCD backlight
    
    @param      none
    */
   void backlight();


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
    */
   void write4bits(uint8_t);
   
   /*!
    @method     
    @abstract   Pulse the LCD enable line (En).
    @discussion Sends a pulse of 1 uS to the Enable pin to execute an command
    or write operation.
    */
   void pulseEnable(uint8_t);
   
   /*!
    @method     
    @abstract   Writes to the expansion module.
    @discussion Writes a byte to the IO expansion module. This will be applied
    to all its outputs.
    */ 
   void expanderWrite(uint8_t);
   
   uint8_t _Addr;           // I2C Address of the IO expander
   uint8_t _cols;           // Number of columns of the LCD
   uint8_t _rows;           // Number of rows of the LCD
   uint8_t _backlightval;   // Backlight shadow value
   I2CIO   _i2cio;          // I2CIO PCF8574* expansion module driver I2CLCDextraIO
};

#endif
