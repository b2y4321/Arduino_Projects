#include <SPI.h>
#include <Wire.h>
#include <Math.h>

//SDFAT lib
#include <BlockDriver.h>
#include <FreeStack.h>
#include <MinimumSerial.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <sdios.h>
#include <SysCall.h>
//BTW, SDFat sleeps automatically!

//Other stuff,
int convertStgToInt(char *stg, int sz);

// File system object.
SdFat sd;

// Log file.
SdFile file;

const uint8_t chipSelect = SS;

char line[40];


int bufSize=40;
int nsize=0;//chars in line

void setup(){
  //initialize everything
  //line=malloc(bufSize*sizeof(char));
  
  Serial.begin(9600);

  sd.begin(SS, SPI_HALF_SPEED);
  if(!sd.begin(SS, SPI_HALF_SPEED)){
     Serial.println("Error");
     return;
  }

  //open file
  if (!file.open("TEST.TXT", O_READ)){
     Serial.println("Error 2");
  }
}

void loop(){
  while((nsize =file.fgets(line,sizeof(line)))>0){ //didn't work with malloc for 1 reason or anoth
    //while there is another line
    Serial.print(nsize);
    Serial.print("  ");
    Serial.println(line);

/*
    for(int i=0;i<nsize;i++){
      Serial.print((int)line[i]);
      Serial.print("  ");
    }
    Serial.println("  ");
*/
    Serial.println(convertStgToInt(line,nsize));
  }
}

int convertStgToInt(char *stg, int sz){
   int decadenum=round(pow(10,sz-2));//rounds to 99 and 999? Not if you use round()
   //Serial.println(decadenum);
   int totsum=0;
   
   //If any nonnumerical digits are received, return -1
   for(int i=0;i<sz-1;i++){
      if((stg[i]<48 || stg[i]>57) && stg[i] != '\n'){
        return -1;
      }
      totsum=totsum+decadenum*(stg[i]-48);
      decadenum=decadenum/10;  
      //No worries about return char - added when decadenunm is zero
      
   }

   return totsum;
}
