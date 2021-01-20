#include <SPI.h>
#include <Wire.h>

#define statlight 7


//SDFAT lib
#include <BlockDriver.h>
#include <FreeStack.h>
#include <MinimumSerial.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <sdios.h>
#include <SysCall.h>
//BTW, SDFat sleeps automatically!

// File system object.
SdFat sd;

// Log file.
SdFile file;

void wrData();

const uint8_t chipSelect = SS;
int nSec=10;
int tempSec=0;
int checkFlag=0;
const int bSize=10;

double v1;
double v2;
double v3;
double v4;

double datchann1[bSize];
double datchann2[bSize];
double datchann3[bSize];
double datchann4[bSize];
int indexnum=0;

void setup(){
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);

  Serial.begin(9600);
  
  pinMode(A0,OUTPUT);
  digitalWrite(A0,HIGH);

  sd.begin(chipSelect, SPI_HALF_SPEED);
  if(!sd.begin(chipSelect, SPI_HALF_SPEED)){
     //slow flash
     while(1){
        digitalWrite(statlight,1);
        delay(1000);
        digitalWrite(statlight,0);
        delay(1000);
     }
  }
  
  if(!file.open("dout.txt", O_RDWR | O_CREAT | O_AT_END)){
    //fast flash
    while(1){
        digitalWrite(statlight,1);
        delay(200);
        digitalWrite(statlight,0);
        delay(200);
     }
  }
  
  file.print(F("nSec"));
  file.print(F("  "));
  file.println(F("Voltage1"));
  file.print(F("  "));
  file.println(F("Voltage2"));
  file.print(F("  "));
  file.println(F("Voltage3"));
  file.print(F("  "));
  file.println(F("Voltage4"));
  file.close();

  // set up Timer1 
  
  TCCR1A = 0x00;    //Upper timer1 control bits
  TCNT1=3036;//1s pulse
  TIMSK1 = 0x01;//comp0 disabled
  TCCR1B = 0b00000100;    //PS256, on

  digitalWrite(statlight,1);
}

void loop(){
  if(checkFlag==1){
    checkFlag=0;

    v1=(5.0/1023.0)*analogRead(A0);
    v2=(5.0/1023.0)*analogRead(A1);
    v3=(5.0/1023.0)*analogRead(A2);
    v4=(5.0/1023.0)*analogRead(A3);

    datchann1[indexnum]=v1;
    datchann2[indexnum]=v2;
    datchann3[indexnum]=v3;
    datchann4[indexnum]=v4;

    indexnum++;

    if(indexnum>=bSize){
      wrData(bSize-1);
      indexnum=0;
    }
  }

}

void wrData(int indx){
  int bSize=indx+1;
  
  //If file cannot open, shut down
  if(!file.open("dout.txt", O_RDWR | O_CREAT | O_AT_END)){
    TCCR1B=0x00;
    //fast flash
    while(1){
        digitalWrite(statlight,1);
        delay(200);
        digitalWrite(statlight,0);
        delay(200);
     }
  }

  for(int i=0;i<bSize;i++){
    file.print(ts-(bSize-i-1));
    file.print(F("  "));
    file.println(datchann1[i]);
    file.print(F("  "));
    file.println(datchann2[i]);
    file.print(F("  "));
    file.println(datchann3[i]);
    file.print(F("  "));
    file.println(datchann4[i]);
  }

  file.close();
}

ISR(TIMER1_OVF_vect){ //comparator ISR command
  TCNT1=3036;
  tempSec++;
  ts++;
  
  if(tempSec>=nSec){
    tempSec=0;
    checkFlag=1;
  }
}
