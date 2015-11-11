#include<Wire.h>

String inputString = "";
boolean inputComplete;

byte buf[3] = {12,10,18};
byte adrress = 9;

void setup() {
  Serial.begin(9600);
  inputString.reserve(200);//reserve 200 bytes fro the inputstring
  Wire.begin(adrress);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

void loop() {
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
  void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    //String resultCharString = "Slave 9 received char " + c;
    //Serial
    
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  //String resultIntString = "Slave 9 received int " + x;
  Serial.println(x);
  
  //serialEvent();
  //Serial.println(inputString.c_str());
  //Wire.write("blub");//respond with message of 6 bytes as expected by master
  //inputComplete = false;
  //inputString = "";
}
 
