#include "Arduino.h"
#include <U8g2lib.h>
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

class RythmEdit{
  byte* score[16];
  byte* track;
  byte pos;
  byte baseX,baseY;
  public:
  RythmEdit(){
  }
  RythmEdit(byte pscore[8][16],byte* ptrack,byte x, byte y): pos(0), track(ptrack), baseX(x), baseY(y){
    for(byte i = 0; i < 16; i ++){
      score[i] = pscore[i];
    }
  }
  void init();
  void display(int gpos, bool active);
  bool run(byte* bstate);
  void rotate();
};
