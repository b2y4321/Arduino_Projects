#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

//Create an instance of the new soft serial library to control the serial LCD
//Note, digital pin 3 of the Arduino should be connected to Rx of the serial LCD.

File myFile;

//This is a character buffer that will store the data from the serial port
char rxData[20];
char rxIndex=0;

//Variables to hold the speed and RPM data.
int vehicleSpeed=0;
int vehicleRPM=0;
int Torques=0;
int timePos=0;
double throttlePercent=0.0;

void setup(){
  //Both the Serial LCD and the OBD-II-UART use 9600 bps.
  Serial.begin(9600);
  //Wait for a little while before sending the reset command to the OBD-II-UART
  delay(1500);
  //Reset the OBD-II-UART
  Serial.println("ATZ");
  //Wait for a bit before starting to send commands after the reset.
  delay(2000);
  //Delete any data that may be in the serial port before we begin.
  Serial.flush();

myFile = SD.open("cardata.txt", FILE_WRITE);
    if (myFile) {
    myFile.print("time");
    myFile.print("  ");
    myFile.print("VehicleRPM");
    myFile.print("  ");
    myFile.print("ThrottlePercent");
    myFile.print("  ");
    myFile.print("Torque");
    myFile.print("  ");
    myFile.println("VehicleSpeed");
    myFile.close();
}


  
}


void loop() {
//vehicle speed
  Serial.println("010D");
  //Get the response from the OBD-II-UART board. We get two responses
  //because the OBD-II-UART echoes the command that is sent.
  //We want the data in the second response.
  getResponse();
  getResponse();
  //Convert the string data to an integer
  vehicleSpeed = strtol(&rxData[6],0,16);
  
  Serial.flush();


//vehicle rpm
  Serial.println("010C");
  getResponse();
  getResponse();
  vehicleRPM = ((strtol(&rxData[6],0,16)*256)+strtol(&rxData[9],0,16))/4;  

  Serial.flush();


//Time since engstart
  Serial.println("011F");
  getResponse();
  getResponse();
  timePos=((strtol(&rxData[6],0,16)*256)+strtol(&rxData[9],0,16));  

  Serial.flush();


//Vehicle throttle percentage
  Serial.println("0145");
  getResponse();
  getResponse();
  throttlePercent = (100.0/256.0)*strtol(&rxData[6],0,16);

  Serial.flush();


//Torque in Nm
  Serial.println("0163");
  getResponse();
  getResponse();
  Torques=((strtol(&rxData[6],0,16)*256)+strtol(&rxData[9],0,16));  

  Serial.flush();

myFile = SD.open("cardata.txt", FILE_WRITE);
  if (myFile) {
    myFile.print(timePos);
    myFile.print("  ");
    myFile.print(vehicleRPM);
    myFile.print("  ");
    myFile.print(throttlePercent);
    myFile.print("  ");
    myFile.print(Torques);
    myFile.print("  ");
    myFile.println(vehicleSpeed);
    myFile.close();
}

delay(100);

}

//The getResponse function collects incoming data from the UART into the rxData buffer
// and only exits when a carriage return character is seen. Once the carriage return
// string is detected, the rxData buffer is null terminated (so we can treat it as a string)
// and the rxData index is reset to 0 so that the next string can be copied.

void getResponse(void){
  char inChar=0;
  //Keep reading characters until we get a carriage return
  while(inChar != '\r'){
    //If a character comes in on the serial port, we need to act on it.
    if(Serial.available() > 0){
      //Start by checking if we've received the end of message character ('\r').
      if(Serial.peek() == '\r'){
        //Clear the Serial buffer
        inChar=Serial.read();
        //Put the end of string character on our data string
        rxData[rxIndex]='\0';
        //Reset the buffer index so that the next character goes back at the beginning of the string.
        rxIndex=0;
      }
      //If we didn't get the end of message character, just add the new character to the string.
      else{
        //Get the new character from the Serial port.
        inChar = Serial.read();
        //Add the new character to the string, and increment the index variable.
        rxData[rxIndex++]=inChar;
      }
    }
  }
}





