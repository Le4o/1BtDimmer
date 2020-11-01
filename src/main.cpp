#include <mbed.h>

#define DOWN 0;
#define UP 1;
#define FALL 2;
#define MIN 3;
#define RISE 4;
#define MAX 5;

DigitalOut lRed(p6);
DigitalOut lBlue(p7);

PwmOut intensity(p8);
InterruptIn btn(p5);

Timeout t;
Ticker ti;

int cState;
bool sBtn;
float frq;

void initialize();
void onButtonPress();
void onButtonRelease();
void pressStateTransition();
void blinkLed();

void initialize() {
  lBlue = 1.0;
  lRed = 0.0;
  intensity = 1.0;
  cState = UP;
}

void onButtonPress(){
  sBtn = true;
  t.attach(&pressStateTransition, 1.0);
}

void onButtonRelease(){
    sBtn = false;
    ti.detach();
    if(cState == MAX || cState == UP || cState == FALL){
        if(intensity > 0.0){
            lBlue = 0.0;
            lRed = 1.0;
            cState = DOWN;
        }
    }
    else if(cState == MIN || cState == DOWN || cState == RISE){
        if(intensity < 1.0){
            lBlue = 1.0;
            lRed = 0.0;
            cState = UP;
        }
    }
}

void pressStateTransition(){
  t.attach(&blinkLed, 0.1);
  if(sBtn){
      if (cState == UP) cState = RISE;
      else if (cState == DOWN) cState = FALL;
      else if (cState == FALL){
          if(intensity == 0.0) cState = MIN;
          else intensity = intensity - 0.05;
      }
      else if (cState == RISE){
          if(intensity == 1.0) cState = MAX;
          else intensity = intensity + 0.05;
      }
      else{
          if (intensity >= 1.0) cState = MAX;
          else if (intensity <= 0.0) cState = MIN;
      }
  }
}

void blinkLed(){
  if(cState == FALL){
      frq = 1;
      lRed = !lRed;
  }
  else if(cState == RISE){
      frq = 1;
      lBlue = !lBlue;
  }
  else if(cState == MIN){
      frq = 0.1;
      lRed = !lRed;
  }
  else if(cState == MAX){
      frq = 0.1;
      lBlue = !lBlue;
  }
  t.attach(&blinkLed, frq);
}

int main() {
  initialize();
  btn.rise(&onButtonPress);
  btn.fall(&onButtonRelease);
}