#if (ARDUINO <  100)
#include <WProgram.h>
#else
#include <Arduino.h>
#endif
#include "FastIO.h"
#include <util/delay.h>

fio_register fio_pinToOutputRegister(uint8_t pin){
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW); // also turns off pwm timer
#ifdef FIO_FALLBACK
	//  just wasting memory if not using fast io...
	return 0;
#else
	return portOutputRegister(digitalPinToPort(pin));
#endif
}

fio_register fio_pinToInputRegister(uint8_t pin){
	pinMode(pin, INPUT);
	digitalWrite(pin, LOW); // also turns off pwm timer and pullup
#ifdef FIO_FALLBACK
	//  just wasting memory if not using fast io...
	return 0;
#else
	return portInputRegister(digitalPinToPort(pin));
#endif
}

uint8_t fio_pinToBit(uint8_t pin){
#ifdef FIO_FALLBACK
	// (ab)use the bit variable to store the pin
	return pin;
#else
	return digitalPinToBitMask(pin);
#endif
}

void fio_digitalWrite(fio_register pinRegister, uint8_t pinBit, uint8_t value) {
#ifdef FIO_FALLBACK
	digitalWrite(pinBit, value);
#else
	if(value == LOW){
		fio_digitalWrite_LOW(pinRegister,pinBit);
	}else{
		fio_digitalWrite_HIGH(pinRegister,pinBit);
	}
#endif
}

int fio_digitalRead(fio_register pinRegister, uint8_t pinBit){
#ifdef FIO_FALLBACK
	return digitalRead(pinBit);
#else
	if (*pinRegister & pinBit) return HIGH;
	return LOW;
#endif
}

void fio_shiftOut(fio_register dataRegister, fio_bit dataBit, fio_register clockRegister, fio_bit clockBit, uint8_t value){
	// # disable interrupts
	// uint8_t oldSREG = SREG;
	// cli();

	for(int8_t i = 7; i>-1; --i){
		fio_digitalWrite(dataRegister, dataBit, !!(value & (1 << i)));
		fio_digitalWrite_HIGH(clockRegister, clockBit);
		fio_digitalWrite_LOW(clockRegister, clockBit);
	}

	// # enable interrupts
	// SREG = oldSREG;
}

void fio_shiftOut(fio_register dataRegister, uint8_t dataBit, fio_register clockRegister, uint8_t clockBit){
	fio_digitalWrite_LOW(dataRegister, dataBit);
	for(uint8_t i = 0; i<8; ++i){
			fio_digitalWrite_HIGH(clockRegister, clockBit);
			fio_digitalWrite_LOW(clockRegister, clockBit);
	}
}

void fio_shiftOut1_init(fio_register shift1Register, fio_bit shift1Bit){
	// Make sure that capacitors are charged
	fio_digitalWrite(shift1Register,shift1Bit,HIGH);
	delayMicroseconds(300);
}
void fio_shiftOut1(fio_register shift1Register, fio_bit shift1Bit, uint8_t value){

	// disable interrupts since timing is going to be critical
	uint8_t oldSREG;
	oldSREG = SREG;
	cli();


	// note: profiling showed that shift out takes 18us longer than the sum of needed delays

	// iterate but ignore last byte (must be LOW)
	for(int8_t i = 7; i>0; --i){
		fio_digitalWrite_LOW(shift1Register,shift1Bit);
		if(LOW==!!(value & (1 << i))){
			// LOW = 0 Bit
			// hold pin LOW for 15us
			delayMicroseconds(14);
			fio_digitalWrite_HIGH(shift1Register,shift1Bit);
			// hold pin HIGH for 30us
			delayMicroseconds(29);
		}else{
			// HIGH = 1 Bit
			//hold pin LOW for 1us - did that already
			fio_digitalWrite_HIGH(shift1Register,shift1Bit);
			//hold pin HIGH for 15us
			delayMicroseconds(14);
		}
	}
	// send last bit (=LOW) and Latch command
	fio_digitalWrite_LOW(shift1Register,shift1Bit);
	delayMicroseconds(199); // Hold pin low for 200us
	fio_digitalWrite_HIGH(shift1Register,shift1Bit);
	delayMicroseconds(299); // Hold pin high for 300us

	// enable interrupts
	SREG = oldSREG;

}

