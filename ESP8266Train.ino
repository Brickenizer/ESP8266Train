#include <Arduino.h>
#include <ESP8266WiFi.h>

// System and external includes
//Webserver
#include <ESP8266WebServer.h>
// https://github.com/Links2004/arduinoWebSockets
#include <WebSocketsServer.h>

// Additions for static pages
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
// Include the SPIFFS library for sound, graphics and html
#include <FS.h>   

// local defines
#include "pin_defines.h"
#include "chug.h"
#include "index.h"
#include "support.h"

uint32_t Period;
Chug chug;
uint32_t lastWhistle;

float motor_pct;
bool  motor_state;
float lights_pct;
bool  lights_state;
float volume_pct;
bool  volume_state;

uint32_t brake_time=millis()+3000;

const char* ssid     = "NETGEAR41";
const char* password = "enter_wifi_password";

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void setup()
{
  Serial.begin(115200);
  delay(10);
  
  WiFi.begin(ssid, password);
  Serial.printf("WiFi started");
  delay(100);
  
  chug.BeginSFX("/ch1.wav");
  void setupMotorShield();
  delay(600); // allow time for setups..

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
  server.handleClient();
  // advance the audio loop
  if(chug.AudioLoop()){
    //Could put steam or random effect triggered on chug
    // sound effect here.
  }
} 

                       
