#include<Wire.h>

//Status: Slave can receive (1 byte at the moment) and send data from/to master
//Next Steps: parse the received data and control the servo which is connected to the slave

String inputString = "";
boolean inputComplete;

byte buf[3] = {12,10,18};
byte address = 9;

void setup() {
  Serial.begin(9600);
  inputString.reserve(200);       //reserve 200 bytes fro the inputstring
  Wire.begin(address);
  Wire.onRequest(requestEvent);   //register the handler for a request event  (Master: Wire.requestFrom(...))
  Wire.onReceive(receiveEvent);   //register the handler for a receive event  (Master: Wire.write(...))
}

void loop() {
  delay(100);   //we dont need to do anything here, as the handlers are called on incoming events
}

//callback which is called, when the slave receives a request from the master 
//(master requests the current data from the slave, e.g. current servo angle or speed)
void requestEvent(){
  Serial.println("Pls input string");
  serialEvent();
  Wire.write(inputString.c_str());
  inputComplete = false;
  inputString = "";
} 

//method for reading serial console input
void serialEvent(){
  while(Serial.available()){
      char inChar = (char)Serial.read();
      inputString+=inChar;      //adds console input to the inputString
      if(inChar=='\n'){
        inputComplete = true;   // input string is ready if user enters "enter" 
      }
  }
}
//callback which is called, when the slave receives a receive event from the master
//(master sends a command to slave, e.g. new servo angle or speed)
  void receiveEvent(int howMany) {
  while (1 < Wire.available()) {  // loop through all but the last
    char c = Wire.read();         // receive byte as a character   
    Serial.print(c);
  }
  int x = Wire.read();            // receive byte as an integer
  Serial.println(x);
 }
