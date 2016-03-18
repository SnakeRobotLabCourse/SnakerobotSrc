
#include "MLX90316.h"
#include  "Servo.h"
#include "Wire.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"
#include "SPI.h"
#include <SnakeRobot.h>


/* module depended parameters */
#define SLAVEADRESS 4
#define MIDMS 1522                                            // stop servo1543 1521
#define CENTERANGLE 1840                                      // find for each module
                             

/*****************************************************************/
/* task1: receive commands from master, task2: control 
the servo, task3: feedback joint angle*/
unsigned long previousTime;
byte interval = 50;
int Angle = CENTERANGLE;
int a;
SlaveModule myslave(MIDMS, CENTERANGLE);
uint8_t cmd;

/*****************************************************************/
void setup() {
                 
    Wire.begin(SLAVEADRESS);                                    // join I2C bus
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
Serial.begin(9600);
    myslave.init();
    previousTime = millis();
}
int readangle;


/*****************************************************************/
void loop() {

    if( millis() > previousTime + interval)
    {
        
      myslave.run(Angle, cmd);
      
      previousTime = millis();
    
  }
  a = myslave.readAngle();
  if (a > 0)
  {
    readangle = a;
  }
  
  Serial.println(readangle);
  delay(5);
}



/*****************************************************************/
void receiveEvent(int howMany) {
    if (howMany == 3)
    { 
       cmd = Wire.read();
       Angle = receiveInt(); 
        
    } 
}



/*****************************************************************/
void requestEvent() {
    //respond with message containing angle as expected by master
       
      sendInt(readangle);
    
 
}
