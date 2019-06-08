#include "PatternEdit.h"

void PatternEdit::init(){

}

void PatternEdit::display(int gpos, bool active){
  const byte size = 7;
  u8g2.setDrawColor(1);
  for(byte i = 0; i < 8; i ++){
    if(score[*track][i] > 0){
      u8g2.drawBox(baseX + i*size, baseY + 32 - score[*track][i]*2 , size, 2);
    }
  }

  // grid
  for(byte i = 0; i < 8; i ++){
    u8g2.drawBox(baseX + i*size, baseY , 1, 8);
  }

  if(active){
    // cursor
    u8g2.drawBox(baseX + pos*size, baseY + 31 - 2 , size, 1);
    u8g2.drawBox(baseX + pos*size, baseY + 0 + 2 , size, 1);
  }

  // seek
  u8g2.drawBox(baseX + gpos*size, baseY , 1, 32);

  // tool
  u8g2.drawFrame(baseX + size*8, baseY , size, 32);

  // track
  u8g2.drawBox(baseX + size*8, baseY + *track * 4, size, 4);

  // frame
  u8g2.drawFrame(baseX + 0, baseY , 63, 32);
}

bool PatternEdit::run(byte* bstate){
  bool dirty = false;
  for(byte i = 4; i <= 16; i ++){ // keyboard handler
    if(bstate[i] > 0){
      score[*track][pos] = i-4 + 1;
      dirty = true;
    }
  }
  if(bstate[3] > 0){ // F4 button handler
    score[*track][pos] = 0;
    dirty = true;
  }
  // F1-F2
  //if(bstate[0] > 0){visible = true;}
  //if(bstate[1] > 0){visible = false;}

  // Up button handler
  if(bstate[20] == 1 || (bstate[20] % 32 == 1) && bstate[20] > 128){
    dirty = true;
    (*track) ++;
    if(*track >= 8){*track = 0;}
  }
  // Down button handler
  if(bstate[18] == 1 || (bstate[18] % 32 == 1) && bstate[18] > 128){
    dirty = true;
    if(*track > 0){
      (*track) --;
    }else{
      *track = 7;
    }
  }

  // Right button handler
  if(bstate[19] == 1 || (bstate[19] % 32 == 1) && bstate[19] > 128){
    dirty = true;
    pos ++;
    if(pos >= 8){pos = 0;}
  }
  // Left button handler
  if(bstate[17] == 1 || (bstate[17] % 32 == 1) && bstate[17] > 128){
    dirty = true;
    if(pos > 0){
      pos --;
    }else{
      pos = 7;
    }
  }
  return dirty;
}

void PatternEdit::rotate(){

}
