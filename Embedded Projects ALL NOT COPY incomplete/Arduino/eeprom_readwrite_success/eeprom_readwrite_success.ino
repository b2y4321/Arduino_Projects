#include <Wire.h>

#define i2caddr 0x50

int dat1=100;
int back1=0;
int stat1=1;
int stat2=1;
//If write is unsuccessful you will read 255

void setup(){
    Serial.begin(9600);
    Wire.begin();
}

void loop(){
  //Writing part
  Wire.beginTransmission(i2caddr);
  Wire.write(0x00);//High addr register 1
  Wire.write(0x07);//Low addr register 2
  Wire.write(dat1);
  stat1=Wire.endTransmission();

  delay(5);
  
  //Reading part
  //Tell it what you want
  Wire.beginTransmission(i2caddr);
  Wire.write(0x00);//High addr register 1
  Wire.write(0x07);//Low addr register 2
  stat2=Wire.endTransmission();

  //Now request
  Wire.requestFrom(0x50,1);//Request 1 byte from 
  if(Wire.available()){
     back1=Wire.read();
  }
  Serial.println(stat1);
  Serial.println(stat2);
  Serial.println(dat1);
  Serial.println(back1);

  delay(1000);
}

