
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include "automower.h"


//tx-rx port
#define IN 1
#define OUT 2


//Real Access Point configuration
const char
  *ssid = "wifi", 
  *password = "wifipass";

//Software Access Point configuration
const char
*APssid = "ESP6288",
*APpassword = "dupadupa"; //MINIMUM 8 chars or softAP will use default name and pass

//--web server
#define webServerPort 80
ESP8266WebServer server(webServerPort);

//--udp ticker
WiFiUDP g_udp;
#define g_port 6666
const unsigned long int t_waitTimeMS = 30000;//30sec (not exact)
unsigned long int t_memory=0;


automower automower(IN,OUT);
  

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
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
  String message=automower.getStatus();
  server.send(200, "application/json", message);
}
boolean connectedToAP=false;

void setup(void) {

  //Serial
  Serial.begin(115200);

  //soft AP
  Serial.print("Softrawre AP ");
  if(!WiFi.softAP(APssid, APpassword)){
     Serial.println("failed");  
  }else{
     Serial.println("started");
  }

  //connect to AP
  WiFi.begin(ssid, password);

  // web server
  server.on("/json", handleGetValueJSON);
  server.onNotFound(handleNotFound);
  server.begin();  
  Serial.println("HTTP server started");
}

void SerialPrintWiFiStatus(){
  Serial.print("WIFI: ");
  if(WiFi.status() == WL_CONNECTED){ 
    Serial.print("Connected to: ");
    Serial.print(ssid);
    Serial.print(" IP address: ");
    Serial.println(WiFi.localIP());
  }else{
    Serial.print("Disconnected from: ");
    Serial.println(ssid);
  }
}




void loop(void) {

  //webserver
  server.handleClient();

  //ticker
  int d = (int)millis()-t_memory;
  if(d<0) { d=-d; }//millis can overflow
  if(d>t_waitTimeMS) {
     t_memory=millis();
  }

  //check if connection status has changed
  boolean isConnectedToAP=(WiFi.status() == WL_CONNECTED);
  if(connectedToAP != isConnectedToAP) {
    connectedToAP = isConnectedToAP;
    Serial.print("INFO: ");
    if(connectedToAP) { Serial.println("WIFI connected"); }
    else              { Serial.println("WIFI disconnected"); }
  }

  //ms - give esp8266 time to breathe
  delay(100);  
}

