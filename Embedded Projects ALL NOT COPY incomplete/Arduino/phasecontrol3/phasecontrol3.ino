#include <avr/io.h>
#include <avr/interrupt.h>

void phasedet();
int pulsewidth=10;//4 is generally accepted to be good
double cpc=520.83; //Number of cycles of TMR1 counted per half sine AC wave (60hz)
double fracFull=0.25;//Percent power as compared to full power

//buttons
int buttonCount=0;
int prevup=0;
int prevdn=0;
int recalc=0;
int b;
void setup(){
  Serial.begin(9600);
  pinMode(3,INPUT);
  pinMode(8,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(3),phasedet,RISING);//Is RISING right?
  
  // set up Timer1 
  //TRIAC timing calcs: We want OCR1A to be the number of pulses we need to delay firing of the TRIAC
  b=(520.83*(1.0-fracFull));//Comparator stop value (will trigger flag and ISR once TCNT1==OCR1A). A base10 value!
  TCCR1A = 0x00;    //Upper timer1 control bits
  TCCR1B = 0x00;    //Lower timer1 control bits
  TCNT1=0;
  OCR1A=b;
  //Other stuff     
  TIMSK1 = 0x03;//0x03    //enable comparator A and overflow interrupts

  //Timer2 setup
  TCCR2B=0x00;
  
  TCNT2=0;
  TIFR2=0;
  TIMSK2=0x01;//ovf interrupt
  TCCR2A=0x00;
  TCCR2B=0b00000101;//ps=1024
}

void loop(){
  if(recalc==1){
    int b=(int)(520.83*(1.0-fracFull));
    recalc=0;
  }
  Serial.println(TCNT1);
}

void phasedet(){
   TCCR1B=0x04;
   TCNT1=0;//Can load any value to timer count 1, like in PIC (from 0 to 65536)
}

ISR(TIMER1_COMPA_vect){ //comparator ISR command
  digitalWrite(8,HIGH);  //set TRIAC gate to high
  TCNT1=65536-pulsewidth;//We turn gate on until timer overflows, for a lenth of pulsewidth
}

ISR(TIMER1_OVF_vect){ //comparator ISR command
  digitalWrite(8,LOW);  //set TRIAC gate to low
  TCCR1B = 0x00;    //Turn off the damn timer
}

ISR(TIMER2_OVF_vect){
   buttonCount++;
   if(buttonCount==15){
       buttonCount=0;
       if(digitalRead(4)==1){
          if(prevup==1 && fracFull<1.0){
            fracFull=fracFull+0.05;
            recalc=1;
          }
       }
    
       if(digitalRead(5)==1){
          if(prevdn==1 && fracFull>0.0){
            fracFull=fracFull-0.05;
            recalc=1;
          }
       }
       prevup=digitalRead(4);
       prevdn=digitalRead(5);
   }
}


//15

