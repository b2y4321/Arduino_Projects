#include <avr/io.h>
#include <avr/interrupt.h>

void phasedet();
int pulsewidth=10;//4 is generally accepted to be good
double cpc=520.83; //Number of cycles of TMR1 counted per half sine AC wave
double fracFull=0.25;//Percent power as compared to full power


void setup(){
  pinMode(2,INPUT_PULLUP);
  pinMode(8,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(2),phasedet,RISING);//Is RISING right?

  // set up Timer1 
  //TRIAC timing calcs: We want OCR1A to be the number of pulses we need to delay firing of the TRIAC
  int b=(int)(520.83*(1.0-fracFull));//Comparator stop value (will trigger flag and ISR once TCNT1==OCR1A). A base10 value!
  TCCR1A = 0x00;    //Upper timer1 control bits
  TCCR1B = 0x00;    //Lower timer1 control bits
  TCNT1=0;
  OCR1A=b;
  //Other stuff     
  TIMSK1 = 0x01;//0x03    //enable comparator A and overflow interrupts
}

void loop(){
  
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





