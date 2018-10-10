#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "wav_support.h"



#include "support.h"
#include "pin_defines.h"

extern ESP8266WebServer server;

void updateMotor(String motor_str){
  if(motor_str.length()){
    Serial.print ("motor= ");
    Serial.print ( motor_str ) ;
    Serial.print (" = ");
    Serial.print ( motor_str.toInt()  ) ;
    Serial.println("");

    int velocity=motor_str.toInt();
    int speed = abs(velocity);    
    analogWrite(MOTOR_PIN, speed);
    digitalWrite(MOTOR_DIR_PIN, (velocity>0)?1:0);
  }
}

void updateLights(String lights_str){
  if(lights_str.length()){
    Serial.print("lights= ");
    Serial.print ( lights_str  ) ;
    Serial.print(" = ");    
    Serial.print ( lights_str.toInt()  ) ;
    Serial.println("");
    
    int velocity=lights_str.toInt();
    int speed = abs(velocity);
    analogWrite(LED_PIN, speed);
    digitalWrite(LED_DIR_PIN, (velocity>0)?1:0);
  }
}

void handleRoot() {
  digitalWrite(LED_PIN, 1);
  server.send(200, "text/plain", "handleRoot: Hello from esp8266!");
  digitalWrite(LED_PIN, 0);
}

void handleInline(){
  server.send(200, "text/plain", "handleInline: this works as well");
}

void handleControl(){
    server.send(200, "text/plain", "handleControl: this works as well");
    if(server.arg("door")){
    Serial.print("door=");
    Serial.println ( server.arg("door" )  ) ;
    }
    if(server.arg("time" )){
    Serial.print("time=");
    Serial.println ( server.arg("time" )  ) ;
    }
    if(server.arg("motor" ).length()){
      int velocity=server.arg("motor" ).toInt();
      int speed = abs(velocity);
      analogWrite(MOTOR_PIN, speed);
      digitalWrite(MOTOR_DIR_PIN, (velocity>0)?1:0);

      String tmp_str=server.arg("motor");
      int tmp_str_len=tmp_str.length();
      Serial.print("motor= ");
      Serial.print(tmp_str_len);
      Serial.print(" = ");    
      Serial.print ( server.arg("motor" )  ) ;
      Serial.print(" = ");    
      Serial.print ( server.arg("motor" ).toInt()  ) ;
      Serial.println("");
    }else{
      Serial.println("motor not set");
    }
}

void handleSound(){
  server.send(200, "text/plain", "handleSound: playing sounds");
  wav_startPlayingFile("/T0.wav");
  wav_startPlayingFile("/T1.wav");
}


void handleNotFound(){
  digitalWrite(LED_PIN, 1);
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
  digitalWrite(LED_PIN, 0);
}
