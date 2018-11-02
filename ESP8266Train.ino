#include <Arduino.h>
#include <ESP8266WiFi.h>

//Webserver
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>   // https://github.com/Links2004/arduinoWebSockets

// Additions for static pages
//#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
//#include <ESP8266WebServer.h>
#include <FS.h>   // Include the SPIFFS library

#include "pin_defines.h"
#include "chug.h"
#include "index.h"
#include "support.h"

uint32_t x, x_low=0,x_high=100;
uint32_t TimeNow;
uint32_t Period;
Chug *chug;
uint32_t lastWhistle;

float motor_pct;
bool  motor_state;
float lights_pct;
bool  lights_state;
float volume_pct;
bool  volume_state;

uint32_t brake_time=millis()+3000;

const char* ssid     = "NETGEAR41";
const char* password = "kindflower181";

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void setup()
{
  Serial.begin(115200);
  delay(10);
  
  WiFi.begin(ssid, password);
  Serial.printf("WiFi started");
  delay(100);
  chug = new Chug();
  chug->BeginSFX("/ch1.wav");
  void setupMotorShield();
  delay(500); // allow time for setups..

  delay(100);

    // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the SPI Flash Files System
  SPIFFS.begin();
  
  x=0;
  Period=500;
  server.on("/", handleIndex);
  server.on("/index", handleIndex);
  server.on("/index.html", handleIndex);
  server.on("/update",handleUpdateJSON);
  server.on("/motor",handleUpdateJSON);
  server.on("/lights",handleUpdateJSON);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  
  motor_pct=0;
  motor_state=false;
  lights_pct=100;
  lights_state=false;
  volume_pct=0;
  volume_state=false;
}

void loop()
{ 
  TimeNow=millis();
  server.handleClient();
  if(chug->AudioLoop()){
    //Could put steam or random effect triggered on chug
    // sound effect here.
  }
} 

                       
