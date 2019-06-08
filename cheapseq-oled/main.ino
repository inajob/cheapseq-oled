#include <U8g2lib.h>
//#include <U8x8lib.h>
#include <Wire.h>
#include "tones.h"
#include "MultiTunes.h"
#include "encoder.h"
#include "PatternEdit.h"
#include "RythmEdit.h"

//U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
extern volatile unsigned int d[4];
extern volatile unsigned char vol[4];
extern volatile byte nf;
extern volatile byte nf2;

byte bstate[21];

byte track[3] = {0, 0, 0};
byte score[2][8][8];
byte rscore[8][16];
byte pos = 0;
byte rpos = 0;
byte activePane = 0;


const byte M_SPEED = 0;
const byte M_SHIFT = 1;
const byte M_OP = 2;

byte mode = M_SPEED;

// global params
int speed = 500;
int shift = 0;
int op = 0;


PatternEdit patternEdit(score[0],&track[0],0,0);
PatternEdit patternEdit2(score[1],&track[1],0,32);
RythmEdit rythmEdit(rscore,&track[2],64,32);

bool visible = true;

void clearScore(){
    for(byte k = 0; k < 2; k ++){
      for(byte i = 0; i < 8; i ++){
        for(byte j = 0; j < 8;j ++){
          score[k][i][j] = 0;
        }
      }
    }
}

void setup(){
  u8g2.begin();
  u8g2.setFlipMode(1);
  u8g2.clear();

  encoder_init();
  soundSetup();

  vol[0] = 8;
  d[0] = 100;

  vol[1] = 8;
  d[1] = 200;

  vol[2] = 0;
  d[2] = 0;
  vol[3] = 0;
  d[3] = 0;


  clearScore();

  pinMode(4, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(10, INPUT);

  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);

  pinMode(14, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);
}

void display(){
  char buf[32];

  soundPause();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_micro_tr);
  u8g2.setDrawColor(1);
  String s;

  s = String("cheapseq");
  u8g2.drawStr(64 + 2,5, s.c_str());

  s = String("Speed: ") + String(speed);
  u8g2.drawStr(64 + 2 + 8,12, s.c_str());

  s = String("Shift: ") + String(shift);
  u8g2.drawStr(64 + 2 + 8,12 + 6*1, s.c_str());

  s = String("Op: ") + String(op);
  u8g2.drawStr(64 + 2 + 8,12 + 6*2, s.c_str());

  u8g2.drawStr(64 + 2,12 + 6*mode, ">");




  //u8g2.setDrawColor(1);
  //for(byte i = 0; i < 32; i ++){
  //  u8g2.drawBox(i*4, 64 - score[0][i]*2 , 2, 2);
  //}
  //u8g2.drawBox(pos*4, 63 , 2, 1);

  patternEdit.display(pos, activePane == 0);
  patternEdit2.display(pos, activePane == 1);
  rythmEdit.display(rpos, activePane == 2);
  u8g2.sendBuffer();

  soundResume();
}

bool dirty = true;
unsigned int bcount = 0;
byte bscan;
unsigned int dcount = 0;
unsigned int pcount = 0;
unsigned int count = 0;
int rdecay = 16;

void loop(){

  pcount ++;
  if(pcount > speed){pcount = 0;} // speed
  if(pcount == 0){
    rpos = (rpos + 1)%16;
    pos = rpos/2;
    if(rpos%2 == 0)dirty = true;
  }

  // sound on
  for(byte i = 0; i < 2; i ++){
    if(score[i][track[i]][pos] > 0){
      d[i] = (tones[score[i][track[i]][pos] + shift] << (1 + op));
      vol[i] = 16;
    }else{
      vol[i] = 0;
    }
  }
  // rythm
  if(rscore[track[2]][rpos] > 0){
    switch(rscore[track[2]][rpos]){
      case 1: nf = 0xf; break;
      case 2: nf2 = 0xf; break;
      case 3: nf = nf2 = 0xf; break;
    }
  }else{
    nf = false;
    nf2 = false;
  }

  // decay
  dcount ++;
  if(dcount > 1)dcount = 0;
  if(dcount == 0){
    //if(vol[0] > 0)vol[0] --;
    if(vol[1] > 0)vol[1] --;
    if(vol[2] > 0)vol[2] --;
  }

  count ++;
  if((count & (0xffff >> rdecay)) == (0xffff >> rdecay)){
    if(nf != 0){
      nf = (nf >> 1);
    }
    if(nf2 != 0){
      nf2 = (nf2 >> 1);
    }
  }

  bcount ++;
  if(bcount == 5){bcount = 0;} // button delay
  if(bcount == 0){
    bscan ++;
    if(bscan > 4){
      bscan = 0;
    }
    pinMode(4, INPUT);
    pinMode(6, INPUT);
    pinMode(7, INPUT);
    pinMode(8, INPUT);
    pinMode(10, INPUT);

    switch(bscan){
      case 0:
        pinMode(10, OUTPUT);
        digitalWrite(10, LOW);
      break;
      case 1:
        pinMode(8, OUTPUT);
        digitalWrite(8, LOW);
      break;
      case 2:
        pinMode(7, OUTPUT);
        digitalWrite(7, LOW);
      break;
      case 3:
        pinMode(6, OUTPUT);
        digitalWrite(6, LOW);
        break;
      case 4:
        pinMode(4, OUTPUT);
        digitalWrite(4, LOW);
      break;
    }

    bool b0,b1,b2,b3,b4;

    b0 = !digitalRead(14);
    b1 = !digitalRead(15);
    b2 = !digitalRead(16);
    b3 = !digitalRead(18);
    b4 = !digitalRead(19);

    switch(bscan){
      case 0:
        if(b1 == 1){bstate[0] ++;}else{bstate[0] = 0;}
        if(b3 == 1){bstate[1] ++;}else{bstate[1] = 0;}
        if(b2 == 1){bstate[2] ++;}else{bstate[2] = 0;}
        if(b0 == 1){bstate[3] ++;}else{bstate[3] = 0;}
      break;
      case 1:
        if(b2 == 1){bstate[4] ++;}else{bstate[4] = 0;}
        if(b3 == 1){bstate[5] ++;}else{bstate[5] = 0;}
        if(b0 == 1){bstate[6] ++;}else{bstate[6] = 0;}
        if(b4 == 1){bstate[7] ++;}else{bstate[7] = 0;}
        if(b1 == 1){bstate[8] ++;}else{bstate[8] = 0;}
      break;
      case 2:
        if(b1 == 1){bstate[9] ++;}else{bstate[9] = 0;}
        if(b4 == 1){bstate[10] ++;}else{bstate[10] = 0;}
        if(b0 == 1){bstate[11] ++;}else{bstate[11] = 0;}
      break;
      case 3:
        if(b3 == 1){bstate[12] ++;}else{bstate[12] = 0;}
        if(b1 == 1){bstate[13] ++;}else{bstate[13] = 0;}
        if(b4 == 1){bstate[14] ++;}else{bstate[14] = 0;}
        if(b0 == 1){bstate[15] ++;}else{bstate[15] = 0;}
        if(b2 == 1){bstate[16] ++;}else{bstate[16] = 0;}
      break;
      case 4:
        if(b0 == 1){bstate[17] ++;}else{bstate[17] = 0;}
        if(b4 == 1){bstate[18] ++;}else{bstate[18] = 0;}
        if(b1 == 1){bstate[19] ++;}else{bstate[19] = 0;}
        if(b2 == 1){bstate[20] ++;}else{bstate[20] = 0;}
      break;
    }

    if(bscan == 4){ // all scan done
      if(bstate[0] == 2){ // F1
        activePane ++;
        if(activePane > 2){
          activePane = 0;
        }
      }else if(bstate[1] > 2){ // F2
        if(bstate[4] == 2){
          mode = M_SPEED;
        }else if(bstate[6] == 2){
          mode = M_SHIFT;
        }else if(bstate[8] == 2){
          mode = M_OP;
        }
      }else{
        switch(activePane){
          case 0:
            dirty = (dirty | patternEdit.run(bstate));
          break;
          case 1:
            dirty = (dirty | patternEdit2.run(bstate));
          break;
          case 2:
            dirty = (dirty | rythmEdit.run(bstate));
          break;
        }
      }
      /*
      for(byte i = 4; i <= 16; i ++){ // keyboard handler
        if(bstate[i] > 0){
          score[0][pos] = i-4 + 1;
          dirty = true;
        }
      }
      if(bstate[3] > 0){ // F4 button handler
        score[0][pos] = 0;
        dirty = true;
      }
      // F1-F2
      if(bstate[0] > 0){visible = true;}
      if(bstate[1] > 0){visible = false;}

      // Right button handler
      if(bstate[19] == 1 || bstate[19] % 32 == 1){
        dirty = true;
        pos ++;
        if(pos > 8){pos = 0;}
      }
      // Left button handler
      if(bstate[17] == 1 || bstate[17] % 32 == 1){
        dirty = true;
        if(pos > 0){
          pos --;
        }else{
          pos = 7;
        }
      }
      */
    }
  }

  if(dirty){
    if(visible)display();
    dirty = false;
  }
}

void rotate(char n){
  static char acc = 0;
  acc += n;

  char diff;

  if(abs(acc) >= 4){
    diff = (acc >> 2);

    switch(mode){
      case M_SPEED:
        if(speed > 100){
          speed += diff*10;
        }else{
          speed += diff;
        }
        if(speed < 50){
          speed = 50;
        }
        break;
      case M_SHIFT:
       shift += diff;
       break;
      case M_OP:
       op += diff;
       break;
    }
    dirty = true;

    acc = 0;
  }
}

