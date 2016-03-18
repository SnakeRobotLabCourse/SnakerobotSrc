#include "MLX90316.h"
#include  "Servo.h"
#include "Wire.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"
#include "SPI.h"
#include <SnakeRobot.h>

#define SPEED 100

byte counter = 0;         
Master xx(9,10, 0);

String s;

void setup(void)
{
  Serial.begin(9600);
  xx.init();  

}

void loop(void)
{
//        String s = "Test";
//        xx.RF24send(&s);
//        delay(500);
    if(Serial.available()>0)
    {
        s = Serial.readString();
    //    String s = "Test";
        xx.RF24send(&s);
    }
//        counter++;
//        if ( counter >= SPEED )
//        {
//            
//            counter = 0;
//        }
        xx.RF24receive();
      
        if ( xx.isDone() )
        {
            Serial.println(xx.getReceivedMessage());
            xx.clearReceivedMessageBuffer();
        }
        delay(5);
    
}



