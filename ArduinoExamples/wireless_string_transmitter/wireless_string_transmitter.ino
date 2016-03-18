#include "MLX90316.h"
#include  "Servo.h"
#include "Wire.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"
#include "SPI.h"
#include <SnakeRobot.h>

#define SPEED 100

byte counter = 20;         
Master xx(9,10, 0);



void setup(void)
{
  Serial.begin(9600);
  Serial.setTimeout(5);
  xx.init();  

}
bool done = false;
void loop(void)
{

        String s="";
        
        

          if (Serial.available()>0)
          {
            s = Serial.readString();
            char bb[s.length()+1];
            strcpy(bb, s.c_str());
            

            String c(bb);
           // Serial.println(c);
            xx.RF24send(&c);
          }

        
        






//
//        xx.RF24receive();
//      
//        if ( xx.isDone() )
//        {
//            Serial.println(xx.getReceivedMessage());
//            xx.clearReceivedMessageBuffer();
//        }
        delay(10);
    
}




