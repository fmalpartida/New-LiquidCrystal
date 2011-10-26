#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "Arduino.h"
#include "LiquidCrystal.h"

// Constructor
LiquidCrystal::LiquidCrystal () 
{

}

/********** high level commands, for the user! */
void LiquidCrystal::clear()
{
   command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
   delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal::home()
{
   command(LCD_RETURNHOME);  // set cursor position to zero
   delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal::setCursor(uint8_t col, uint8_t row)
{
   int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
   if ( row > _numlines ) 
   {
      row = _numlines-1;    // we count rows starting w/0
   }
   
   command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystal::noDisplay() 
{
   _displaycontrol &= ~LCD_DISPLAYON;
   command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal::display() 
{
   _displaycontrol |= LCD_DISPLAYON;
   command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal::noCursor() 
{
   _displaycontrol &= ~LCD_CURSORON;
   command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal::cursor() {
   _displaycontrol |= LCD_CURSORON;
   command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidCrystal::noBlink() {
   _displaycontrol &= ~LCD_BLINKON;
   command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal::blink() {
   _displaycontrol |= LCD_BLINKON;
   command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal::scrollDisplayLeft(void) {
   command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidCrystal::scrollDisplayRight(void) {
   command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal::leftToRight(void) 
{
   _displaymode |= LCD_ENTRYLEFT;
   command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystal::rightToLeft(void) {
   _displaymode &= ~LCD_ENTRYLEFT;
   command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal::autoscroll(void) {
   _displaymode |= LCD_ENTRYSHIFTINCREMENT;
   command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal::noAutoscroll(void) {
   _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
   command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystal::createChar(uint8_t location, uint8_t charmap[]) {
   location &= 0x7; // we only have 8 locations 0-7
   command(LCD_SETCGRAMADDR | (location << 3));
   for (int i=0; i<8; i++) {
      write(charmap[i]);
   }
}

/*********** mid level commands, for sending data/cmds */

void LiquidCrystal::command(uint8_t value) 
{
   send(value, LOW);
}

size_t LiquidCrystal::write(uint8_t value) 
{
   send(value, HIGH);
   return 1; // assume sucess
}