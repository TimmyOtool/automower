#include <SoftwareSerial.h>
#include "automower.h"
/*
  ESP8266 Serial Communication

  The blue LED on the ESP-12 module is connected to GPIO2
  (which is also the TXD pin; so we cannot use Serial.print() at the same time)
*/
String readString;
SoftwareSerial ESPserial(2, 3);

String response(String query){
  if(readString == R_STATUS ){
    return "0f01f10001";
  }
  return "unknow";
}

void setup() {
  ESPserial.begin(9600); 
  Serial.begin(9600);     // Initialize the Serial interface with baud rate of 9600
}

// the loop function runs over and over again forever
void loop() {

  while (Serial.available()) {
    delay(3);  //delay to allow buffer to fill
    if (Serial.available() > 0) {
      char c = Serial.read();  //gets one byte from serial buffer
      readString += c; //makes the string readString
    }
  }

  if (readString.length() > 0) {
    ESPserial.println(response(readString)); //see what was received
    Serial.println(response(readString));
    readString = "";
  }
}
