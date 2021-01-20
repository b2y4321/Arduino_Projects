#include <TimedAction.h>


void checkButton();
TimedAction ButtonCheck=TimedAction(200,checkButton);

void setup() {
  
}

int pwmval=255;
int dir=0;

void loop() {
  ButtonCheck.check();
  analogWrite(9,pwmval);
}

void checkButton(){
   if(digitalRead(A0)==HIGH){
      if(pwmval>=235 && dir==1){
        pwmval=pwmval+20;
        dir=0;
        return;
      }
      if(pwmval<=40 && dir==0){
        dir=1;
        pwmval=pwmval-20;
        return;
      }
      if(dir==0){
        pwmval=pwmval-20;
      }
      if(dir==1){
        pwmval=pwmval+20;
      }
   }
}

