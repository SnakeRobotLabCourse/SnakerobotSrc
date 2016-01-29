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

#define MIDMS 1535 //stop servo1543 1521
//1534

#define MAXMS3 (MIDMS+15) //turnservo fast from 360 to 0
#define MINMS3 (MIDMS-15) //turnservo fast from 0 to 360
#define MAXMS2 (MIDMS+12) //turnservo slow from 360 to 0
#define MINMS2 (MIDMS-12)//turnservo slow from 0 to 360
#define MAXMS1 (MIDMS+10)//turnservo very slow from 360 to 0
#define MINMS1 (MIDMS-10)//turnservo very slow from 0 to 360

#define ACCURRACY3 600 // -> bigger angle distance then this will turn fast
#define ACCURRACY2 300 // -> bigger angle distance then this will turn slow
#define ACCURRACY1 30  // -> bigger angle distance then this will turn very slow


int targetAngle = 2000;
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
  // myservo.writeMicroseconds(MIDMS);   
// I suggest delete this line, because it is possible that we forgot to change the stop point when this program is 
// uploaded to another module having different stop point.
  Wire.begin(SLAVEADRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  
}

void loop() {
   updateAngle();
   //if(targetAngle > 2700 || targetAngle < 900){
   // some module may cannot reach 2700 or 900, for example 800 to 2600 or 1000 to 2800
   if(targetAngle > 2600 || targetAngle < 1000){
    Serial.println("writing stopX");
     myservo.writeMicroseconds(MIDMS);
     return;
  }
  //TODO: stop servo if angle is out of possible range
  delay(50);
}

void updateAngle(){
  
    Serial.println("updateAngle func");
  ii = mlx_1.readAngle();// read the given angle from sensor
  
   Serial.println(ii);
   if(ii<0){
    Serial.println("stopping servo cause angle sensor not working");
     myservo.writeMicroseconds(MIDMS);
     return;
   }
 // if(targetAngle > 2700 || targetAngle < 900){
 if(targetAngle > 2600 || targetAngle < 1000){
    Serial.println("stopping servo");
     myservo.writeMicroseconds(MIDMS);
     return;
  }
//TODO: return if target angle to big or small
  
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
      Serial.println("min1");
      myservo.writeMicroseconds(MAXMS1);
    } else if(curDiffLeft > ACCURRACY2){
      Serial.println("min2");
      myservo.writeMicroseconds(MAXMS1);
    } else if(curDiffLeft > ACCURRACY1){
      Serial.println("min3");
      myservo.writeMicroseconds(MAXMS1);
    } else{
      
  Serial.println("writing stop1");
       myservo.writeMicroseconds(MIDMS);
    }
    
  } else {//if(curDiffLeft >= curDiffRight){
    if(curDiffRight > ACCURRACY3){
       Serial.println("max1");
      myservo.writeMicroseconds(MINMS1);
    } else if(curDiffRight > ACCURRACY2){
       Serial.println("max2");
      myservo.writeMicroseconds(MINMS1);
    } else if(curDiffRight > ACCURRACY1){
       Serial.println("max3");
      myservo.writeMicroseconds(MINMS1);
    } else{
      Serial.println("writing stop2");
       myservo.writeMicroseconds(MIDMS);
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
  Serial.println(message);
  // read each part of the message
  char* token=strtok(message, ":");
  char command=*token;
  Serial.print("Command Char-");Serial.println(command);
  // if command is 'S'
  if(command=='G') { // don't need to read anymore as it's just a get command from master and no more input is expected in this message
    //requestEvent();
  } else if(command=='C'){
    //startCalibrating();
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

//function that executes whenever data is requested by master 
//this function is registered as an event, see setup()
void requestEvent() {
  Serial.println("RequestEvent");
    //respond with message containing angle as expected by master
    sendInt(ii);
}

void sendInt(int num){
  Wire.write((byte *)&num, sizeof(unsigned int));
}

