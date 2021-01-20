#include <TimedAction.h>

#include <LiquidCrystal.h>

void speedSet();
void speedWarning();
TimedAction speedSetter=TimedAction(300,speedSet);
TimedAction speedse=TimedAction(200,speedWarning);


LiquidCrystal lcd(2,3,6,7,8,9);
boolean speedSets=false;
int maxSpeed=60;

void setup() {
  lcd.begin(16, 2);
  lcd.print("hello, world!");
  lcd.setCursor(0,1);
  pinMode(A2,INPUT);
  pinMode(A1,INPUT);
  pinMode(4,OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
}
void loop() {
   speedSetter.check();
   speedse.check();
   int velocit=analogRead(A1);
   lcd.print("Speed of "+velocit);
}

void speedSet(){
   int stat=digitalRead(A2);
   if(stat==1){
     if(speedSets==true){
        speedSets=false;
     }
     if(speedSets==false){
       speedSets=true;
     }
   }
}

void speedWarning(){
   if(speedSets==1){
      int vel=analogRead(A1);
      if(vel>=maxSpeed){
        digitalWrite(4,HIGH);
      }
      else{
        digitalWrite(4,LOW);
      }
   }
}

