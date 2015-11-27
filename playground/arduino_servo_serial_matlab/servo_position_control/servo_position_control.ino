/* Snake Robot 04.11.2015
 * Servo Position Control using MLX90316
 */

#include <MLX90316.h>  //MLX90316 Library
#include <Servo.h>     //Servo Library

Servo myservo;         //Create a servo object to control
int pos=2000;           //Desired position  90 degree,STOP Point is near 1534
unsigned long previousMillis=0;                                //

int pinSS = 7;         //Encoder chip pins SS SCK MOSI
int pinSCK = 10;
int pinMOSI = 8;

int ii;                //Real Angle
float error;             //the Error 
float order;             //the order after mapping
float k1=0.4;                //P Parameter K1
int k2=1;                //P Parameter K2
int ini_angle = 2050;
MLX90316 mlx_1  = MLX90316(); 
/*****************************************************************************/
void setup()
{
  myservo.attach(9);
  Serial.begin(115200);
  //Serial.begin(9600);
  mlx_1.attach(pinSS,pinSCK, pinMOSI); 
  //Serial.println(" MLX90316 Rotary Position Sensor");
  delay(2000);
  ii = mlx_1.readAngle();
  while((ii>=ini_angle + 2) || (ii <= ini_angle - 2))
  {
    error=-1*(pos-ii);
    order=map(error,-3600,3600,-600,600)+1534;
    ii = mlx_1.readAngle();
    delay(10);
  }
}

void loop() 
{   
    unsigned long currentMillis = millis();                    //current time
    previousMillis=currentMillis;
    ii = mlx_1.readAngle(); //First step: to read the real angle position
    Serial.println(ii);      //Print the real postition on the screen
    //Serial.print(" ");      //Print the real postition on the screen
    //Serial.println("a");   //Print a letter "a" and change to the next line
    //error=-1*(pos-ii)*k1+1534;
    error=-1*(pos-ii);
    order=map(error,-3600,3600,-600,600)+1534;
    //Serial.println(order);      //Print the real postition on the screen
    myservo.writeMicroseconds(order);
    delay(10);               // waits 15ms for the servo to reach the position 

}





