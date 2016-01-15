/*
 * Receive messages from the Master, parse and execute them, and send Ack back to the Master
 */

#include <Wire.h>
#include <MLX90316.h>
#include<Servo.h>
#define INPUT_SIZE 2
#define SLAVEADRESS 9
bool address_set=false;
char input[INPUT_SIZE+1];
int time_to_set_angle=-99;

Servo myservo;
int pinSS = 7;
int pinSCK = 10;
int pinMOSI = 8;
int ii;
int iiX;
int curDiffRight;
int curDiffLeft;

int midms = 1518; //stop servo

#define MAXMS3 2400  //turnservo fast from 360 to 0
#define MINMS3 600   //turnservo fast from 0 to 360
#define MAXMS2 1590//turnservo slow from 360 to 0
#define MINMS2 1476//turnservo slow from 0 to 360
#define MAXMS1 1556//turnservo very slow from 360 to 0
#define MINMS1 1495//turnservo very slow from 0 to 360

#define ACCURRACY3 600 // -> bigger angle distance then this will turn fast
#define ACCURRACY2 300 // -> bigger angle distance then this will turn slow
#define ACCURRACY1 30  // -> bigger angle distance then this will turn very slow


int targetAngle = 800;
int tempTargetAngle;
int duration = 500;

int calibratingDiff;
int minAngle = 9000; //use when calibrating later
int maxAngle = 9000; //use when calibrating later
boolean calibratingMax = true;
boolean calibrating = false;
boolean calibratingFinished = false;
boolean calibratingDiffOnceBigger = false;

int byteSending = 1;
int toTransfer = 32766;
int Shift = toTransfer;
int mask = 0xFF;
char toSend = 0;

boolean lastMessage = false;

MLX90316 mlx_1  = MLX90316(); 


void setup() {
  Serial.begin(9600);  // start serial for giving input
  
  mlx_1.attach(pinSS,pinSCK, pinMOSI ); 

  myservo.attach(9);
  myservo.writeMicroseconds(midms);

  Wire.begin(SLAVEADRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  startCalibrating();
}

void loop() {
  if(calibrating && !calibratingFinished){
    calibrate();
  } else{
    updateAngle();
  }
  delay(100);
}

void calibrate(){
  ii = mlx_1.readAngle();
  if(calibratingMax){//calibrate on of the sides
    if(maxAngle > ii){
      calibratingDiff = maxAngle -ii;
    } else{
       calibratingDiff = ii -maxAngle;
    }
    maxAngle = ii;
    if(calibratingDiff < 1){//if difference to last value is smalle then 1 degree, we assume we found the value
      calibratingMax = false;//continue with the other
      myservo.writeMicroseconds(MAXMS2);//switch direction servo
      ii= -1;
    } else{
      myservo.writeMicroseconds(MINMS2);//keep turning
    }
  } else{//calibrate the other of the sides
    if(minAngle > ii){
      calibratingDiff = minAngle -ii;
    } else{
      calibratingDiff = ii -minAngle;
    }
      minAngle = ii;
    
    //remember last read value
    if(calibratingDiff < 1){//if difference to last value is smalle then 1 degree, we assume we found the value
      calibratingMax = true;
      calibratingFinished = true;//we are done calibrating
      myservo.writeMicroseconds(midms);//stop servo
      Serial.print("minangle =");
      Serial.println(minAngle);
      Serial.print("maxangle =");
      Serial.println(maxAngle);
    } else{
      myservo.writeMicroseconds(MAXMS2);//keep turning
    }
  }
}

void updateAngle(){
  ii = mlx_1.readAngle();// read the given angle from sensor
  if(ii < targetAngle){ //if the angle is between 0 and the target angle
    iiX = ii+3600;

    curDiffLeft = iiX -targetAngle; //messure distance when turning away from targetangle going over 0
    curDiffRight = targetAngle -ii; //messure distance when turning towardds targetangle
    
  } else {//(ii > targetAngle)  //if the angle is between 360 and the target angle
    iiX = ii-3600;

    curDiffLeft = ii -targetAngle;  //messure distance when turning towardds targetangle
    curDiffRight = targetAngle -iiX;   //messure distance when turning away from targetangle going over 0
  } 
  if(curDiffRight > curDiffLeft){
    if(curDiffLeft > ACCURRACY3){
      myservo.writeMicroseconds(MAXMS3);
    } else if(curDiffLeft > ACCURRACY2){
      myservo.writeMicroseconds(MAXMS2);
    } else if(curDiffLeft > ACCURRACY1){
      myservo.writeMicroseconds(MAXMS1);
    } else{
       myservo.writeMicroseconds(midms);
    }
    
  } else {//if(curDiffLeft >= curDiffRight){
    if(curDiffRight > ACCURRACY3){
      myservo.writeMicroseconds(MINMS3);
    } else if(curDiffRight > ACCURRACY2){
      myservo.writeMicroseconds(MINMS2);
    } else if(curDiffRight > ACCURRACY1){
      myservo.writeMicroseconds(MINMS1);
    } else{
       myservo.writeMicroseconds(midms);
    }
  }
}

//callback which is called, when the slave receives a receive event from the master
//(master sends a command to slave, e.g. new servo angle or speed)
void receiveEvent(int howMany) {
  Serial.println("Receive Event");
  char message[12];
  int i = 0;
  while (0 < Wire.available()) {  // loop through all
    char c = Wire.read();         // receive byte as a character  
     message[i] = c;
     i++;
    Serial.print(c);
  }
  Serial.println();
  parseMessage(message);
 }

void parseMessage(char* message) {
  lastMessage = message;
  Serial.println(message);
  // read each part of the message
  char* token=strtok(message, ":");
  char command=*token;
  Serial.print("Command Char-");Serial.println(command);
  // if command is 'S'
  if(command=='G') { // don't need to read anymore as it's just a get command from master and no more input is expected in this message
    //requestEvent();
  } else if(command=='C'){
    startCalibrating();
    Serial.println("Starting calibration");
  }
  else {
    int i=0;
    while(token != NULL) {
      token = strtok(NULL, ":");
      if(i==0) {
        targetAngle=atoi(token);
        i++;
      }
      else if(i==1) {
        time_to_set_angle=atoi(token);
        break;
      }
    }
    Serial.print("Angle to set to-");Serial.println(targetAngle);
    Serial.print("Time (in ms) in which to set the Angle to-");Serial.println(time_to_set_angle);

    // SET THE SERVO ANGLE HERE AND SEND AN ACK TO THE MASTER
    
  }
}

//function to begin calibrating the angle sensor
//while calibrating no other commands can be processed
void startCalibrating(){
  calibrating = true;
  minAngle = 9000;
  maxAngle = 9000;
}

//function that executes whenever data is requested by master 
//this function is registered as an event, see setup()
void requestEvent() {
  Serial.println("RequestEvent");
  if(calibrating){
    sendInt(1800);
    calibrating = false;
  } else{
    //respond with message containing angle as expected by master
    //sendInt(ii);
    sendInt(3600);
  }
}

void sendInt(int num){
  Wire.write((byte *)&num, sizeof(unsigned int));
}

