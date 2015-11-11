#include<Wire.h>

String inputString = "";
boolean inputComplete;


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
    Serial.println("givvenbyte");
    Serial.println(adressByte);
    
    inputComplete = false;
    inputString = "";
    if(adressByte == 56){ //ascii 56 = 8, 57=9
        Serial.println("adress 8");
       Wire.requestFrom(8,1); 
       read();
    } else if (adressByte == 57){
        Serial.println("adress 9");
       Wire.requestFrom(9,1); 
       read();
    } else{
        Serial.println("no know adress");
    }
  }

  delay(3000);
}

void read(){
  while(Wire.available()){//slave may send less then requested
  //digitalWrite(13, HIGH);    // turn the LED off by making the voltage LOW
    
    byte c = Wire.read();//receive a btye as character
    Serial.print(c);//print the character
    
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

