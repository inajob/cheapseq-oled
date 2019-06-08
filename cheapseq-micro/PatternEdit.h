#include "Arduino.h"
#include <U8g2lib.h>
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

class PatternEdit{
  byte* score[8];
  byte* track;
  byte pos;
  byte baseX,baseY;
  public:
  PatternEdit(){
  }
  PatternEdit(byte pscore[8][8],byte* ptrack,byte x, byte y): pos(0),track(ptrack), baseX(x), baseY(y){
    for(byte i = 0; i < 8; i ++){
      score[i] = pscore[i];
    }
  }
  void init();
  void display(int gpos, bool active);
  bool run(byte* bstate);
  void rotate();
};
