// header for automower
#include "HardwareSerial.h"
#ifndef AUTOMOWER_H
#define AUTOMOWER_H

#define R_STATUS "0f01f10000"
#define R_DAY 0f36b70000"
#define R_MONTH "0f36b90000"
#define R_YEAR "0f36bd0000"
#define R_TIMERSTATUS "0f4a4e0000"
#define R_BATTERY_CURRENT "0f01eb0000"
#define R_CHARGING_TIME "0f01ec0000"
#define R_CHARGING_BATTERY_AMOUNT "0f01ef0000"
#define R_CHARGING_AMOUT_LEFT_WHEN_STARTING_SEARCH "0f1f00000"
#define R_BATTERY_TEMP "0f02330000"
#define R_TIME_SINCE_CHARGING "0f02340000"
#define R_MOTOR_SPEED "0f2eea0000"
#define R_USED_BATTERY_AMOUNT "0f2ee00000"
#define R_BATTERY_VOLTAGE "0f2efe0000"
#define R_TIMER_ACTIF "0f4a4e0000"
#define R_WEEK_TIMER1_START_STD "0F4A380000"
#define R_WEEK_TIMER1_START_MIN "0F4A390000"
#define R_WEEK_TIMER1_STOP_STD "0F4A3A0000"
#define R_WEEK_TIMER1_STOP_MIN "0F4A3B0000"
#define R_WEEKEND_TIMER1_START_STD "0F4A3C0000"
#define R_WEEKEND_TIMER1_START_MIN "0F4A3D0000"
#define R_WEEKEND_TIMER1_STOP_STD "0F4A3E0000"
#define R_WEEKEND_TIMER1_STOP_MIN "0F4A3F0000"
#define R_WEEK_TIMER2_START_STD "0F4A400000"
#define R_WEEK_TIMER2_START_MIN "0F4A410000"
#define R_WEEK_TIMER2_STOP_STD "0F4A420000"
#define R_WEEK_TIMER2_STOP_MIN "0F4A430000"
#define R_WEEKEND_TIMER2_START_STD "0F4A440000"
#define R_WEEKEND_TIMER2_START_MIN "0F4A450000"
#define R_WEEKEND_TIMER2_STOP_STD "0F4A460000"
#define R_WEEKEND_TIMER2_STOP_MIN "0F4A470000"
#define R_TIMER_DAY "0F4A500000"
#define R_MODE "0F012C0000"
#define W_TIMERACTIF "0FCA4E0000"
#define W_TIMERINACTIF "0FCA4E0001"
#define W_MODE_HOME "0F812C0003"
#define W_MODE_MAN "0F812C0000"
#define W_MODE_AUTO "0F812C0001"


class automower
{

  public:

    automower(Stream &serialOut);

    char* getStatus();
    char* getMode();
    char* getTimerStatus();
    char* getBattery();
    char* getBatteryTemp();
    char* getBatteryCapacity();
    char* getBatteryVoltage();
    char* getBatteryCharging();
    char* getBatteryChargingTime();
    char* getBatteryTimeSinceCharge();
    char* getBatteryChargingAmount();
    char* getBatteryChargingAmountWhenSearching();

  private:
    Stream* _serialOut;
    char* send(char* cmd);

};

#endif
