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
// Modified to work serially with the shiftOut() function, an 8-bit shiftregister 
// (SR) and an LCD in 4-bit mode.
//
// Shiftregister connection description (NEW as of 2009.07.27)
//
// Bit  #0 - N/C - not connected, used to hold a zero
// Bits #1 - N/C
// Bit  #2 - connects to RS (Register Select) on the LCD
// Bits #3 - #6 from SR connects to LCD data inputs D4 - D7.
// Bit  #7 - is used to enabling the enable-puls (via a diode-resistor AND "gate")
//
// 2 or 3 Pins required from the Arduino for Data, Clock, and Enable (optional). 
// If not using Enable, the Data pin is used for the enable signal by defining 
// the same pin for Enable as for Data. Data and Clock outputs/pins goes to the 
// shiftregister.
// LCD RW-pin hardwired to LOW (only writing to LCD). Busy Flag (BF, data bit D7) 
// is not read.
//
// Any shift register should do. I used 74LS164, for the reason that's what I 
// had at hand.
//
//  Original project homepage: http://code.google.com/p/arduinoshiftreglcd/
//
// History
// 2011.10.29  fmalpartida - adaption of the library to the LCD class hierarchy.
// 2009.05.23  raron - but; based mostly (as in almost verbatim) on the 
//             "official" LiquidCrystal library.
// 2009.07.23  Incorporated some proper initialization routines
//             inspired (lets say copy-paste-tweaked) from LiquidCrystal library 
//             improvements from LadyAda
// 2009.07.25  raron - Fixed comments. I really messed up the comments before 
//             posting this, so I had to fix it.
//             Also renamed a function, but no improvements or functional changes.
// 2009.07.27  Thanks to an excellent suggestion from mircho at the Arduiono 
//             playgrond forum, the number of wires now required is only two!
// 2009.07.28  Mircho / raron - a new modification to the schematics, and a more 
//             streamlined interface
// 2009.07.30  raron - minor corrections to the comments. Fixed keyword highlights. 
//             Fixed timing to datasheet safe.
// 2011.07.02  Fixed a minor flaw in setCursor function. No functional change, 
//             just a bit more memory efficient.
//             Thanks to CapnBry (from google code and github) who noticed it. 
//             URL to his version of shiftregLCD:
//https://github.com/CapnBry/HeaterMeter/commit/c6beba1b46b092ab0b33bcbd0a30a201fd1f28c1
//
//
// This library is only compatible with Arduino's SDK version 1.0
//
//
// @author F. Malpartida - fmalpartida@gmail.com
// ---------------------------------------------------------------------------
#ifndef _LIQUIDCRYSTAL_SR_
#define _LIQUIDCRYSTAL_SR_

#include <inttypes.h>
#include <LCD.h>


// two-wire indicator constant
// ---------------------------------------------------------------------------
#define TWO_WIRE 204
#define SR_RS_BIT 0x04
#define SR_EN_BIT 0x80

class LiquidCrystal_SR : public LCD
{
public:
   /*!
    @method     
    @abstract   LCD SHIFT REGISTER constructors.
    @discussion Defines the pin assignment that the LCD will have.
    The constructor does not initialize the LCD. Assuming 1 line 8 pixel high 
    font.
    
    @param srdata   pin for shiftregister data line.
    @param srclock  pin for shiftregister clock line.
    @param enable   enable pin for the shiftregister.
    */
   LiquidCrystal_SR ( uint8_t srdata, uint8_t srclock, uint8_t enable );


   // Set nr. of lines, assume 8 pixel high font
   LiquidCrystal_SR ( uint8_t srdata, uint8_t srclock, uint8_t enable, 
                      uint8_t lines );
   
   // Set nr. of lines and font
   LiquidCrystal_SR( uint8_t srdata, uint8_t srclock, uint8_t enable, 
                     uint8_t lines, uint8_t font );

      
   /*!
    @function
    @abstract   LCD initialization.
    @discussion Initializes the LCD to a given size (col, row). This methods
    initializes the LCD, therefore, it MUST be called prior to using any other
    method from this class or parent class.
    
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
    as an LCD command using the shift register.
    
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
   void init ( uint8_t srdata, uint8_t srclock, uint8_t enable, uint8_t lines, 
               uint8_t font );
   /*!
    @method     
    @abstract   For sending data when initializing the display to 4-bit
    @discussion Initializes the LCD pin allocation and configuration.
    */
   void init4bits ( uint8_t );

   uint8_t _srdata_pin;  // Serial Data pin
   uint8_t _srclock_pin; // Clock Pin
   uint8_t _enable_pin;  // Enable Pin
   uint8_t _two_wire;    // two wire mode
};

#endif

