#include <Arduino.h>
#include "index.h"
#include "support.h"

void handleIndex(){
  Serial.printf("handleIndex CALLED\n");
  if (!handleFileRead("index.html")){
    handleNotFound();
  }
}
