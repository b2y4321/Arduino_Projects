#include <SPI.h>
#include <Wire.h>

//SDFAT lib
#include <BlockDriver.h>
#include <FreeStack.h>
#include <MinimumSerial.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <sdios.h>
#include <SysCall.h>

int ln=1;
int szArr=1;
int returnFlag=0;

int rdFlag=0;
int rgbstep=0;
char buf[2];

void setup() {
  size_t n;
  
  Serial.begin(9600);
  
  pinMode(A0,OUTPUT);
  digitalWrite(A0,HIGH);

  sd.begin(chipSelect, SPI_HALF_SPEED);
  if(!sd.begin(chipSelect, SPI_HALF_SPEED)){
     Serial.println("Error");
     return;
  }
  
  if(!file.open("dout.txt", O_RDWR | O_CREAT | O_AT_END)){
    Serial.println("Error2");
  }
  
  //Reading line by line
  while ((n = file.fgets(line, sizeof(line))) > 0) {
      // Print line number.
      Serial.print(ln++);
      Serial.print(": ");
      Serial.print(line);
      if (line[n - 1] != '\n') {
        // Line is too long or last line is missing nl.
        Serial.println(F(" <-- missing nl"));
        returnFlag=1;
      }
  }
  
  //Processing number of lines
  if(returnFlag==1){
    szArr=ln-2;//We must account for the nonexistent line after return
  }
  if(returnFlag==0){
    szArr=ln-1;
  }

  int redval[szArr];
  int grnval[szArr];
  int blueval[szArr];

  int szVal=0;
  
  //Processing rgb values
  while ((n = file.fgets(line, sizeof(line))) > 0) {
      if (line[n - 1] != '\n') {
        break;
      }
      for(int i=0;i<sizeof(line);i++){
        if(!isdigit(line[i])){
          rdFlag=1;
        }
        if(rdFlag==1){
          rdFlag=0;

          if(rgbstep==0){
            redval[szVal]=100*(line[i-3]-'0')+10*(line[i-2]-'0')+(line[i-1]-'0');//convert numeric chars to int
            rgbstep++;
          }
          if(rgbstep==1){
            grnval[szVal]=100*(line[i-3]-'0')+10*(line[i-2]-'0')+(line[i-1]-'0');//convert numeric chars to int
            rgbstep++;
          }
          if(rgbstep==2){
            grnval[szVal]=100*(line[i-3]-'0')+10*(line[i-2]-'0')+(line[i-1]-'0');//convert numeric chars to int
            rgbstep=0;
            szVal++;
          }
          
        }
        buf[i]=line[i];
      }
  }
  
  Serial.println(F("\nDone"));
}

void loop() {
  // put your main code here, to run repeatedly:

}
