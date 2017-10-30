#include "automower.h"



SoftwareSerial mower(2,3);

automower::automower(byte receivePin, byte transmitPin) {
  _receivePin = receivePin;
  _transmitPin = transmitPin;

  mower=SoftwareSerial(receivePin,transmitPin);
  mower.begin(9600);
}


String automower::send(const byte* cmd) {
  //unsigned long t1=millis();
  //TODO temp
  unsigned long millis;
  int i=0;
  for (i=0;i<5;i++,cmd++) mower.write(*cmd);  
  //delay(100);

  //t=millis();
  unsigned long t=0;
   while (i<5 &&  (millis-t) < 320)          // read incoming data from mower and save it into input string
     if (mower.available() !=0) input[i++]=mower.read();

  return "";
}


String automower::getStatus() {
  return(send(R_STATUS));
}


String automower::getMode() {
  return(send(R_MODE));
}

String automower::setMode(int mode) {

  //return(send(R_MODE));
}


String automower::getTimerStatus() {
  return(send(R_TIMER_STATUS));
}

String automower::getBattery() {

}

String automower::getBatteryTemp() {

}

String automower::getBatteryCapacity() {

}

String automower::getBatteryVoltage() {

}

String automower::getBatteryCharging() {

}

String automower::getBatteryChargingTime() {

}

String automower::getBatteryTimeSinceCharge() {
  //return send();
}

String automower::getBatteryChargingAmount() {

}

String automower::getBatteryChargingAmountWhenSearching() {

}




