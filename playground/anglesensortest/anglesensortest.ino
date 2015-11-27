#include <MLX90316.h>
#include<Servo.h>

/* MLX90316 Rotary Position Sensor
 * KHM 2010 /  Martin Nawrath
 * Kunsthochschule fuer Medien Koeln
 * Academy of Media Arts Cologne
 */

//#include <Metro.h> //Include Metro library


Servo myservo;
int pinSS = 7;
int pinSCK = 10;
int pinMOSI = 8;
int ii;
int iiX;
int curDiffRight;
int curDiffLeft;

int midms = 1533; //stop servo

int maxms3 = 2400;  //turnservo fast from 360 to 0
int minms3 = 600;   //turnservo fast from 0 to 360
int maxms2 = 1590;//turnservo slow from 360 to 0
int minms2 = 1476;//turnservo slow from 0 to 360
int maxms1 = 1553;//turnservo very slow from 360 to 0
int minms1 = 1513;//turnservo very slow from 0 to 360

int targetAngle = 2700;
int tempTargetAngle;
int duration = 500;

int accurracy3 = 600; // -> bigger angle distance then this will turn fast
int accurracy2 = 300; // -> bigger angle distance then this will turn slow
int accurracy1 = 10;  // -> bigger angle distance then this will turn very slow

int minAngle = 0; //use when calibrating later
int maxAngle = 0; //use when calibrating later

MLX90316 mlx_1  = MLX90316(); 

void setup(){

  Serial.begin(9600);
  mlx_1.attach(pinSS,pinSCK, pinMOSI ); 

  myservo.attach(9);
  myservo.writeMicroseconds(midms);

}

void loop() {

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
    if(curDiffLeft > accurracy3){
      myservo.writeMicroseconds(maxms3);
    } else if(curDiffLeft > accurracy2){
      myservo.writeMicroseconds(maxms2);
    } else if(curDiffLeft > accurracy1){
      myservo.writeMicroseconds(maxms1);
    } else{
       myservo.writeMicroseconds(midms);
    }
    
  } else {//if(curDiffLeft >= curDiffRight){
    if(curDiffRight > accurracy3){
      myservo.writeMicroseconds(minms3);
    } else if(curDiffRight > accurracy2){
      myservo.writeMicroseconds(minms2);
    } else if(curDiffRight > accurracy1){
      myservo.writeMicroseconds(minms1);
    } else{
       myservo.writeMicroseconds(midms);
    }
  }
 
  delay(10);
}





