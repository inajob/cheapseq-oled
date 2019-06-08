int new_SW, old_SW;
volatile int ecount;
volatile int intcount; //割り込み回数

extern void rotate(char);

//const int QEM[] = {0, -1, 1, 2, 1, 0, 2, -1, -1, 2, 0, 1, 2, 1, -1, 0};
const int QEM[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
void QE_IRQ(void)
{
  old_SW = new_SW;
  new_SW = digitalRead (0) * 2 + digitalRead (1);
  char diff = QEM[old_SW * 4 + new_SW];
  if(diff != 0){rotate(diff);};
  ecount += diff;
  intcount++; //割り込み回数カウント、通常は不要
}

void encoder_init(){
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  attachInterrupt(2, QE_IRQ, CHANGE);
  attachInterrupt(3, QE_IRQ, CHANGE);
  new_SW = digitalRead (0) * 2 + digitalRead (1);
}
