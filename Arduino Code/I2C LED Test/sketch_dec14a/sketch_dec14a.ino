#include <Wire.h>
byte buf[3] = {12,10,18};  //speed, current angle, target angle
byte adrress = 8;
void setup() 
{
  Serial.begin(9600);
  Wire.begin(adrress);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
}
void loop() {
  delay(100);
}
// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  Wire.write(buf[1]); // respond with message of 6 bytes// as expected by master
}

