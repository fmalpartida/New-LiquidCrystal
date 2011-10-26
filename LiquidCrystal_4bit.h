#ifndef LiquidCrystal_4bit_h
#define LiquidCrystal_4bit_h

#include <inttypes.h>
#include "LiquidCrystal.h"


class LiquidCrystal_4bit : public LiquidCrystal
{
public:
   LiquidCrystal_4bit(uint8_t rs, uint8_t enable,
                      uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                      uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
   LiquidCrystal_4bit(uint8_t rs, uint8_t rw, uint8_t enable,
                      uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                      uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
   LiquidCrystal_4bit(uint8_t rs, uint8_t rw, uint8_t enable,
                      uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
   LiquidCrystal_4bit(uint8_t rs, uint8_t enable,
                      uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
      

   
   virtual void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);   
   virtual void send(uint8_t, uint8_t);
   virtual void write4bits(uint8_t);
   virtual void pulseEnable();
   
private:
   void init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
             uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
             uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
   
   void write8bits(uint8_t);
   
   uint8_t _rs_pin; // LOW: command.  HIGH: character.
   uint8_t _rw_pin; // LOW: write to LCD.  HIGH: read from LCD.
   uint8_t _enable_pin; // activated by a HIGH pulse.
   uint8_t _data_pins[8];
};

#endif
