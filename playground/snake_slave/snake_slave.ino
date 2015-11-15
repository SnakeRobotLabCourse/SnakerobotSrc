/*
 * Receive messages from the Master, parse and execute them, and send Ack back to the Master
 */

#include <Wire.h>
#define INPUT_SIZE 2
bool address_set=false;
char input[INPUT_SIZE+1];
int angle_to_set=-99;
int time_to_set_angle=-99;

void setup() {
  Serial.begin(9600);  // start serial for giving input
  Serial.println("Please enter the address you want to set for this slave");
}

void loop() {
  if(!address_set) {
    while(Serial.available()) {
      byte input_size = Serial.readBytes(input, INPUT_SIZE);
      input[input_size]=0;
      int myAddress = atoi(input);
      Wire.begin(myAddress);
      address_set=true;
      Serial.print("Address assigned to this slave is ");Serial.println(myAddress);
      delay(500);
      Wire.onRequest(requestEvent);
      parseMessage(); // put this here just to demonstrate the function
      Serial.end();
      break;
    }
  }
  delay(100);
}

void parseMessage() {
  Serial.println("Analyzing the dummy message \'S:30:500\' from the master");
  // taking a fixed string here "S:30:500", which means 'S'et your angle to 30 degrees in 500 milli seconds
  char message[9]="S:30:500";
  // read each part of the message
  char* token=strtok(message, ":");
  char command=*token;
  Serial.print("Command Char-");Serial.println(command);
  // if command is 'S'
  if(command=='G') { // don't need to read anymore as it's just a get command from master and no more input is expected in this message
    requestEvent();
  }
  else {
    int i=0;
    while(token != NULL) {
      token = strtok(NULL, ":");
      if(i==0) {
        angle_to_set=atoi(token);
        i++;
      }
      else if(i==1) {
        time_to_set_angle=atoi(token);
        break;
      }
    }
    Serial.print("Angle to set to-");Serial.println(angle_to_set);
    Serial.print("Time (in ms) in which to set the Angle to-");Serial.println(time_to_set_angle);

    // SET THE SERVO ANGLE HERE AND SEND AN ACK TO THE MASTER
    
  }
}


//function that executes whenever data is requested by master
//this function is registered as an event, see setup()
void requestEvent() {
  Wire.write("Replace this text with the current Angle");  //respond with message containing angle as expected by master
}

