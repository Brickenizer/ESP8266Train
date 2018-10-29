#include <ESP8266WebServer.h>
#include "chug.h"

#include "support.h"
#include "pin_defines.h"

extern ESP8266WebServer server;
extern Chug *chug;

extern float motor_pct;
extern bool  motor_state;
extern float lights_pct;
extern bool  lights_state;
extern float volume_pct;
extern bool  volume_state;

void writeToMotorPct(float velocity_pct_in);
void writeToLightsPct(float lights_pct_in, bool lights_state_in);

void setupMotorShield(){
  // 4 pins control the motor shield.
  // Pins 4 and 5 control the speed of the motors with PWM in a default range of 0..1023
  // while pins 0 and 2 define the direction the motors will turn.
  // The second motor power source will be used for the cabin lights.
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(MOTOR_DIR_PIN, OUTPUT);

  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_DIR_PIN, OUTPUT);

  writeToMotorPct(0);
  writeToLightsPct(100,true);
  writeToLightsPct(100,false);

}
void writeToMotorPct(float velocity_pct){
    if(velocity_pct > 100) 
      velocity_pct=100;
    else if(velocity_pct < -100)
      velocity_pct=-100;

    float speed = (velocity_pct>0)?velocity_pct:-velocity_pct;
    chug->SetSpeed((26*speed)/100.0);
    Serial.printf("SpeedMPH = %f \n", chug->GetSpeedMPH());

    motor_pct = velocity_pct;
    int write_val=(speed * 1023.0)/100.0;
    analogWrite(MOTOR_PIN, write_val);
    digitalWrite(MOTOR_DIR_PIN, (motor_pct>0)?LOW:HIGH);
    Serial.printf("motor_pct = %f\n",motor_pct);
}
void updateMotorPct(String motor_str){
  if(motor_str.length()){
    Serial.printf("motor= %s\n",motor_str.c_str());

    float velocity_pct=motor_str.toFloat();
    writeToMotorPct(velocity_pct);
  }
}
void updateMotor(String motor_str, float range_max){
  if((-0.001 < range_max) && (range_max < 0.001)){
    return;
  }
  if(motor_str.length()){
    Serial.printf("motor= %s\n",motor_str.c_str());

    float velocity_pct=motor_str.toFloat();
    velocity_pct = velocity_pct*100.0/range_max;
    writeToMotorPct(velocity_pct);
  }  
}
void writeToLightsPct(float lights_pct_in, bool lights_state_in){    
    float velocity=lights_pct_in;
    if(velocity > 100) 
      velocity=100;
    else if(velocity < -100)
      velocity=-100;

    float speed = (velocity>0)?velocity:-velocity;
    if(lights_state_in == true){
      lights_pct = velocity;
      lights_state = true;
    }else{
      speed = 0;
      lights_state = false;
    }
    int write_val=(speed * 1023.0)/100.0;
    Serial.printf("writeToLightsPct: lights_pct= %f - lights_pct\n",lights_pct);
    Serial.printf("writeToLightsPct: write_val= %f - write_val\n",write_val);
    analogWrite(LED_PIN, write_val);
    digitalWrite(LED_DIR_PIN, (lights_pct>0)?LOW:HIGH);
}

void updateLightsState(String lights_state_str){
  if(lights_state_str){
    String tmp_str=lights_state_str;
    tmp_str.toLowerCase();
    bool tf_state=false;
    if((tmp_str == "on") ||
    (tmp_str == "1") ||
    (tmp_str == "true")){
    tf_state=true;
  }
  writeToLightsPct(lights_pct, tf_state);  
}
}

void updateLightsPct(String lights_str){
  if(lights_str.length()){
    Serial.printf("updateLightsPct: lights= %s\n",lights_str.c_str());

    float velocity_pct=lights_str.toFloat();
    bool vel_state=(velocity_pct == 0)?false:true;
    Serial.printf("updateLightsPct: lights= %f - velocity_pct\n",velocity_pct);
    writeToLightsPct(velocity_pct, vel_state);
  }
}

void updateLights(String lights_str, float range_max){
  if((-0.001 < range_max) && (range_max < 0.001)){
    return;
  }
  if(lights_str.length()){
    Serial.printf("lights= %s\n",lights_str.c_str());

    float lights_pct=lights_str.toFloat();
    lights_pct = lights_pct*100.0/range_max;
    writeToLightsPct(lights_pct, true);
  }  
}

void updateSound(String sound_str){
  if(sound_str.length()){
    Serial.print("sound= ");
    Serial.print ( sound_str  ) ;
    Serial.print(" = ");    
    Serial.print ( sound_str.toInt()  ) ;
    Serial.println("");
    String tmp_sound="/"+sound_str+".wav";
    if(tmp_sound.length()> 5){
     chug->BeginSFX(tmp_sound.c_str());
    }
  }
}

void updateVolume(String volume_str){
  if(volume_str.length()){
    Serial.print("volume= ");
    Serial.print ( volume_str  ) ;
    Serial.print(" = ");    
    Serial.print ( volume_str.toInt()  ) ;
    Serial.println("");
    
    float volume_tmp=volume_str.toFloat();
    volume_tmp = (volume_tmp>0)?volume_tmp:-volume_tmp;
    if(volume_tmp > 100) 
      volume_tmp=100;
    else if(volume_tmp < 0)
      volume_tmp=0;
    
    volume_pct = volume_tmp;
    chug->SetGainPCT(volume_pct);
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


void handleNotFound(){
  // If the client requests any URI
  // send it if it exists
  if (handleFileRead(server.uri())) {
    return;
  } else {
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
    // server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  }  
}

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  else if (filename.endsWith(".svg")) return "image/svg+xml";
  
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}

void handleUpdate(){
  Serial.printf("handleUpdate-start\n");
    Serial.print("uri=");
  Serial.println(server.uri());
  Serial.print("method=");
  Serial.println(server.method());
  Serial.print("args=");
  Serial.println(server.args());

  String message="";
  for (int i=0; i< server.args(); ++i){
    message += "Arg " + String(i) + " -> ";
    message += server.argName(i) + ": ";
    message += server.arg(i) + "\n";
  }
  
  Serial.printf("%s\n",message.c_str());
  // MOTOR
  String tmp_str = server.arg("motor" );
  if(tmp_str.length() == 0){
    tmp_str = server.arg("motor_pct");
  }
  if(tmp_str.length()>0){
    updateMotorPct(tmp_str);
    Serial.printf("motor = %s\n",tmp_str.c_str());
  }

  //LIGHTS
  tmp_str = server.arg("lights" );
  if(tmp_str.length() == 0){
    tmp_str = server.arg("lights_pct");
  }
  if(tmp_str.length()>0){
    updateLightsPct(tmp_str);
    Serial.printf("lights = %s\n",tmp_str.c_str());
  }

  tmp_str = server.arg("lights_state");
  if(tmp_str.length() > 0){
    updateLightsState(tmp_str);
    Serial.printf("lights_state = %s\n", tmp_str.c_str());
  }

  //SOUND
  tmp_str = server.arg("sound" );
  if(tmp_str.length() == 0){
    tmp_str = server.arg("sound_name");
  }
  if(tmp_str.length()>0){
    updateSound(tmp_str);
    Serial.printf("sound = %s\n",tmp_str.c_str());
  }

    //VOLUME
  tmp_str = server.arg("volume" );
  if(tmp_str.length() == 0){
    tmp_str = server.arg("volume_pct");
  }
  if(tmp_str.length()>0){
    updateVolume(tmp_str);
    Serial.printf("volume = %s\n",tmp_str.c_str());
  }


  //Simple REST catch for Internet of LEGO NODE RED protocol
  if(server.uri() == "/motor"){
    // get the params
    String tmp_str = server.arg("params" );
    if(tmp_str.length() > 0){
      updateMotor(tmp_str, 1024.0);
    }
  }
  
  if(server.uri() == "/lights"){
    // get the params
    String tmp_str = server.arg("params" );
    if(tmp_str.length() > 0){
      updateLights(tmp_str, 1024.0);
    }
  }
  
  Serial.printf("handleUpdate-end\n");
}
void handleUpdateJSON(){
  Serial.printf("handleUpdateJSON-start\n");
  handleUpdate();
  // form JSON return
  String ret_string;
  ret_string  = "{\n";
  ret_string += "\"motor_pct\": \""  + String(motor_pct)  + "\",\n";
  ret_string += "\"lights_pct\": \"" + String(lights_pct) + "\",\n";
  ret_string += "\"volume_pct\": \"" + String(volume_pct) + "\",\n";
//compatibility with Internet of LEGO Node red - begin
//{"return_value": 1, "id": "1", "name": "Horizon Express", "hardware": "esp8266", "connected": true}
  ret_string += "\"return\": \""     + String("1")        + "\",\n";
  ret_string += "\"id\": \""         + String("1")        +  "\",\n";
  ret_string += "\"name\": \""       + String("Steam Engine") + "\",\n";
  ret_string += "\"hardware\": \""   + String("esp8266")  + "\",\n";
  ret_string += "\"connected\": "    + String("true")     + "\n";
//compatibility with Internet of LEGO Node red - end
  ret_string += "}\n";
  server.send(200, "application/json", ret_string);
  Serial.printf("after set\n");
  Serial.printf("motor_pct = %f\n", motor_pct);
  Serial.printf("lights_pct = %f\n", lights_pct);
  Serial.printf("volume_pct = %f\n", volume_pct);
  Serial.printf("JSON string =%s\n",ret_string.c_str());
  Serial.printf("handleUpdateJSON-end\n");
}
