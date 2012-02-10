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


// two-wire indicator constant
// ---------------------------------------------------------------------------
#define SR_RS_BIT 0x04
#define SR_EN_BIT 0x02

class LiquidCrystal_SR1 : public LCD
{
public:
   /*!
    @method     
    @abstract   LCD SHIFT REGISTER constructors.
    @discussion Defines the pin assignment that the LCD will have.
    The constructor does not initialize the LCD. Assuming 1 line 8 pixel high 
    font.
    
    @param srdata[in]   pin for shiftregister data line.
    @param srclock[in]  pin for shiftregister clock line.
    @param enable[in]   enable pin for the shiftregister.
    */
   LiquidCrystal_SR1 ( uint8_t srsig );
   
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
    @method     
    @abstract   Initializes the LCD pin allocation
    @discussion Initializes the LCD pin allocation and configuration.
    */
   void init ( uint8_t srsig );
   /*!
    @method     
    @abstract   For sending data when initializing the display to 4-bit
    @discussion Initializes the LCD pin allocation and configuration.
    */
   //void write4bits ( uint8_t );

   // signal pin
   fio_register _srSignalRegister;
   fio_bit  _srSignalBit;

};

#endif

