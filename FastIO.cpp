// ---------------------------------------------------------------------------
// Created by Florian Fida on 20/01/12
// Copyright 2012 - Under creative commons license 3.0:
//        Attribution-ShareAlike CC BY-SA
//        http://creativecommons.org/licenses/by-sa/3.0/
//
// This software is furnished "as is", without technical support, and with no
// warranty, express or implied, as to its usefulness for any purpose.
// ---------------------------------------------------------------------------
// fio_shiftOut1 functions are based on Shif1 protocol developed by Roman Black 
// (http://www.romanblack.com/shift1.htm)
//
// Thread Safe: No
// Extendable: Yes
//
// @file FastIO.h
// This file implements basic fast IO routines.
// 
// @brief 
//
// @version API 1.0.0
//
// @author Florian Fida -
//
// @todo:
//  support chipkit:
// (https://github.com/chipKIT32/chipKIT32-MAX/blob/master/hardware/pic32/
//   cores/pic32/wiring_digital.c)
// ---------------------------------------------------------------------------

#include "FastIO.h"

fio_register fio_pinToOutputRegister(uint8_t pin, uint8_t initial_state)
{
	pinMode(pin, OUTPUT);
   
	if(initial_state != SKIP) 
   {
      digitalWrite(pin, initial_state); // also turns off pwm timer
   }
#ifdef FIO_FALLBACK
	//  just wasting memory if not using fast io...
	return 0;
#else
	return portOutputRegister(digitalPinToPort(pin));
#endif
}

fio_register fio_pinToInputRegister(uint8_t pin)
{
	pinMode(pin, INPUT);
	digitalWrite(pin, LOW); // also turns off pwm timer and pullup
#ifdef FIO_FALLBACK
	//  just wasting memory if not using fast io...
	return 0;
#else
	return portInputRegister(digitalPinToPort(pin));
#endif
}

uint8_t fio_pinToBit(uint8_t pin)
{
#ifdef FIO_FALLBACK
	// (ab)use the bit variable to store the pin
	return pin;
#else
	return digitalPinToBitMask(pin);
#endif
}

void fio_digitalWrite(fio_register pinRegister, uint8_t pinBit, uint8_t value) 
{
#ifdef FIO_FALLBACK
	digitalWrite(pinBit, value);
#else
	if(value == LOW)
   {
		fio_digitalWrite_LOW(pinRegister,pinBit);
	}
   else
   {
		fio_digitalWrite_HIGH(pinRegister,pinBit);
	}
#endif
}

int fio_digitalRead(fio_register pinRegister, uint8_t pinBit)
{
#ifdef FIO_FALLBACK
	return digitalRead (pinBit);
#else
	if (*pinRegister & pinBit)
   {
      return HIGH;
   }
	return LOW;
#endif
}

void fio_shiftOut ( fio_register dataRegister, fio_bit dataBit, fio_register clockRegister, 
                    fio_bit clockBit, uint8_t value, uint8_t bitOrder)
{
	// # disable interrupts
	// uint8_t oldSREG = SREG;
	// cli();
	int8_t i;

	for(i = 0; i < 8; i++)
   {
		if (bitOrder == LSBFIRST)
      {
			fio_digitalWrite(dataRegister, dataBit, !!(value & (1 << i)));
		}
      else
      {
			fio_digitalWrite(dataRegister, dataBit, !!(value & (1 << (7 - i))));
		}
		fio_digitalWrite_HIGH (clockRegister, clockBit);
		// Switching is a little bit faster
		fio_digitalWrite_SWITCH (clockRegister,clockBit);
	}
	// # enable interrupts
	// SREG = oldSREG;
}

void fio_shiftOut(fio_register dataRegister, uint8_t dataBit, fio_register clockRegister, uint8_t clockBit)
{
	// shift out 0x0 (B00000000) fast, byte order is irrelevant
	fio_digitalWrite_LOW (dataRegister, dataBit);

	for(uint8_t i = 0; i<8; ++i)
   {
		fio_digitalWrite_HIGH (clockRegister, clockBit);
		fio_digitalWrite_SWITCH (clockRegister, clockBit);
	}
}
void fio_shiftOut1_init(uint8_t pin)
{
	fio_shiftOut1_init(fio_pinToOutputRegister(pin,HIGH),fio_pinToBit(pin));
}

void fio_shiftOut1_init(fio_register shift1Register, fio_bit shift1Bit)
{
	// Make sure that capacitors are charged
	// 300us is an educated guess...
	fio_digitalWrite(shift1Register,shift1Bit,HIGH);
	delayMicroseconds(300);
}
void fio_shiftOut1(fio_register shift1Register, fio_bit shift1Bit, uint8_t value, boolean noLatch)
{
	/*
	 * this function are based on Shif1 protocol developed by Roman Black (http://www.romanblack.com/shift1.htm)
	 *
	 * test sketches:
	 * 	http://pastebin.com/raw.php?i=2hnC9v2Z
	 * 	http://pastebin.com/raw.php?i=bGg4DhXQ
	 * 	http://pastebin.com/raw.php?i=tg1ZFiM5
	 *    http://pastebin.com/raw.php?i=93ExPDD3 - cascading
	 * tested with:
	 * 	TPIC6595N - seems to work fine (circuit: http://www.3guys1laser.com/
    *                   arduino-one-wire-shift-register-prototype)
	 * 	7HC595N
	 */
	// disable interrupts since timing is going to be critical
	uint8_t oldSREG;
	oldSREG = SREG;
	cli();

	// iterate but ignore last bit (is it correct now?)
	for(int8_t i = 7; i>=0; --i)
   {

		// assume that pin is HIGH (smokin' pot all day... :) - requires initialization
		if(LOW==!!(value & (1 << i)))
      {
			// LOW = 0 Bit
			fio_digitalWrite_SWITCHTO(shift1Register,shift1Bit,LOW);
			// hold pin LOW for 15us
			delayMicroseconds(15);
			fio_digitalWrite_SWITCHTO(shift1Register,shift1Bit,HIGH);
			// hold pin HIGH for 30us
			delayMicroseconds(30);
		}
      else
      {
			// HIGH = 1 Bit
			fio_digitalWrite_SWITCHTO(shift1Register,shift1Bit,LOW);
			//hold pin LOW for 1us - done! :)
			fio_digitalWrite_SWITCHTO(shift1Register,shift1Bit,HIGH);
			//hold pin HIGH for 15us
			delayMicroseconds(15);
		}
		if(!noLatch && i==1)
      {
         break;
      }
	}
   
	if(!noLatch)
   {
		// send last bit (=LOW) and Latch command
		fio_digitalWrite_SWITCHTO(shift1Register,shift1Bit,LOW);
		// Hold pin low for 200us
		delayMicroseconds(199);
		fio_digitalWrite_HIGH(shift1Register,shift1Bit);
		// Hold pin high for 300us and leave it that way - using explicit HIGH here, just in case.
		delayMicroseconds(299);
	}

	// enable interrupts
	SREG = oldSREG;

}
void fio_shiftOut1(uint8_t pin, uint8_t value, boolean noLatch)
{
	fio_shiftOut1(fio_pinToOutputRegister(pin, SKIP),fio_pinToBit(pin),value, noLatch);
}
