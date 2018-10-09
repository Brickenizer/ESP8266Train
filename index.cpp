#include <Arduino.h>
#include "index.h"
#include "pin_defines.h"
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
<H1>LEGOTRAIN2</H1>
<h1>Train Controls</h1>
<p>Drag the slider to control the train.</p>

<div class="controlcontainer">
  <table border=10>
  <tr>
  <td colspan=2>Motor</td>
  <td colspan=2>Lights</td>
  </tr>
<form method="GET" action="/index">
  <tr><td>
   <input  type="range" orient="vertical" name="motor"  value="0" min=-1023 max=1023 id="motor"><br>
  Value:<br>
  <span id="motor_val"></span><br>
  <input type="submit" value="Submit"><br>
</td><td align=center valign=top>
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
<form method="GET" action="/index">

<td>
<input type="range" orient="vertical" name="lights" value="0" min=-1023 max=1023 id="lights"><br>
Value:<br>
<span id="lights_val"></span><br>
  <input type="submit" value="Submit">
</td><td align=center valign=top>
  <button onclick="lightsSet(1023)">100%</button><br>
  <button onclick="lightsSet(768)">75%</button><br>
  <button onclick="lightsSet(512)">50%</button><br>
  <button onclick="lightsSet(256)">25%</button><br>
  <button onclick="lightsSet(0)">OFF</button><br>
  <button onclick="lightsSet(-256)">-25%</button><br>
  <button onclick="lightsSet(-512)">-50%</button><br>
  <button onclick="lightsSet(-768)">-75%</button><br>
  <button onclick="lightsSet(-1023)">-100%</button><br>
  <br>
</td>
</tr>
</table>
</form>

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
</script>

</body>
</html>
)rawliteral";


void handleIndex(){

  String tmp_str = server.arg("motor" );
  updateMotor(tmp_str);
  tmp_str = server.arg("lights" );
  updateLights(tmp_str);
  
  server.send_P(200, "text/html", INDEX_HTML);
  Serial.print("uri=");
  Serial.println(server.uri());
  Serial.print("method=");
  Serial.println(server.method());
  Serial.print("args=");
  Serial.println(server.args());

}
