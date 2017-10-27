#include "automower.h"
#include <SoftwareSerial.h>
automower::automower(uint8_t receivePin, uint8_t transmitPin) {
  _receivePin = receivePin;
  _transmitPin = transmitPin;
}


String automower::send(String cmd) {
  SoftwareSerial ESPserial(2, 3);
  ESPserial.begin(9600);

  String readString;

  ESPserial.println(readString);
  while (ESPserial.available()) {
    //delay(3);
    if (ESPserial.available() > 0) {
      char c = ESPserial.read();  //gets one byte from serial buffer
      readString += c; //makes the string readString
    }
  }

  if (readString.length() > 0) {
    return (readString);
  }

  return "";
}


String automower::getStatus() {
  return(send(R_STATUS));
}


String automower::getMode() {
  return(send(R_MODE));
}

String automower::setMode(int mode) {
  return(send(R_MODE));
}


String automower::getTimerStatus() {
  return(send(R_TIMERSTATUS));
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
  return send("test");
}

String automower::getBatteryChargingAmount() {

}

String automower::getBatteryChargingAmountWhenSearching() {

}




