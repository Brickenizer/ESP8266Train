#ifndef _SUPPORT_H
#define _SUPPORT_H

void setupMotorShield();
void updateLights(String lights_str);
void updateMotor(String motor_str);
void updateSound(String sound_str);

//For FS html files
String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);       // send the right file to the client (if it exists)


void handleNotFound();
void handleInline();
void handleRoot();
void handleUpdate();
void handleUpdateJSON();

#endif
