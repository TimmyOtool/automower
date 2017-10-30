
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
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

//--udp ticker
WiFiUDP g_udp;
#define g_port 6666
const unsigned long int t_waitTimeMS = 30000;//30sec (not exact)
unsigned long int t_memory = 0;


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

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
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
  // Real world application would store these values in some variables for
  // later use.
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

void handleGetValueJSON() {
  String message = "{ ";
  message += " value=";
  message += "";
  message += " }";
  server.send(200, "application/json", message);
}

void handleGetStatus() {
  String message = automower.getStatus();
  server.send(200, "application/json", message);
}
boolean connectedToAP = false;

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
    //TODO listen as AP
    startAP();
  } else {
    Serial.println("Config loaded");

    //connect to AP
    Serial.print ( "Connecting to " );
    Serial.println ( ssid );
    Serial.print ( "with pass " );
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
    }

    Serial.println ( "" );
    Serial.print ( "Connected to " );
    Serial.println ( ssid );
    Serial.print ( "IP address: " );
    Serial.println ( WiFi.localIP() );

    // web server
    server.on("/json", handleGetValueJSON);
    server.on("/", handleRootConfig);
    server.on("/config", handleConfig);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("HTTP server started");
  }
}

void startAP() {
  WiFi.softAP("automowerConfig", "password");

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRootConfig);
  server.on("/config", handleConfig);
  server.begin();
}

void handleConfig() {
  Serial.println("config");
  if (server.hasArg("ssid") && server.hasArg("password")) {
    Serial.println("saving");

    saveConfig(server.arg("ssid"), server.arg("password"));
  } else {
    Serial.println("missing arg redirect");
    String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=0\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
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


void SerialPrintWiFiStatus() {
  Serial.print("WIFI: ");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected to: ");
    Serial.print(ssid);
    Serial.print(" IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.print("Disconnected from: ");
    Serial.println(ssid);
  }
}




void loop(void) {

  //webserver
  server.handleClient();

  //ticker
  int d = (int)millis() - t_memory;
  if (d < 0) {
    d = -d;  //millis can overflow
  }
  if (d > t_waitTimeMS) {
    t_memory = millis();
  }

  //check if connection status has changed
  boolean isConnectedToAP = (WiFi.status() == WL_CONNECTED);
  if (connectedToAP != isConnectedToAP) {
    connectedToAP = isConnectedToAP;
    Serial.print("INFO: ");
    if (connectedToAP) {
      Serial.println("WIFI connected");
    }
    else              {
      Serial.println("WIFI disconnected");
    }
  }

  //ms - give esp8266 time to breathe
  delay(100);
}

