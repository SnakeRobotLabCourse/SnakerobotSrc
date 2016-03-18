#include "MLX90316.h"
#include  "Servo.h"
#include "Wire.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"
#include "SPI.h"
#include <SnakeRobot.h>

#define SPEED 100

int counter = 0;
Master xx(9,10, 1);
bool done =false;
String tosend;
unsigned long timestamp;
unsigned long timeslot;
unsigned long timestamp2;
unsigned long time2=0;
unsigned long time3=0;
unsigned long timestamp4;
unsigned long time4=0;
void setup(void){
  Serial.begin(57600);
  Serial.setTimeout(5);
  xx.init();
  
timestamp = millis();
}

void loop(void)
{
          String s="";
        
        
          done =false;
          if (Serial.available()>0)
          {
            timestamp4 = millis();
            s = Serial.readString();
            char bb[s.length()+1];
            strcpy(bb, s.c_str());
            

            String c(bb);
           // Serial.println(c);
		        timestamp2 = millis();
            time3=xx.forwardToSlaves(&c);
			      time2 = millis() - timestamp2;
            time4 = millis() - timestamp4;			
            done = true;
          }

          

          if ( counter >= 15)
         {
          xx.collectFromSlaves();
          tosend = xx.getCollectedMessage();
          timeslot = millis() - timestamp;
          timestamp = millis();
          tosend.concat('&');
          tosend.concat(timeslot);
		      tosend.concat('&');
          tosend.concat(time2);
          tosend.concat('&');
          tosend.concat(time3);
          tosend.concat('&');
          tosend.concat(time4);
          Serial.println(tosend);
          xx.clearMessageToSendBuffer();
          counter =0;
         }
  
counter++;


//  delay(5);
}



