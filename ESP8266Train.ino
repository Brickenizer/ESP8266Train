#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WebSocketsServer.h>   // https://github.com/Links2004/arduinoWebSockets

#include "pin_defines.h"
#include "index.h"
#include "support.h"
#include "wavspiffs.h"
#include "wav_support.h"

const char* ssid     = "NETGEAR41";
const char* password = "kindflower181";
 
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
 
#include "train.h"

// Variables to be exposed to the API
int speed = 0;     //stopped
int direction = 1; //forward
int lights = 0;    //lights off


void setupMotorShield(void){
  // 4 pins control the motor shield.
  // Pins 4 and 5 control the speed of the motors with PWM in a default range of 0..1023
  // while pins 0 and 2 define the direction the motors will turn.
  // The second motor power source will be used for the cabin lights.
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(MOTOR_DIR_PIN, OUTPUT);

  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_DIR_PIN, OUTPUT);

  // initialize motor and set direction
  digitalWrite(MOTOR_PIN, speed);
  digitalWrite(MOTOR_DIR_PIN, direction);

  // initialize lights
  digitalWrite(LED_PIN, lights);
  digitalWrite(LED_DIR_PIN, 1); // forward polarity through motor driver for LEDs

}
void setup(void){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

   if (!SPIFFS.begin()) {
    Serial.println("SPIFFS.begin() failed");
    return;
  }else{
        Serial.println("SPIFFS.begin() success");
  }

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
 
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
 
  server.on("/", handleRoot);
  server.on("/train", handleTrain);
  server.on("/index", handleIndex);
  server.on("/inline", handleInline);
  server.on("/control", handleControl);
  server.on("/sound",handleSound);
  
  server.onNotFound(handleNotFound);
   server.begin();
  Serial.println("HTTP server started");

  setupMotorShield();
  wav_setup();
}
 
void loop(void){
  server.handleClient();
  wav_loop();
}

/*******/

void startPlaying(const char *filename)
{
  char nowPlaying[80] = "nowPlaying=";

  wav_startPlayingFile(filename);
  strncat(nowPlaying, filename, sizeof(nowPlaying)-strlen(nowPlaying)-1);
  webSocket.broadcastTXT(nowPlaying);
}

void update_browser() {
  if (!wav_playing()) {
    webSocket.broadcastTXT("nowPlaying=");
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    
    case WStype_DISCONNECTED:
    default:
      Serial.printf("[%u] Disconnected!\r\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\r\n", num, payload);

      // Looks for button press "bSFXn=1" messages where n='0'..'9'
      if ((length == 7) &&
          (memcmp((const char *)payload, "bSFX", 4) == 0) &&
          (payload[6] == '1')) {
        switch (payload[4]) {
          case '0':
            startPlaying("/T0.wav");
            break;
          case '1':
            startPlaying("/T1.wav");
            break;
          case '2':
            startPlaying("/T2.wav");
            break;
          case '3':
            startPlaying("/T3.wav");
            break;
          case '4':
            startPlaying("/T4.wav");
            break;
          case '5':
            startPlaying("/T5.wav");
            break;
          case '6':
            startPlaying("/T6.wav");
            break;
          case '7':
            startPlaying("/T7.wav");
            break;
          case '8':
            startPlaying("/Cow1.wav");
            break;
          case '9':
            startPlaying("/Cow3.wav");
            break;
        }
      }
      else {
        Serial.printf("Unknown message from client [%s]\r\n", payload);
      }

      // send message to client
      // webSocket.sendTXT(num, "message here");

      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;
    case WStype_BIN:
      Serial.printf("[%u] get binary length: %u\r\n", num, length);
      hexdump(payload, length);

      // send message to client
      // webSocket.sendBIN(num, payload, length);
      break;
  }
}
