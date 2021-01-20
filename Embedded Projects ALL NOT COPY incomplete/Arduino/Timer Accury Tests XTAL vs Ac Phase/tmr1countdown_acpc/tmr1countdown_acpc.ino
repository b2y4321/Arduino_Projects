#include <LiquidCrystal_PCF8574.h>
#include <avr/io.h>
#include <avr/interrupt.h>

LiquidCrystal_PCF8574 lcd(0x27);
void ISRa();

int cnt=0;
int hr=24;
int mins=0;
int secs=0;
int displayCtr=0;
int cc=0;
int keygen=1;

void setup(){
  digitalWrite(8,HIGH);
  attachInterrupt(digitalPinToInterrupt(3), ISRa, FALLING);
  
  lcd.begin(16,2);
  lcd.setBacklight(255);
  
}  

void loop(){
  if(cnt==120){
    cnt=0;
    secs--;
    displayCtr=1;
  }
   if(secs<0){
      mins--;
      secs=59;
   }
   if(mins<0){
     hr--;
     mins=59;
   }
   if(displayCtr==1 && keygen==1){
     lcd.clear();
     lcd.print(hr);
     lcd.print(':');
     lcd.print(mins);
     lcd.print(':');
     lcd.print(secs);
     displayCtr=0;
   }
   if(secs==0 && mins==0 && hr==0){
      digitalWrite(8,LOW);
      keygen=0;
   }
   if(secs<0 && mins==0 && hr==0){
      secs=0;
   }
}

void ISRa(){
   cnt++;
}




