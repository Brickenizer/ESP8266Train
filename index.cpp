#include <Arduino.h>
#include "index.h"
//#include "pin_defines.h"
#include "support.h"
#include <ESP8266WebServer.h>

extern ESP8266WebServer server;
/*
 * index.html
 */
// This string holds HTML, CSS, and Javascript for the HTML5 UI.
// The browser must support HTML5 WebSockets which is true for all modern browsers.
const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
.slidecontainer {
    width: 100%;
}

.slider {
    -webkit-appearance: none;
    width: 100%;
    height: 25px;
    background: #d3d3d3;
    outline: none;
    opacity: 0.7;
    -webkit-transition: .2s;
    transition: opacity .2s;
}
input[type=range][orient=vertical]
{
    writing-mode: bt-lr; /* IE */
    -webkit-appearance: slider-vertical; /* WebKit */
    width: 8px;
    height: 175px;
    padding: 0 5px;
}

</style>
</head>
<body>
<H1>LEGOTRAIN2 Train Controls INLINE</h1>
<p>Drag the slider to control the train.</p>

<div class="controlcontainer">
  <table border=10>
  <tr>
  <td colspan=2>Motor</td>
  <td colspan=2>Lights</td>
  <td colspan=2>Sound</td>
  </tr>
  <tr>

<form method="GET" action="/index">
<!--Motor-->  
  <td>
   <input  type="range" orient="vertical" name="motor"  value="0" min=-1023 max=1023 id="motor"><br>
  Value:<br>
  <span id="motor_val"></span><br>
  <input type="submit" value="Submit"><br>
</td>
<td align=center valign=top>
  <button onclick="motorSet(1024)">100%</button><br>
  <button onclick="motorSet(768)">75%</button><br>
  <button onclick="motorSet(512)">50%</button><br>
  <button onclick="motorSet(256)">25%</button><br>
  <button onclick="motorSet(0)">OFF</button><br>
  <button onclick="motorSet(-256)">-25%</button><br>
  <button onclick="motorSet(-512)">-50%</button><br>
  <button onclick="motorSet(-768)">-75%</button><br>
  <button onclick="motorSet(-1023)">-100%</button><br>
  <br>
</td>
</form>

<!--Lights-->
<form method="GET" action="/index">
<td>
<input type="range" orient="vertical" name="lights" value="0" min=-1023 max=1023 id="lights"><br>
Value:<br>
<span id="lights_val"></span><br>
  <input type="submit" value="Submit">
</td>
<td align=center valign=top>
  <button onclick="lightsSet(100)">100%</button><br>
  <button onclick="lightsSet(75)">75%</button><br>
  <button onclick="lightsSet(50)">50%</button><br>
  <button onclick="lightsSet(25)">25%</button><br>
  <button onclick="lightsSet(0)">OFF</button><br>
  <button onclick="lightsSet(-25)">-25%</button><br>
  <button onclick="lightsSet(-50)">-50%</button><br>
  <button onclick="lightsSet(-75)">-75%</button><br>
  <button onclick="lightsSet(-100)">-100%</button><br>
  <br>
</td>
</form>

<!--Sound-->
<form method="GET" action="/index">
<td>
<input type="range" onchange="soundVolume(-1)" orient="vertical" name="volume" value="0" min=0 max=100 id="volume"><br>
Value:<br>
<span id="volume_val"></span><br>
  <input type="submit" value="Submit">
</td>
<td align=center valign=top>

<table border=1>
<tr>
  <td><button onclick="soundPlay(Class 4 med with reverb 1)">reverb</button></td>
  <td><button onclick="soundPlay(brakes)">brakes</button></td>
  <td><button onclick="soundPlay(whistle)">whistle</button></td>
</tr>

<tr>
<td><button onclick="soundPlay(99_H1_1)">99_H1_1</button></td>
<td><button onclick="soundPlay(Class 4 1_1)">Class 4 1_1</button></td>
<td><button onclick="soundPlay(ch1)">ch1</button></td>
</tr>

<tr>
<td><button onclick="soundPlay(99_H1_1)">99_H1_1</button></td>
<td><button onclick="soundPlay(Class 4 1_1)">Class 4 1_1</button></td>
<td><button onclick="soundPlay(ch1)">ch1</button></td>
</tr>

<tr>
<td><button onclick="soundPlay(99_H1_2)">99_H1_2</button></td>
<td><button onclick="soundPlay(Class 4 1_2)">Class 4 1_2</button></td>
<td><button onclick="soundPlay(ch2)">ch2</button></td>
</tr>

<tr>
<td><button onclick="soundPlay(99_H1_3)">99_H1_3</button></td>
<td><button onclick="soundPlay(Class 4 1_3)">Class 4 1_3</button></td>
<td><button onclick="soundPlay(ch3)">ch3</button></td>
</tr>

<tr>
<td><button onclick="soundPlay(99_H1_4)">99_H1_4</button></td>
<td><button onclick="soundPlay(Class 4 1_4)">Class 4 1_4</button></td>
<td><button onclick="soundPlay(ch4)">ch4</button></td>
</tr>

</table>
  
  <button onclick="soundVolume(100)">100%</button><br>
  <button onclick="soundVolume(75)">75%</button><br>
  <button onclick="soundVolume(50)">50%</button><br>
  <button onclick="soundVolume(25)">25%</button><br>
  <button onclick="soundVolume(0)">0%</button><br>
  <br>
</td>
</tr>
</table>
</form>

<form method="GET" action="/index">
<select name="sound" size=10>
<option value="99_H1_1">H1_1
<option value="99_H1_2">H1_2
<option value="99_H1_3">H1_3
<option value="99_H1_4">H1_4
<option value="Class 4 1_1">Class 4 1_1
<option value="Class 4 1_2">Class 4 1_2
<option value="Class 4 1_3">Class 4 1_3
<option value="Class 4 1_4">Class 4 1_4
<option value="Class 4 med with reverb 1">reverb
<option value="brakes">brakes
<option value="ch1">ch1
<option value="ch2">ch2
<option value="ch3">ch3
<option value="ch4">ch4
<option value="initiated">initiated
<option value="pfeiferl">pfeifer
<option value="whistle">whistle
</select>
  <input type="submit" value="SUBMIT"><br>
</form>
<form method="GET" action="/index">
<input type=hidden name="sound" id="sound_val" value="brakes">
  <input type="submit" value="SUBMIT"><br>
</form>

<A href="/index?sound=whistle">whistle</a>
</div>

<script>

var motor_slider = document.getElementById("motor");
var motor_output = document.getElementById("motor_val");
motor_output.innerHTML = motor_slider.value;
motor_slider.oninput = function() {
  motor_output.innerHTML = this.value;
}

var lights_slider = document.getElementById("lights");
var lights_output = document.getElementById("lights_val");
lights_output.innerHTML = lights_slider.value;
lights_slider.oninput = function() {
  lights_output.innerHTML = this.value;
}

function lightsSet(light_val) {
    lights_slider.value=light_val;
    lights_output.innerHTML=light_val;
}

function motorSet(motor_val) {
    motor_slider.value=motor_val;
    motor_output.innerHTML=motor_val;
}

function soundVolume(sound_vol){
  var sound_volume  = document.getElemementById("sound_volume");
  if(sound_vol < 0){
    
  }else{
    
  }
  sounds_volume.innerHTML=sound_vol;
}

function soundPlay(sound_name){
  var sound_output  = document.getElemementById("sound_val");
  sounds_output.innerHTML=sound_name;
}
</script>

</body>
</html>
)rawliteral";


void handleIndex(){
  Serial.printf("handleIndex CALLED\n");
  handleUpdate();
  
  server.send_P(200, "text/html", INDEX_HTML);
}

void handleIndex2(){
  Serial.printf("handleIndex2 CALLED\n");
  if (!handleFileRead("index.html")){
    handleNotFound();
  }
}
