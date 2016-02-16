/*
 * Receive messages from the Master, parse and execute them, and send Ack back to the Master
 */

#include <Wire.h>
#include <MLX90316.h>
#include<Servo.h>

/* module depended parameters */
#define SLAVEADRESS 9
#define MIDMS 1543                                            // stop servo1543 1521
#define CENTERANGLE 1583                                      // find for each module

/* parameters, array size*/
#define ANGLERESOLUTION 10                                    // resolution of updating angle
#define SAFEANGLERANGE 850                                    // max safe angle offset from center angle 
#define COMMANDARRAYSIZE 32                                   // the size of array storing recent commands
#define MESSAGEBUFFERSIZE 20                                  // the size of the temp array storing recieved command


int time_to_set_angle=-99;                                    // used for time-aware turing, currently not support
int targetAngles[COMMANDARRAYSIZE];                           // array storing recent commands
unsigned long targetTimes[COMMANDARRAYSIZE];                  // array storing recent commands
int writepointer =0;                                          // pointer used to write command
int readPointer=0;                                            // pointer used to read command


const int DEFAULTSPEEDLEFT = MIDMS - 100;                     // default turning left speed 
const int DEFAULTSPEEDRIGHT = MIDMS + 100;                    // default turning right speed 

const int safeAngleMax = CENTERANGLE + SAFEANGLERANGE;        // the max angle in safe range
const int safeAngleMin = CENTERANGLE - SAFEANGLERANGE;        // the min angle in safe range
int targetAngle = CENTERANGLE;                                // initialize the module to center position

unsigned long microsX;                                        // used for time-aware turing, currently not support 
unsigned long targetTimeX;                                    // used for time-aware turing, currently not support

Servo myservo;

/* define pins for angle sensor, and variable to store read angle*/
int pinSS = 7;
int pinSCK = 10;
int pinMOSI = 8;
int ii;
MLX90316 mlx_1  = MLX90316(); 



void setup() {
    Serial.begin(9600);


                                         
    mlx_1.attach(pinSS,pinSCK, pinMOSI );                       // angle sensor

    myservo.attach(9);                                          // servo attached    
                          
    Wire.begin(SLAVEADRESS);                                    // join I2C bus
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);

}

void loop() {

    if(targetTimes[readPointer]){
        targetTimeX = targetTimes[readPointer];
        microsX = millis();
        if(targetTimeX < microsX){
            targetAngle = targetAngles[readPointer];
            readPointer = (readPointer + 1) % COMMANDARRAYSIZE;
        }
    }
 
   

    updateAngle(targetAngle);
}


void updateAngle(int tArgetAngle){ 
  
    if(tArgetAngle > safeAngleMax || tArgetAngle < safeAngleMin){

        return;
    }
  
    ii = mlx_1.readAngle();                                       // read the given angle from sensor
  
    if(ii > safeAngleMax || ii < safeAngleMin  || abs(ii - tArgetAngle) < ANGLERESOLUTION){
       // TODO: it is better to detach the servo here
        return;
    }
    myservo.writeMicroseconds(  (  (ii < tArgetAngle) ? DEFAULTSPEEDLEFT : DEFAULTSPEEDRIGHT ) );
   
}




//callback which is called, when the slave receives a receive event from the master
//(master sends a command to slave, e.g. new servo angle or speed)
void receiveEvent(int howMany) {
    Serial.println("Receive Event");
    char message[MESSAGEBUFFERSIZE];
    byte i = 0;
    while (0 < Wire.available() && i < MESSAGEBUFFERSIZE) {         // loop through all
        char c = Wire.read();                                       // receive byte as a character  
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

    switch (command) {
        case 'S':
            token = strtok(NULL, ":");
            targetAngles[writepointer]=atoi(token);
            token = strtok(NULL, ":");
            if (token != NULL){
                targetTimes[writepointer]=atoi(token)+millis(); 
            }
            Serial.print("Angle to set to-");Serial.println(targetAngles[writepointer]);
            Serial.print("Time (in ms) in which to set the Angle to-");Serial.println(time_to_set_angle);

            
            writepointer = ( writepointer + 1 ) % COMMANDARRAYSIZE;   //increase pointer value by one
    
            break;
        default:
            Serial.println("Not support, skip...");
            break;   
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






////function to begin calibrating the angle sensor
////while calibrating no other commands can be processed
//  void startCalibrating(){
//    calibrating = true;
//    minAngle = 9000;
//    maxAngle = 9000;
//  }

//void calibrate(){
//  ii = mlx_1.readAngle();
//  Serial.println(ii);
//  if(calibratingMax){//calibrate on of the sides
//    if(maxAngle > ii){
//      calibratingDiff = maxAngle -ii;
//    } else{
//       calibratingDiff = ii -maxAngle;
//    }
//    if(ii>0){
//      maxAngle = ii;
//    }
//    
//    if(calibratingDiff < 1 || ii ==-1){//if difference to last value is smalle then 1 degree, we assume we found the value
//      calibratingMax = false;//continue with the other
//      myservo.writeMicroseconds(MAXMS1);//switch direction servo
//      ii= -1;
//    } else{
//      myservo.writeMicroseconds(MINMS1);//keep turning
//    }
//  } else{//calibrate the other of the sides
//    if(minAngle > ii){
//      calibratingDiff = minAngle -ii;
//    } else{
//      calibratingDiff = ii -minAngle;
//    }
//    if(ii>0){
//      minAngle = ii;
//    }
//      
//    
//    //remember last read value
//    if(calibratingDiff < 1 || ii ==-1){//if difference to last value is smalle then 1 degree, we assume we found the value
//      calibratingMax = true;
//      calibratingFinished = true;//we are done calibrating
//      myservo.writeMicroseconds(MIDMS);//stop servo
//      Serial.print("minangle =");
//      Serial.println(minAngle);
//      Serial.print("maxangle =");
//      Serial.println(maxAngle);
//    } else{
//      myservo.writeMicroseconds(MAXMS1);//keep turning
//    }
//  }
//}


// old version
//void updateAngle(){
//  
//    Serial.println("updateAngle func");
//    ii = mlx_1.readAngle();// read the given angle from sensor
//  
//   Serial.println(ii);
//   if(ii<0){
//    Serial.println("stopping servo cause angle sensor not working");
//     myservo.writeMicroseconds(MIDMS);
//     return;
//   }
// if(targetAngle > CENTERANGLE+850 || targetAngle < CENTERANGLE-850){
//    Serial.println("stopping servo");
//     myservo.writeMicroseconds(MIDMS);
//     return;
//  }
//
//
//  
//  if(ii < targetAngle){ //if the angle is between 0 and the target angle
//    iiX = ii+3600;
//
//    curDiffLeft = iiX -targetAngle; //messure distance when turning away from targetangle going over 0
//    curDiffRight = targetAngle -ii; //messure distance when turning towardds targetangle
//    
//  } else {//(ii > targetAngle)  //if the angle is between 360 and the target angle
//    iiX = ii-3600;
//
//    curDiffLeft = ii -targetAngle;  //messure distance when turning towardds targetangle
//    curDiffRight = targetAngle -iiX;   //messure distance when turning away from targetangle going over 0
//  } 
//  if(curDiffRight > curDiffLeft){
//    if(curDiffLeft > ACCURRACY3){
//      myservo.writeMicroseconds(MAXMS1);
//    } else if(curDiffLeft > ACCURRACY2){
//      myservo.writeMicroseconds(MAXMS1);
//    } else if(curDiffLeft > ACCURRACY1){
//      myservo.writeMicroseconds(MAXMS1);
//    } else{
//      
//       myservo.writeMicroseconds(MIDMS);
//    }
//    
//  } else {//if(curDiffLeft >= curDiffRight){
//    if(curDiffRight > ACCURRACY3){
//      myservo.writeMicroseconds(MINMS1);
//    } else if(curDiffRight > ACCURRACY2){
//      myservo.writeMicroseconds(MINMS1);
//    } else if(curDiffRight > ACCURRACY1){
//      myservo.writeMicroseconds(MINMS1);
//    } else{
//       myservo.writeMicroseconds(MIDMS);
//    }
//  }
//}

