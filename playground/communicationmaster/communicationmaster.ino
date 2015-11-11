#include<Wire.h>

void setup() {
 
   Wire.begin();
   Serial.begin(9600);
  // put your setup code here, to run once:
  //pinMode(13, OUTPUT);
  digitalWrite(13, LOW);   // turn the LED on (HIGH is the voltage level)
           
}

void loop() {

  Wire.requestFrom(8,1); //request 1 byte (currentAngle) from slave device #8
  //adress 0-7 are reserved
  Serial.println("check1");
  while(Wire.available()){//slave may send less then requested
  //digitalWrite(13, HIGH);    // turn the LED off by making the voltage LOW
    
    byte c = Wire.read();//receive a btye as character
    Serial.print(c);//print the character
    
  }
  Serial.println("\nend1");
  delay(3000);

  Wire.requestFrom(9,1); //request 1 byte (currentAngle) from slave device #8
  //adress 0-7 are reserved
  Serial.println("check2");
  while(Wire.available()){//slave may send less then requested
  //digitalWrite(13, HIGH);    // turn the LED off by making the voltage LOW
    
    byte c = Wire.read();//receive a btye as character
    Serial.print(c);//print the character
    
  }
  Serial.println("\nend2");
  delay(3000);
}
