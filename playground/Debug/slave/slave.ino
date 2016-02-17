/*
 * Receive messages from the Master, parse and execute them, and send Ack back to the Master
 */

#include <Wire.h>
#include <MLX90316.h>
#include<Servo.h>


//#define TIMEAWARE                                           // toggling the code to time aware mode
//#define DEBUG                                               // toggling the code to print something for debugging


/* module depended parameters */
#define SLAVEADRESS 4
#define MIDMS 1534                                            // stop servo1543 1521
#define CENTERANGLE 1883                                      // find for each module

/* parameters, array size*/
#define ANGLERESOLUTION 2                                     // resolution of updating angle
#define SAFEANGLERANGE 850                                    // max safe angle offset from center angle 
#define COMMANDARRAYSIZE 32                                   // the size of array storing recent commands
#define MESSAGEBUFFERSIZE 20                                  // the size of the temp array storing recieved command
#define PCONTTROLVALIDRANGE 200
#define SERVOSPEEDRANGE 100

#ifdef DEBUG
 #define DEBUG_PRINT(x)         Serial.print(x)
 #define DEBUG_PRINTDEC(x)      Serial.print(x, DEC)
 #define DEBUG_PRINTLN(x)       Serial.println(x)
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTDEC(x)
 #define DEBUG_PRINTLN(x) 
#endif

boolean isDetached = false;

#ifdef TIMEAWARE
int targetAngles[COMMANDARRAYSIZE];                           // array storing recent commands
unsigned long targetTimes[COMMANDARRAYSIZE];                  // array storing recent commands
int writepointer =0;                                          // pointer used to write command
int readPointer=0;                                            // pointer used to read command
unsigned long microsX;                                        // used for time-aware turing, currently not support 
unsigned long targetTimeX;                                    // used for time-aware turing, currently not support
#endif

const int DEFAULTSPEEDLEFT = MIDMS - SERVOSPEEDRANGE;         // default turning left speed 
const int DEFAULTSPEEDRIGHT = MIDMS + SERVOSPEEDRANGE;        // default turning right speed 

const int safeAngleMax = CENTERANGLE + SAFEANGLERANGE;        // the max angle in safe range
const int safeAngleMin = CENTERANGLE - SAFEANGLERANGE;        // the min angle in safe range
int targetAngle = CENTERANGLE;                                // initialize the module to center position


unsigned long previousTime;

Servo myservo;

/* define pins for angle sensor, and variable to store read angle*/
const byte pinSS = 7;
const byte pinSCK = 10;
const byte pinMOSI = 8;
int angleRead;
MLX90316 mlx_1  = MLX90316(); 



void setup() {
    Serial.begin(9600);
                        
    mlx_1.attach(pinSS,pinSCK, pinMOSI );                       // angle sensor

    myservo.attach(9);                                          // servo attached    
                          
    Wire.begin(SLAVEADRESS);                                    // join I2C bus
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);

    previousTime = millis();
    updateAngle(MIDMS);

}

void loop() {

    if( millis() > previousTime + 100)
    {
        #ifdef TIMEAWARE
        if(targetAngles[readPointer] > 0)
        {
            targetTimeX = targetTimes[readPointer];
            microsX = millis();
            if(targetTimeX < microsX){
                Serial.println("Update command!");
                targetAngle = targetAngles[readPointer];
                readPointer = (readPointer + 1) % COMMANDARRAYSIZE;
            }
        }
        #endif


      angleRead = mlx_1.readAngle();                                       // read the given angle from sensor
      DEBUG_PRINTLN(angleRead);
    
      if(angleRead > safeAngleMax + 100 || angleRead < safeAngleMin - 100)
      {
          myservo.detach();                                               // For safety, it is better to detach the servo here
          isDetached = true;
      }
      else{
          if ( isDetached ){
              myservo.attach(9);
              isDetached = false;
          }
          updateAngle(targetAngle);
      }

      
      previousTime = millis();
    
  }
    
}


void updateAngle(int tArgetAngle){ 
    
    if(tArgetAngle > safeAngleMax || tArgetAngle < safeAngleMin){
        return;
    }  
    if( abs(angleRead - tArgetAngle) < ANGLERESOLUTION){
        return;
    }

    /****************************************/
    int error = angleRead - tArgetAngle;    
    if ( abs(error) > PCONTTROLVALIDRANGE ){
        myservo.writeMicroseconds(  (  (angleRead < tArgetAngle) ? \
        DEFAULTSPEEDLEFT : DEFAULTSPEEDRIGHT ) );
    }
    else{
        int order = map(error, -PCONTTROLVALIDRANGE, PCONTTROLVALIDRANGE,\
        -SERVOSPEEDRANGE, SERVOSPEEDRANGE ) + MIDMS;
        myservo.writeMicroseconds(order);
    }
    
}




//callback which is called, when the slave receives a receive event from the master
//(master sends a command to slave, e.g. new servo angle or speed)
void receiveEvent(int howMany) {
    if (howMany == 2)
    {
        DEBUG_PRINTLN("Receive Event"); 
        targetAngle = Wire.read() << 8;
        targetAngle |= Wire.read();
        DEBUG_PRINT("Angle to set to-");DEBUG_PRINTLN(targetAngle);
    } 
    return;
}

//function that executes whenever data is requested by master 
//this function is registered as an event, see setup()
void requestEvent() {
    DEBUG_PRINTLN("RequestEvent");
    //respond with message containing angle as expected by master
    sendInt(angleRead);
 
}

void sendInt(int num){
    Wire.write(num>>8);
    Wire.write(num&255);
}


void parseMessage(char* message) {
   
    DEBUG_PRINTLN(message);
                                                                      // read each part of the message
    char* token=strtok(message, ":");
    char command=*token;
    DEBUG_PRINT("Command Char-");DEBUG_PRINTLN(command);
    int receivedAngle;
    switch (command) {
        case 'S':
            receivedAngle = atoi( strtok(NULL, ":") );
            
            targetAngle = receivedAngle;
            DEBUG_PRINT("Angle to set to-");DEBUG_PRINTLN(targetAngle);
            #ifdef TIMEAWARE
            targetAngles[writepointer]=receivedAngle;
            token = strtok(NULL, ":");
            if (token != NULL){
                targetTimes[writepointer]=atoi(token)+millis(); 
            }
            writepointer = ( writepointer + 1 ) % COMMANDARRAYSIZE;   //increase pointer value by one
            #endif
   
            break;
        default:
            DEBUG_PRINTLN("Not support, skipping...");
            break;   
  }
}





