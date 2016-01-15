#include <Wire.h>
void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}
void loop() {
  Wire.requestFrom(8, 1);    // request 1 bytes（current angle） from slave device #8
  while (Wire.available()) { // slave may send less than requested
    byte c = Wire.read(); // receive a byte as character
    Serial.println(c);         // print the character
    if (c == 18)
    {
        digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(1000);              // wait for a second
        digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
        delay(1000); 
        Serial.println("right");  
    }
    else
    {
        digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(5000);              // wait for a second
        digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
        delay(5000); 
        Serial.println("error");  
    }
  }
  delay(500);
}

