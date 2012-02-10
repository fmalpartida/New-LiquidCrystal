// ---------------------------------------------------------------------------
// Copyright 2011 - Under creative commons license 3.0:
//        Attribution-ShareAlike CC BY-SA
//
// This software is furnished "as is", without technical support, and with no 
// warranty, express or implied, as to its usefulness for any purpose.
//
// TODO: history, comments, etc...
// ---------------------------------------------------------------------------
#ifndef _LIQUIDCRYSTAL_SR1_
#define _LIQUIDCRYSTAL_SR1_

#include <inttypes.h>
#include "LCD.h"
#include "FastIO.h"

class LiquidCrystal_SR1: public LCD
{
public:
   /*!
    @method     
    @abstract   LCD SHIFT REGISTER constructor.
    @discussion Defines the pin assignment that the LCD will have.
    The constructor does not initialize the LCD. Assuming 1 line 8 pixel high 
    font.
    
    @param srsig[in]   pin for shiftregister signal line.
    */
   LiquidCrystal_SR1(uint8_t srsig);

   /*!
    @function
    @abstract   Send a particular value to the LCD.
    @discussion Sends a particular value to the LCD for writing to the LCD or
    as an LCD command using the shift register.
    
    Users should never call this method.
    
    @param      value[in] Value to send to the LCD.
    @result     mode LOW - write to the LCD CGRAM, HIGH - write a command to
    the LCD.
    */
   virtual void send(uint8_t value, uint8_t mode);

private:
   /*!
    * @method
    * @abstract takes care of the 1 wire shift out process
    */
   void shiftIt(uint8_t val, uint8_t enableBit);

   // signal pin
   fio_register _srSignalRegister;
   fio_bit _srSignalBit;
};

#endif

