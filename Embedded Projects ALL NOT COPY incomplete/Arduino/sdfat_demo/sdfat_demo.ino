#include <SPI.h>
#include <Wire.h>

//SDFAT lib
#include <BlockDriver.h>
#include <FreeStack.h>
#include <MinimumSerial.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <sdios.h>
#include <SysCall.h>
//BTW, SDFat sleeps automatically!

#define addr 0x68

void tickincr();//+1sec
void cntpulse();

int dat[5];
int tSim=0;
int wrFlag=0;

int sec=0;
int pulses=0;
int tickFlag=0;

// File system object.
SdFat sd;

// Log file.
SdFile file;

const uint8_t chipSelect = SS;



void setup(){
  Serial.begin(9600);
  
  pinMode(A0,OUTPUT);
  digitalWrite(A0,HIGH);

  sd.begin(chipSelect, SPI_HALF_SPEED);
  if(!sd.begin(chipSelect, SPI_HALF_SPEED)){
     Serial.println("Error");
     return;
  }
  
  if(!file.open("dout.txt", O_RDWR | O_CREAT | O_AT_END)){
    Serial.println("Error2");
  }
  
  file.print(F("nSec"));
  file.print(F("  "));
  file.println(F("Spd"));
  file.close();

  Serial.println("Success:");
}

void loop(){
  file.open("dout.txt", O_RDWR | O_CREAT | O_AT_END);
  file.print(F("nSec"));
  file.print(F("  "));
  file.println(F("Spd"));
  file.close();
  Serial.println("Success:");
  delay(1000);
}
