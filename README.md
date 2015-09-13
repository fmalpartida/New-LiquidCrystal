# README #

## Introduction ##

[[https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads/I2CLCDextraIO_assemblyProject_small.jpg|LCD library]]

Welcome to the //LCD Library// for **Arduino** and **Chipkit**. It is a derivate of the original LiquidCrystal Library as 
sourced in the Arduino SDK. It has been developed to be compatible with the current LiquidCrystal library, 
its performance is almost 5 times faster and fully extendable if need be. 

Being faster, gives your applications more time to do more things than just controlling the LCD. So, its cool, you can do more stuff.

It supports most Hitachi HD44780 based LCDs, or compatible, connected to any project using: 4, 8 
wire parallel interface, I2C IO port expander and Shift Regiter.

It currently supports 4 types of connections:
* 4 bit parallel LCD interface
* 8 bit parallel LCD interface
* I2C IO bus expansion board with the PCF8574* I2C IO expander ASIC such as [[http://www.electrofunltd.com/2011/10/i2c-lcd-extra-io.html|I2C LCD extra IO]].
* ShiftRegister adaptor board as described [[http://code.google.com/p/arduinoshiftreglcd/|Shift Register project home]] or in the HW configuration described below, 2 and 3 wire configurations supported.
* ShiftRegister 3 wire latch adaptor board as described [[http://www.arduino.cc/playground/Code/LCD3wires|ShiftRegister 3 Wire Home]]
* Support for 1 wire shift register [[http://www.romanblack.com/shift1.htm|ShiftRegister 1 Wire]]
* I2C bus expansion using general purpose IO lines.

### How do I get set up? ###

* Please refer to the project's wiki

### Contribution guidelines ###

* Writing tests
* Code review
* Help out with bug fixing
* Setup a project logo
* Write new drivers to support more LCDs.

### Who do I talk to? ###

* Repo owner or admin
* For SoftI2CMaster latest versions, updates and support, please refer to [[https://github.com/todbot/SoftI2CMaster|todbot]]