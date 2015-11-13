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
    byte adressByte = inputString[0];
    Serial.print("givenbyte:");
    Serial.println(adressByte);
    
    if(adressByte == 56){ //ascii 56 = 8, 57=9
        Serial.println("adress 8");
         Wire.beginTransmission(8); // transmit to device #44 (0x2c)
                              // device address is specified in datasheet
         Wire.write("Hello8");             // sends value byte  
         Wire.endTransmission();     // stop transmitting
         delay(10);
       Wire.requestFrom(8,1); 
    } else if (adressByte == 48){
      trasnmitServoCommand(adressByte, 1, 127);
        //Serial.println("adress 9");
        //Wire.beginTransmission(9);
        //for(int i=1; i<inputString.length(); i++){
          // dont send address
         //Wire.write(inputString[i]);
        //}
        //Wire.endTransmission();     // stop transmitting
        //delay(10);//WITHOUT THIS IT FAILS
        //Wire.requestFrom(9, 10);
     } else{
        Serial.println("no know adress");
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
    
    byte c = Wire.read();//receive a btye as character
    if(first){
      if(c >0){
        Serial.print("FOUND RESULT: ");
      } else{
        Serial.print("NO RESULT");
      }
      first = false;
    }
    Serial.println(c);//print the character
    
  }
  
}

void trasnmitServoCommand(byte adress, byte direction, byte speed){
  byte slaveindex = adress-48;
  if(slaveindex>=0){
      Serial.println("slave index");
     Serial.println(slaveindex);
     Wire.beginTransmission(slaves[slaveindex]);
     Wire.write('d'); //send 'd' as byte
     Wire.write(direction); //direction 0 = stop, 1 = clockwise, 2 = counterclockwise
     Wire.write('s'); //send 'd' as byte
     Wire.write(speed);
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

