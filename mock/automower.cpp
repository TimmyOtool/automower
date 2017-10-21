#include "automower.h"

automower::automower(Stream &serialOut) {
  _serialOut = &serialOut;

}


char* automower::getStatus() {
  return send(R_STATUS);
}


char* automower::getMode() {
return send(R_MODE);
}

char* automower::getTimerStatus() {
return send(R_TIMERSTATUS);
}

char* automower::getBattery() {

}

char* automower::getBatteryTemp() {

}

char* automower::getBatteryCapacity() {

}

char* automower::getBatteryVoltage() {

}

char* automower::getBatteryCharging() {

}

char* automower::getBatteryChargingTime() {

}

char* automower::getBatteryTimeSinceCharge() {
  return send("test");
}

char* automower::getBatteryChargingAmount() {

}

char* automower::getBatteryChargingAmountWhenSearching() {

}

char* send(char* cmd) {
 //_serialOut-> -> print(cmd);
 return "";
}


