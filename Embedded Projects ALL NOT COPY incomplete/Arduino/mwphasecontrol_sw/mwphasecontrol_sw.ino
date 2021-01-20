#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#define up 8
#define dn 9
#define zdc 2
#define gate1 A2

void trig();
void upd();

//Power params
int pwrmin=1;//min pwr
int pwrmax=100;//max pwr
int pwrincr1=1;//Power Increment for slow scrolling
int pwrincr2=5;//Power Increment for fast scrolling (after n presses)
int pwr=50;//Initial pwr
int ssct=5;//Number of consecutive pulses - to switch between slow and fast scrollong.

//Debounce params
int debtimeorig=70;
int debtime;
int debFlagMax=2; //Stalls polling for n-1 cycles after a press. Use for a after-press delay.

//Trigger params
unsigned int gtrig=50;//Num of clock pulses for gate trig

//System params
//Prev button states
int prevup=1;
int prevdn=1;

long prevTime=0;//Time counter
int debFlag=0;//debounce flag. Incremented after button press
int speedSwitchCount=0;//number of consecutive button presses

LiquidCrystal_PCF8574 lcd(0x27);

void setup(){
  debtime=debtimeorig;
  
  //Interrupts
  attachInterrupt(digitalPinToInterrupt(zdc),trig,RISING);
  pinMode(up,INPUT_PULLUP);
  pinMode(dn,INPUT_PULLUP);
  
  //Timer setup (PS=8)
  OCR1A = 16667.0*(1.0-(double)pwr/(double)pwrmax);      //initialize the comparator
  TIMSK1 = 0x03;    //enable comparator A and overflow interrupts
  TCCR1A = 0x00;    //timer control registers set for
  TCCR1B = 0x00;    //normal operation, timer disabled
  TCNT1=0;

  //Display setup
  lcd.begin(16, 2); // initialize the lcd
  lcd.setBacklight(255);
  lcd.home(); lcd.clear();
  lcd.print(pwr);


  //debounce
  prevTime=millis();
}

void loop(){
  //Debounce
    if(millis()-prevTime>=debtime || millis()<prevTime){
    prevTime=millis();
    
    if(debFlag==0){
      if(speedSwitchCount==ssct-1){
        //If we reach our threshold for fast scroll, double debounce time. Only need to do once.
        //Do this in the cycle before fastscrolling
        debtime=debtime*2;
      }
      if(prevdn==0 && digitalRead(dn)==0){
          if(pwr>=(pwrmin+pwrincr1) && speedSwitchCount<ssct){
            pwr=pwr-pwrincr1;
          }
          else if(pwr>=(pwrmin+pwrincr2)){
            pwr=pwr-pwrincr2;
          }

          debFlag=1;
          speedSwitchCount++;
          
          upd();
      }
      if(prevup==0 && digitalRead(up)==0){
          if(pwr<=(pwrmax-pwrincr1) && speedSwitchCount<ssct){
            pwr=pwr+pwrincr1;
          }
          else if(pwr<=(pwrmax-pwrincr2)){
            pwr=pwr+pwrincr2;
          }
          
          debFlag=1;
          speedSwitchCount++;
          
          upd();
      }
      if(debFlag==0){//If nothing triggered, reset consecutive counter
        speedSwitchCount=0;//speed scroll after n consecutive presses
        debtime=debtimeorig;//Debounce time is normal
      }
      prevdn=digitalRead(dn);
      prevup=digitalRead(up);
    }

    //Set up for delay after press
    if(debFlag>0){
      debFlag++;//delay after press flag
      if(debFlag>debFlagMax){//after timeout period, reset and poll buttons
        debFlag=0;
      }
    } 
  }
  
}

void trig(){
  TCCR1B=0b00000010;//PS8, on
}
void upd(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(pwr);
}

ISR(TIMER1_COMPA_vect){ //comparator match
  digitalWrite(gate1,HIGH);  //set TRIAC gate to high
  TCNT1 = 65536-gtrig;      //trigger pulse width
}

ISR(TIMER1_OVF_vect){ //timer1 overflow
  digitalWrite(gate1,LOW); //turn off TRIAC gate
  TCCR1B = 0x00;          //disable timer stopd unintended triggers
}
