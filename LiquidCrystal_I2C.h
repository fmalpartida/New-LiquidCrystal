#ifndef LiquidCrystal_I2C_h
#define LiquidCrystal_I2C_h

#include <inttypes.h>
#include "Print.h"
#include "LiquidCrystal.h"
#include <Wire.h>
#include <I2CIO.h>

// commands

// flags for display entry mode


// flags for display on/off control


// flags for display/cursor shift


// flags for function set


// flags for backlight control
#define LCD_BACKLIGHT 0x00
#define LCD_NOBACKLIGHT 0x80

#define En B01000000  // Enable bit
#define Rw B00100000  // Read/Write bit
#define Rs B00010000  // Register select bit

class LiquidCrystal_I2C : public LiquidCrystal 
{
public:
   LiquidCrystal_I2C (uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows);
   int init();
   
   // Virtual methods
   virtual void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS );
   virtual void send(uint8_t, uint8_t);
   virtual void write4bits(uint8_t);
   virtual void pulseEnable(uint8_t);

   void printLeft();
   void printRight();
   void shiftIncrement();
   void shiftDecrement();
   void noBacklight();
   void backlight();
   
private:
   int  init_priv();

   void expanderWrite(uint8_t);

   uint8_t _Addr;
   uint8_t _cols;
   uint8_t _rows;
   uint8_t _backlightval;
   I2CIO   _i2cio;
   
};

#endif
