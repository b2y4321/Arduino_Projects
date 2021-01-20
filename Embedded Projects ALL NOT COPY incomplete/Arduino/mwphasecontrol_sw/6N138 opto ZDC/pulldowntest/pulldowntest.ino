double Vread=0;
double Vr2=0;
double Vr3=0;
double Vr4=0;
void setup(){
  pinMode(A0,INPUT);
  Serial.begin(9600);
}
void loop(){
  Vread=(5.0/1023.0)*analogRead(A0);
  if(Vread<4.50){
    delayMicroseconds(100);
    Vr2=(5.0/1023.0)*analogRead(A0);
    delayMicroseconds(100);
    Vr3=(5.0/1023.0)*analogRead(A0);
    delayMicroseconds(100);
    Vr4=(5.0/1023.0)*analogRead(A0);
    Serial.println(Vread);
    Serial.println(Vr2);
    Serial.println(Vr3);
    Serial.println(Vr4);
    delay(100);
  }
}
