#include <LiquidCrystal.h>

#include <TimedAction.h>


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void cruisectr();
void msw();
void speedReading();
TimedAction cruises=TimedAction(250,cruisectr);//Consider debounce
TimedAction maxspeed=TimedAction(250,msw);//Consider debounce
TimedAction speedRead=TimedAction(100,speedReading);

int cruise=0;
int cruiseSet=20;//cruise speed in mph
int maxSpeed=30;
int mswsetting=0;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  //Speed warning adjust
  pinMode(A3,INPUT);
  pinMode(A7,INPUT);
  pinMode(10,OUTPUT);
  
  pinMode(A4,INPUT);//Throttle reading

  //Cruise control adjust
  pinMode(A5,INPUT);
  pinMode(A6,INPUT);
  //end
  
  pinMode(A2,INPUT);//Speed reading
  
  pinMode(A1,INPUT);//Cruise ctr button

  pinMode(9,OUTPUT);//motor pwm

}

void loop() {
  cruises.check();
  maxspeed.check();
  speedRead.check();

  double throttle=analogRead(A4);
  double dcyc=throttle;//change this?
  analogWrite(9,dcyc);
  
  if(cruise==1){//cruise control
     int vel=analogRead(A1);//change this
     if(vel>cruiseSet){
        digitalWrite(9,LOW);
     }
     if(vel<cruiseSet){
        digitalWrite(9,HIGH);
     }
  }
}

void cruisectr(){ //Cruise ctrl button
   int b=digitalRead(A1);
   if(b==1){
     if(cruise==0){
       cruise=1;
     }
     else{
       cruise=0;
     }
   }

   int up=digitalRead(A5);
   int down=digitalRead(A6);
   if(up==1){
       cruiseSet=cruiseSet+1;
   }
   if(down==1){
       cruiseSet=cruiseSet-1;
   }
}

void speedReading(){ //reads speed in mph
   lcd.setCursor(0, 1);  
   int vel=analogRead(A1);//change this
   lcd.print("Speed ");
   lcd.print(vel);

   double throttle=analogRead(A2);
   lcd.setCursor(10, 1);
   lcd.print("Power ");
   lcd.print(throttle);
}

void msw(){  //Max Speed Warning
   if(mswsetting==1){
      int sped=analogRead(A1);//change this
      if(sped>=maxSpeed){
         digitalWrite(10,HIGH);
      }
   }
}
