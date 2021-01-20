#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "RTClib.h"
#define address 0x68

LiquidCrystal_PCF8574 lcd(0x27);
RTC_DS1307 rtc;
void ISRa();

int cnt=0;
int hr=24;
int mins=0;
int secs=0;
int displayCtr=0;
int cc=0;
int keygen=0;

void setup(){
  digitalWrite(8,HIGH);
  attachInterrupt(digitalPinToInterrupt(3), ISRa, FALLING);
  //attachInterrupt(digitalPinToInterrupt(2), ISRb, RISING);
  

  //rtc setup
  if (! rtc.begin()) {
    lcd.println("Couldn't find RTC");
    while (1);
  }
  
   //Sending i2c commands
  //Enable SQW at 1hz
  Wire.begin();
  Wire.beginTransmission(address);
  Wire.write(0x07); 
  Wire.write(0b00010000);
  Wire.endTransmission();

  //set seconds to 0
  Wire.beginTransmission(address);
  Wire.write(0x00); 
  Wire.write(0x00);
  Wire.endTransmission();
  //end i2c teransmission

lcd.begin(16,2);
  lcd.setBacklight(255);
//Initial time to display
     lcd.clear();
     lcd.print(hr);
     lcd.print(':');
     lcd.print(mins);
     lcd.print(':');
     lcd.print(secs);
}  

void loop(){
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

   //debounce
   if(digitalRead(2)==1){
     StartStop();
   }
}

void ISRa(){
   if(keygen==1){
      secs--;
   }
   displayCtr=1;
}
void StartStop(){
  delay(300);//There is a better way to debounce - just fix it sometime
  if(keygen==0){
    keygen=1;

//Enable SQW
  Wire.beginTransmission(address);
  Wire.write(0x00); 
  Wire.write(0x00);
  Wire.endTransmission();
  //end i2c teransmission

//We put this after - to avoid doublecounting
  Wire.begin();
  Wire.beginTransmission(address);
  Wire.write(0x07); 
  Wire.write(0b00010000);
  Wire.endTransmission();
    
    return;
  }
  if(keygen==1){
    keygen=0;

    //Disable SQW
  Wire.begin();
  Wire.beginTransmission(address);
  Wire.write(0x07); 
  Wire.write(0x00);
  Wire.endTransmission();

//We put this after - to avoid doublecounting
  //set seconds to 0
  Wire.beginTransmission(address);
  Wire.write(0x00); 
  Wire.write(0x00);
  Wire.endTransmission();
    
  }

  
  }
  
   





