#ifndef _SUPPORT_H
#define _SUPPORT_H

void updateLights(String lights_str);
void updateMotor(String motor_str);
void updateSound(String sound_str);

void handleNotFound();
void handleControl();
void handleInline();
void handleSound();
void handleRoot();

#endif
