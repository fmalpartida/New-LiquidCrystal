// ---------------------------------------------------------------------------
// Created by Francisco Malpartida on 20/08/11.
// Copyright 2011 - Under creative commons license 3.0:
//        Attribution-ShareAlike CC BY-SA
//
// This module is a port by Marc MERLIN <marc_soft<at>merlins.org>
// See .cpp file for hardware details.
// ---------------------------------------------------------------------------
#ifndef _LIQUIDCRYSTAL_SR_LCD3_
#define _LIQUIDCRYSTAL_SR_LCD3_

#include <inttypes.h>
#include "LCD.h"
#include "FastIO.h"


class LiquidCrystal_SR_LCD3 : public LCD
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
    @param enable[in]   enable pin for the shiftregister (also called strobe).
    */
   LiquidCrystal_SR_LCD3 ( uint8_t srdata, uint8_t srclock, uint8_t enable );
   
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
   void init ( uint8_t srdata, uint8_t srclock, uint8_t enable, uint8_t lines, 
              uint8_t font );
   /*!
    @method     
    @abstract   For sending data when initializing the display to 4-bit
    @discussion Initializes the LCD pin allocation and configuration.
    */
   void write4bits ( uint8_t );
   void _pushOut ( uint8_t );

   // Serial Data pin
   fio_bit _srdata_bit;
   fio_register _srdata_register;

   // Clock Pin
   fio_bit _srclock_bit;
   fio_register _srclock_register;
   
   // Enable Pin
   fio_bit _strobe_bit;
   fio_register _strobe_register;
};

#endif

