void handleTrain(){
  String tmp_str = server.arg("motor" );
  updateMotor(tmp_str);
  tmp_str = server.arg("lights" );
  updateLights(tmp_str);
  
  server.send(200, "text/plain", "Controlling The Train");
  
  Serial.print("uri=");
  Serial.println(server.uri());
  Serial.print("method=");
  Serial.println(server.method());
  Serial.print("args=");
  Serial.println(server.args());
}
