#include <LiquidCrystal.h>
LiquidCrystal lcd(1,2,3,4,5,6);

void setup() {
lcd.begin(16,2);
pinMode(A1,INPUT);
pinMode(A2,INPUT);
}


double shuntResistance=0.001;

void loop() {
//Voltage calcs
int a=analogRead(1);
double voltage=a*(5/1023);

//Current calcs
int b=analogRead(2);
double current=b/shuntResistance;

String String1="Voltage ";
String String2="Current ";
//Writing to lcd
lcd.setCursor(0,0);
lcd.print(String1.concat(voltage));
lcd.setCursor(0,1);
lcd.print(String2.concat(current));
}
