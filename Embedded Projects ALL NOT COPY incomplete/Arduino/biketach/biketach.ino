#include <Arduino.h>
#include <TM1637Display.h>
#include <SPI.h>
#include <SD.h>

//#define CLK 4
//#define DIO 5

void countr();

int sec=0;
int countBuf=0;
int count=0;
int count2=0;
int disp=0;
int wrt=0;
double spd=0;
double spd2=0;
double diam=1.4;//diameter of wheel in ft

//TM1637Display display(CLK, DIO);
File myFile;

void setup(){
  pinMode(3,INPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  
  digitalWrite(9,LOW);
  digitalWrite(8,HIGH);//we are on!

  //Interr setup
  attachInterrupt(digitalPinToInterrupt(3),countr,FALLING);

  //SD card
  if (!SD.begin(4)) {
    digitalWrite(9,HIGH);//uh oh, SD not works
  }
  else{
    myFile = SD.open("spds.txt", FILE_WRITE);
    if (myFile) {
      myFile.print("Time in secs");
      myFile.print("  ");
      myFile.print("Speed mph");
      myFile.print("  ");
      myFile.println("Counts");
      myFile.close();
    }
  }
  
  //timer setup
  TCCR1A = 0x00;    //Upper timer1 control bits
  TCCR1B = 0b00000101;    //Lower timer1 control bits  
  TIMSK1 = 0x01;//0x03    //enable comparator A and overflow interrupts
  TCNT1 = 49911;
  OCR1A=57724;
}
void loop(){
  if(disp==1){
    disp=0;
    //speed calcs
    spd=2.0*0.682*((double)count)*diam;//speed in mph
    countBuf=count;
    count=0;
    //display.showNumberDec((int)spd,false);
  }
  if(wrt==1){
    wrt=0;
    sec++;
    
    //speed calcs
    spd=2.0*0.682*((double)count)*diam;//speed in mph
    spd2=0.682*((double)count2)*diam;//speed in mph
    count2=0;
    count=0;
    //display.showNumberDec((int)spd,false);

    //write ops
    myFile = SD.open("spds.txt", FILE_WRITE);
    if (myFile) {
      myFile.print(sec);
      myFile.print("  ");
      myFile.print(spd2);
      myFile.print("  ");
      myFile.println(countBuf);
      myFile.close();
    }
  }
}



void countr(){
   count++;
   count2++;
}

ISR(TIMER1_COMPA_vect){ //comparator ISR command,m every 0.5s
  disp=1;
}

ISR(TIMER1_OVF_vect){ //timer1 overflow, every 1s
  wrt=1;
  TCNT1=49911;//To get 1 sec, start value set
}
