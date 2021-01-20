#include <SPI.h>
#include <Wire.h>
#include <avr/io.h>
#include <avr/interrupt.h>

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
#define warnpin 5
#define statpin 9
#define trig 7

//for u to change
double tdelay=0.1;//up to 1sec
const int nBuf=60;//You MUST use a const int for this to work with arrays

//other
unsigned int b;

//debounce params
int debcount=0;
int prevbutton=1;
int onFlag1=0;
int dataFlag1=0;
int debStep=0;

//data coll
int incr=0;
int datpt=0;
int changeFlag=0;

SdFat sd;
SdFile file;

double datas1[nBuf];
double datas2[nBuf];

//other expr
void wrData(int sz){
  file.open("dout.txt", O_RDWR | O_CREAT | O_AT_END);
  
  for(int i=0;i<sz;i++){
    file.print(datpt,DEC);
    file.print(F("  "));
    file.print(datas1[i],DEC);
    file.print(F("  "));
    file.println(datas2[i],DEC);
    datpt++;
  }
  file.close();
  //Serial.println("wr2");
}

void openclose(int stat){
  if(stat==0){
    file.open("dout.txt", O_RDWR | O_CREAT | O_AT_END);
    file.println(F("End of data entry"));
    file.close();
  }
  if(stat==1){
    file.open("dout.txt", O_RDWR | O_CREAT | O_AT_END);
    file.println(F("Start of data entry"));
    file.close();
  }
}


void setup(){
  //Serial.begin(9600);
  //pins
  pinMode(trig,INPUT_PULLUP);
  pinMode(warnpin,OUTPUT);
  pinMode(statpin,OUTPUT);
  
  //Timer setup
  b=(unsigned int)(65536.0-62500.0*tdelay);//calc delay. MUST be unsigned
  
  TCCR1A = 0x00;    //Upper timer1 control bits
  TCCR1B = 0b00000100;    //Lower timer1 control bits, ps=256
  TCNT1=b;//for 1sec
  TIMSK1 = 0x01;//comp0 disabled

  //timer2 setup
  TCNT2=0;
  TIFR2=0;
  TIMSK2=0x01;//ovf interrupt only
  TCCR2A=0x00;
  TCCR2B=0b00000111;//ps=1024
  
  //starting up
  sd.begin(SS, SPI_HALF_SPEED);

  //doing checks if errrrrythin working
  if(!sd.begin(SS, SPI_HALF_SPEED)){
     //then flash warning light 1
     while(1){
        digitalWrite(warnpin,1);
        delay(300);
        digitalWrite(warnpin,0);
        delay(300);
     }
  }

  if(!file.open("dout.txt", O_RDWR | O_CREAT | O_AT_END)){
    //then flash warning light 2
     while(1){
        digitalWrite(warnpin,1);
        delay(100);
        digitalWrite(warnpin,0);
        delay(100);
     }
  }

  //writign header
  file.print(F("nSec"));
  file.print(F("  "));
  file.print(F("Voltage1"));
  file.print(F("  "));
  file.println(F("Voltage2"));
  file.close();

  digitalWrite(warnpin,0);
}

void loop(){
  digitalWrite(statpin,onFlag1);
  if(changeFlag==1){
    changeFlag=0;

    if(onFlag1==0){
      wrData(incr);
      incr=0;
    }
    
    openclose(onFlag1);   
  }
  if(onFlag1==1){
    if(dataFlag1==1){ 
      dataFlag1=0;
      //Serial.println("wr3");
      datas1[incr]=(5.0/1023.0)*analogRead(A0);
      datas2[incr]=(5.0/1023.0)*analogRead(A1);
      incr++;
    }
  }
  if(incr>=nBuf){
    incr=0;
    wrData(nBuf);
  }
}



ISR(TIMER1_OVF_vect){ //comparator ISR command
  TCNT1=b;//for 1sec
  dataFlag1=1;//This was the problem
}


ISR(TIMER2_OVF_vect){ //comparator ISR command
  debcount++;
  if(debcount==4 && debStep==0){
      debcount=0;
      //Debounce procedure
      if(prevbutton==0 && digitalRead(trig)==0){
        onFlag1=!onFlag1;
        changeFlag=1;
        debStep=1;
      }
      prevbutton=digitalRead(trig);
  }
  if(debcount==8 && debStep==1){
      debcount=0;
      debStep=0;
      //Debounce procedure
      if(prevbutton==0 && digitalRead(trig)==0){
        onFlag1=!onFlag1;
        changeFlag=1;
        debStep=1;
      }
      prevbutton=digitalRead(trig);
  }
  
  
}
