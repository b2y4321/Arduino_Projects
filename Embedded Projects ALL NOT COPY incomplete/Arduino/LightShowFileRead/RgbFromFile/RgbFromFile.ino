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

int convertStgTorgb(char *stg, int sz);

//initial set parameters
double delayTime=5.0;
int brtn=100;
double delayTimeMax=150.0;
int brtnmax=100;

//Buffer params
const int bufSize=13;//size of color buffer (number of colors)
const int charBufSize=15;

// File system object.
SdFat sd;

// Log file.
SdFile file;

const uint8_t chipSelect = SS;

char line[charBufSize];
int rgbbuf[3];

int reds[bufSize];
int blues[bufSize];
int greens[bufSize];

int nColors=0;//color counter
int nsize=0;//chars in line
int lnum=0;//line number were on

void setup(){
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

  //Read from file
  while((nsize =file.fgets(line,sizeof(line)))>0){ //didn't work with malloc for 1 reason or anoth
    if(lnum==0 || lnum==1){
      Serial.println(convertStgToHeader(line,nsize));
    }
    else{
      Serial.println(convertStgTorgb(line,nsize));
    }
    lnum++;
  }

  //Print colors
  Serial.println(delayTime);
  Serial.println(brtn);
  
  for(int i=0;i<nColors;i++){
    Serial.print(reds[i]);
    Serial.print("  ");
    Serial.print(greens[i]);
    Serial.print("  ");
    Serial.print(blues[i]);
    Serial.println("  ");
  }
}

void loop(){
  
}

int convertStgToHeader(char *stg, int sz){
  int ndigits=0;
  int ndecimals=0;
  int nlz=0;
  int decadenum=1;
  double decadenum2=1.0;
  for(int i=0;i<sz;i++){
    if(stg[i] != '\n'){
      if(stg[i]>=48 && stg[i]<=57){
         ndigits++;
         if(ndecimals==1){
            nlz++;//leading zeros counter
         }
      }
      else if(stg[i]=='.'){
        ndecimals++;
      }
      else{
        return -1;
      }
    }
    else{
      
    }

  }

  //If we reached eol, then process
  
      //if we have a decimal number, then process delay time
  if(ndecimals==1){
    double delayTimeOrig=delayTime;//Save originalk delay time in case we must go back to it
    
    delayTime=0;//reset delay val as we will change it
    decadenum2=decadenum2*(double)round(pow(10,ndigits-nlz-1));
    for(int j=0;j<(ndigits+1);j++){
      if(stg[j]=='.'){
        continue;
      }
      else{
        delayTime=delayTime+decadenum2*(double)(stg[j]-48);
        decadenum2=decadenum2/10.0;
      }
    }
    if(delayTime>=delayTimeMax){
      delayTime=delayTimeOrig;
      return -1;
    }
  }
  //If no decimals are present, we have a brightness instead
  else if(ndecimals==0){
    if(ndigits<=3){//We will only accept values of 3 digits
      int brtnOrig=brtn;//Save originalk delay time in case we must go back to it
      
      brtn=0;//reset brightness val as we will change it
      decadenum=decadenum*round(pow(10,ndigits-1));
      for(int j=0;j<ndigits;j++){
        brtn=brtn+decadenum*(stg[j]-48);
        decadenum=decadenum/10;
      }

      //If our brightness is greater than 100, bad!
      if(brtn>brtnmax){
        brtn=brtnOrig;
        return -1;
      }
    }
    else{
      return -1;
    }
  }
  else{ //If we have something of more than 2 decimals, bad!
    return -1;
  }
      

  
}

int convertStgTorgb(char *stg, int sz){
  int ndigits=0;
  int nvals=0;
  int currValue=0;
  int pval=1;
  
  for(int i=0;i<sz;i++){
    if(stg[i] != ' ' && stg[i] != '\n'){//Go until we hit a space or return
      //If we don't have a space or return, increment through characters.
      //We take this time to also find any nonnumerical characters.
      
      if(stg[i]>=48 && stg[i]<=57){
        ndigits++;//increment digit counter
      }
      else{
        return -1;//Non-numerical character!
      }
    }
    else{
      //Then we have a color (we hit a space or return delimiter)
      pval=1;//Start at 10^0 power, as we wil lbe tacking on digits starting from ones
      currValue=0;//current counter starts at 0

      if(ndigits<=3){
        //Sum over 10^n
        for(int j=i-1;j>=(i-ndigits);j--){
          currValue=currValue+pval*(stg[j]-48);
          pval=pval*10;
        }
        ndigits=0;
      }
      else{//If we have anything over 3 digits, reject it
        return -1;
      }

      //Now let's check the current value
      if(currValue>255){
        currValue=255;
      }

      //If we have space in the buffer, add the requisite value
      if(nvals<3){
        rgbbuf[nvals]=currValue;
      }
      //If we don't then that means we have >3 colors! Bad!
      else{
        return -1;//We have more than 3 rgb values on each line! Invalid
      }
      nvals++;
    }
  }

  if(nvals==3){
    //If number of values is 3, then we have a proper set
    

    //Let's load our proper set into memory
    //Only do so if we have space in array
    if(nColors<bufSize){
      reds[nColors]=rgbbuf[0];
      greens[nColors]=rgbbuf[1];
      blues[nColors]=rgbbuf[2];
      nColors++;
    }
    
    
    nvals=0;
    
    return 1;
  }
  else{
    //If we have too few values, then we don't have a valid set
    return -1;
  }
  
}
