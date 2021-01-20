#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#define up 7
#define dn 8
#define zdc 3
#define gate1 9

void upd();

//Power params
int pwrmin=1;//min pwr
int pwrmax=100;//max pwr
int pwrincr1=1;//Power Increment for slow scrolling
int pwrincr2=5;//Power Increment for fast scrolling (after n presses)
int pwr=50;//Initial pwr
int ssct=5;//Number of consecutive pulses - to switch between slow and fast scrollong.

//Debounce params
int debtime=80;//debounce time ms
int debFlagMax=2; //Stalls polling for n-1 cycles after a press. Use for a after-press delay.

//System params
//Prev button states
int prevup=1;
int prevdn=1;

long prevTime=0;//Time counter
int debFlag=0;//debounce flag. Incremented after button press
int speedSwitchCount=0;//number of consecutive button presses

LiquidCrystal_PCF8574 lcd(0x27);

void setup(){
  //Display setup
  lcd.begin(16, 2); // initialize the lcd
  lcd.setBacklight(255);
  lcd.home(); lcd.clear();
  lcd.print(pwr);

  //debounce
  prevTime=millis();
  pinMode(up,INPUT_PULLUP);
  pinMode(dn,INPUT_PULLUP);
}

void loop(){
  //Debounce
  if(millis()-prevTime>=debtime || millis()<prevTime){
    prevTime=millis();
    
    if(debFlag==0){
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
      if(debFlag==0){
        speedSwitchCount=0;//speed scroll after n consecutive presses
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

void upd(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(pwr);
}
