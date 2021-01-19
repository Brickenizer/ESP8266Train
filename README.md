# ESP8266Train
Using an Arduino ESP8266 to control a model train.  Web control for basis.

Setup Notes:
Boards:
Set up for working with the ESP8266 by adding the boards to Preferences->Additional Boards Managers URL
http://arduino.esp8266.com/stable/package_esp8266com_index.json,
https://raw.githubusercontent.com/nailbuster/esp8266FTPServer/master/library.json

Configuration:
Select NodeMCU 1.0 (ESP-12E Module)
Upload Speed: 921600
CPU Frequency 80MHz
Flash Size 4M (2M SPIFFS)

Make sure the following libraries/includes are available:
//General
Arduino.h

//Webserver
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>   // https://github.com/Links2004/arduinoWebSockets

// Additions for static pages
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <FS.h>   // Include the SPIFFS library

Plug-Ins
ESP8266 Sketch Data Upload  (see also the tutorial: https://www.instructables.com/id/Using-ESP8266-SPIFFS/)
https://github.com/esp8266/arduino-esp8266fs-plugin

