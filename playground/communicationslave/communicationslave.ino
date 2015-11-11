#include<Wire.h>

String inputString = "";
boolean inputComplete;

byte buf[3] = {12,10,18};
byte adrress = 8;

void setup() {
  Serial.begin(9600);
  inputString.reserve(200);//reserve 200 bytes fro the inputstring
  
  Wire.begin(adrress);
  Wire.onRequest(requestEvent);
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
}

void requestEvent(){
  Serial.println("Pls input string");
  serialEvent();
  Wire.write(inputString.c_str());//respond with message of 6 bytes as expected by master
  inputComplete = false;
  inputString = "";

}

void serialEvent(){
  while(Serial.available()){
      char inChar = (char)Serial.read();
      inputString+=inChar;

      if(inChar=='\n'){
        inputComplete = true;
      }
  }

 
}
