#include <SPI.h>
#include <Wire.h>
#include <TM1637Display.h>

//SDFAT lib
#include <BlockDriver.h>
#include <FreeStack.h>
#include <MinimumSerial.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <sdios.h>
#include <SysCall.h>

//display
#define CLK A3
#define DIO A2


#define up 6
#define dn 7
#define rst 5
#define trig 9
#define vpin A0



//Segment preferences
int sevensegavail=1;//Do you have a 7-seg display attached?

//Power parameters for motor
int pwrmax=20;
int pwrmin=4;
int pwr=10;

//Max current params
double vmax1=10.7;//based on 0.47 ohm sense resistor
double vread;
int stopMotor=0;


//Non-editable vars
//Display init
TM1637Display display(CLK, DIO);

//Display error msgs
const uint8_t SEG_ERR[] = {
  SEG_A | SEG_B | SEG_D | SEG_E | SEG_F | SEG_G,   // e
  SEG_E | SEG_G,                                   // r
  SEG_E | SEG_G,                                   // r
  };
  
const uint8_t SEG_NO_SD[] = {
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,           // S
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G            // d
  };

const uint8_t SEG_FILE_ERR[] = {
  SEG_A | SEG_E | SEG_F | SEG_G,                   // F
  SEG_E | SEG_F,                                   // I
  SEG_D | SEG_E | SEG_F,                           // L
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
  };


// SD objects, files, params, etc
SdFat sd;
SdFile file;
const uint8_t chipSelect = SS;

long pcyc=0;//timer val based on PWM

long times=0;//Time since program started

//First clock 
long currtime1=0;
int debtime=300;

//Second clock
long currtime2=0;
int rectime=1000;

//Third clock
long currtime3=0;
int resetTime=2000;

//debounce params
int prevup=1;
int prevdn=1;

void setup(){
  //Pin assignment
  pinMode(up,INPUT);
  pinMode(dn,INPUT);
  pinMode(rst,INPUT);
  pinMode(trig,OUTPUT);
  pinMode(vpin,INPUT);
  
  display.setBrightness(0x0f);
  pcyc=(long)(64000.0*((double)pwr/(double)pwrmax));
  
  //Timer setup
  TCCR1A = 0x00;
  TCCR1B = 0b00000001;//PS=1 enabled
  TCNT1=0;
  OCR1A=pcyc;    
  TIMSK1 = 0x03;//comp0 enabled
  
  //SD initialization
  sd.begin(chipSelect, SPI_HALF_SPEED);
  if(!sd.begin(chipSelect, SPI_HALF_SPEED)){
     display.setSegments(SEG_NO_SD);
     delay(2000);
  }
  
  if(!file.open("dout.txt", O_RDWR | O_CREAT | O_AT_END)){
     display.setSegments(SEG_FILE_ERR);
     delay(2000);
  }
  
  file.print(F("Time in secs"));
  file.print(F("  "));
  file.println(F("Power"));
  file.close();

  //writing to timer0 block
  currtime1=millis();
  currtime2=millis();

  //LCD furthering
  if(sevensegavail==1){
    display.showNumberDec(pwr, false);
  }
  
}

void wrDat(){
  file.open("dout.txt", O_RDWR | O_CREAT | O_AT_END);
  
  file.print(times);
  file.print(F("  "));
  file.println(pwr);
  
  file.close();
}

void wrErr(){
  file.open("dout.txt", O_RDWR | O_CREAT | O_AT_END);
  
  file.println(F("Error"));
  
  file.close();
}



void recalcPWM(){
  if(stopMotor==0){
    pcyc=(long)(64000.0*((double)pwr/(double)pwrmax));
    OCR1A=pcyc;
    
    if(sevensegavail==1){
      display.showNumberDec(pwr, false);
    }
  }
  
  else{
    pcyc=0;
    if(sevensegavail==1){
      display.setSegments(SEG_ERR);
    }
  }
  
}

void loop(){
  //Debounce and trigger timers
   if(millis()-currtime1>=debtime){
      currtime1=millis();

      //debounce regime
      if(prevup==0 && digitalRead(up)==0){
        if(pwr<pwrmax){
          pwr++;
          recalcPWM();
        }
      }
      if(prevdn==0 && digitalRead(dn)==0){
        if(pwr>pwrmin){
          pwr--;
          recalcPWM();
        }
      }
      prevup=digitalRead(up);
      prevdn=digitalRead(dn);
   }
   if(millis()-currtime2>=rectime){
      currtime2=millis();

     if(stopMotor==0){
        wrDat();
     }
     else{
        wrErr();
     }
   }

   //Overcurrent detection
   vread=(5.0/1023.0)*analogRead(vpin);
   if(vread>=vmax1){//Stop motor if current is too high
      stopMotor=1;
      recalcPWM();
      currtime3=millis();
   }
   if(stopMotor==1 && millis()-currtime3>=resetTime){//After reset period, reenable
      stopMotor=0;
      recalcPWM();
   }

   //millis overflow correction
   if(millis()<currtime1){
      currtime1=0;
      currtime2=0;
   }
}

ISR(TIMER1_COMPA_vect){ //comparator ISR command
  digitalWrite(trig,0);
}

ISR(TIMER1_OVF_vect){ //comparator ISR command
  digitalWrite(trig,1);
}
