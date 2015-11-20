#include<Wire.h>

String inputString = "";
boolean inputComplete;
byte slaves[5] = {8,9,10,11,12};

void setup() {
 
   Wire.begin();
   Serial.begin(9600);
  // put your setup code here, to run once:
  //pinMode(13, OUTPUT);
  digitalWrite(13, LOW);   // turn the LED on (HIGH is the voltage level)
           
}

void loop() {

  serialEvent();

  if(inputComplete){
    byte commandByte = inputString[0];
    byte adressByte = inputString[1];
    byte directionByte = inputString[2];
    byte speedByte = inputString[3];
    Serial.print("givenbyte:");
    Serial.println(adressByte);
    if(commandByte == 84){ //84 = 'T'
      trasnmitServoCommand(adressByte, directionByte, speedByte);
    } else if(commandByte == 68) { //69='D'
      requestAngle(adressByte);
    }
    
       
    inputComplete = false;
    inputString = "";
  }

  
   read();
  delay(100);
}

void read(){
  boolean first = true;
  while(Wire.available()){//slave may send less then requested
    
    char c = char(Wire.read());//receive a btye as character
    if(first){
      if(c != NULL){
        Serial.print("FOUND RESULT: ");
      } else{
        Serial.print("NO RESULT");
      }
      first = false;
    }
    if(c != NULL){
      Serial.print(c);//print the character
    }
    
    
  }
  if(!first){
    Serial.println("\nEndOfRequestanswer");
  }
  
}

void requestAngle(byte adress){
  byte slaveindex = adress-48;
  if(slaveindex>=0){
    Wire.requestFrom((int)slaves[slaveindex], 4);
  }
}

void trasnmitServoCommand(byte adress, byte direction, byte speed){
  byte slaveindex = adress-48;
  byte directionValue = direction-48;
  byte speedValue = speed-48;
  if(slaveindex>=0){
     Serial.println("slave index");
     Serial.println(slaveindex);
     Wire.beginTransmission(slaves[slaveindex]);
     Wire.write('d'); //send 'd' as byte
     Wire.write(directionValue); //direction 0 = stop, 1 = counterclockwise, 2 = clockwise
     Wire.write('s'); //send 'd' as byte
     Wire.write(speedValue);
     Wire.endTransmission();     // stop transmitting
     delay(10);
  } else{
    Serial.println("wrong slave index");
    Serial.println(slaveindex);
  }
  
 
}

void serialEvent(){
  while(Serial.available()){
      char inChar = (char)Serial.read();
      if(inChar=='\n'){
        inputComplete = true;
      } else{
        inputString+=inChar;
      }
 }
}

