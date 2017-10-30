
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include "automower.h"
#include <ArduinoJson.h>
#include "FS.h"

//tx-rx port
#define IN 1
#define OUT 2

const char *ssid = "";
const char *password = "";

//--web server
#define webServerPort 80
ESP8266WebServer server(webServerPort);

automower automower(IN, OUT);

bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  const char* apSSID = json["apssid"];
  const char* apPSWD = json["appswd"];
  ssid = apSSID;
  password = apPSWD;

  return true;
}

bool saveConfig(String ssid, String pswd) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["apssid"] = ssid;
  json["appswd"] = pswd;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}

void startAP() {
  WiFi.softAP("automowerConfig", "password");
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  makeDefaultHandle();
  server.begin();
}


//boolean connectedToAP = false;

void setup(void) {
  Serial.begin(9600);
  Serial.println("Starting");
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  } else {
    Serial.println("file system mounted");
  }
  //loadConfig File
  if (!loadConfig()) {
    Serial.println("Failed to load config");
    startAP();
  } else {
    Serial.println("Config loaded");

    //connect to AP
    Serial.print ( "Connecting to " );
    Serial.println ( ssid );
    //Serial.print ( "with pass " );
    Serial.println ( password );
    WiFi.begin(ssid, password);

    int cpt = 0;
    while ( WiFi.status() != WL_CONNECTED && cpt < 40) {
      delay ( 500 );
      Serial.print ( "." );
      cpt++;
    }
    if (cpt >= 40) {
      startAP();
    } else {
      Serial.println ( "" );
      Serial.print ( "Connected to " );
      Serial.println ( ssid );
      Serial.print ( "IP address: " );
      Serial.println ( WiFi.localIP() );
      // web server
      makeAutomowerHandle();
      makeDefaultHandle();
      server.begin();
      Serial.println("HTTP server started");
    }
  }
}

void makeDefaultHandle() {
  server.on("/", handleRootConfig);
  server.on("/config", handleConfig);
  server.onNotFound(handleNotFound);
}


void makeAutomowerHandle() {
  server.on("/status", handleStatus);
  server.on("/mode", handleMode);
  server.on("/battery", handleBattery);
  server.on("/battery/temp", handleBatteryTemp);
  server.on("/battery/capacity", handleBatteryCapacity);
  server.on("/battery/voltage", handleBatteryVoltage);
  server.on("/battery/charging", handleBatteryCharging);
  server.on("/battery/charging/chargingTime", handleBatteryChargingTime);
  server.on("/battery/charging/timeSinceCharge", handleBatteryChargingTimeSinceCharge);
  server.on("/battery/charging/amount", handleBatteryChargingAmount);
  server.on("/battery/charging/amountWhenSearching", handleBatteryChargingAmountWhenSearching);
}

void handleStatus() {
  String status = automower.getStatus();
  String res = "{\"status\":\"+status+\"}";
  handleJson(res);
}

void handleMode() {
  String mode;
  if (server.method() == HTTP_GET) {
    mode = automower.getMode();
  }
  if (server.method() == HTTP_POST) {
    mode = automower.setMode(1);
  }
  String res = "{\"mode\":\"+mode+\"}";
  handleJson(res);
}

void handleBattery() {
  String battery = automower.getBattery();
}
void handleBatteryTemp() {
  String batteryTemp = automower.getBatteryTemp();
}

void handleBatteryCapacity() {
  String batteryCapacity = automower.getBatteryCapacity();
}
void handleBatteryVoltage() {
  String batteryVoltage = automower.getBatteryVoltage();
}
void handleBatteryCharging() {
  String batteryCharging = automower.getBatteryCharging();
}

void handleBatteryChargingTime() {
  String batteryChargingTime = automower.getBatteryChargingTime();
}

void handleBatteryChargingTimeSinceCharge() {
  handleJson(automower.getBatteryTimeSinceCharge());
}

void handleBatteryChargingAmount() {
  String batteryChargingAmount = automower.getBatteryChargingAmount();
}

void handleBatteryChargingAmountWhenSearching() {
  String batteryChargingAmountWhenSearching = automower.getBatteryChargingAmountWhenSearching();
}


void handleConfig() {
  Serial.println("config");
  if (server.hasArg("ssid") && server.hasArg("password")) {
    Serial.println("saving");
    saveConfig(server.arg("ssid"), server.arg("password"));
    Serial.println("Restarting");
    ESP.restart();
  } else {
    Serial.println("missing arg redirect");
    String header = "HTTP/1.1 301 OKLocation: /\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);

  }
}

void handleRootConfig() {
  String content = "<html><body><form action='/config' method='POST'>wifi setup<br>";
  content += "ssid:<input type='text' name='ssid' placeholder='user name'><br>";
  content += "Password:<input type='password' name='password' placeholder='password'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form><br>";
  content += "</body></html>";
  server.send(200, "text/html", content);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleJson(String json) {
  server.send(200, "application/json", json);
}

void loop(void) {
  server.handleClient();
  delay(100);
}

