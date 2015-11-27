/*
 * Receive messages from the Master, parse and execute them, and send Ack back to the Master
 */

#include <Wire.h>
#include <MLX90316.h>
#include<Servo.h>
#define INPUT_SIZE 2
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

int midms = 1533; //stop servo

int maxms3 = 2400;  //turnservo fast from 360 to 0
int minms3 = 600;   //turnservo fast from 0 to 360
int maxms2 = 1590;//turnservo slow from 360 to 0
int minms2 = 1476;//turnservo slow from 0 to 360
int maxms1 = 1553;//turnservo very slow from 360 to 0
int minms1 = 1513;//turnservo very slow from 0 to 360

int targetAngle = 800;
int tempTargetAngle;
int duration = 500;

int accurracy3 = 600; // -> bigger angle distance then this will turn fast
int accurracy2 = 300; // -> bigger angle distance then this will turn slow
int accurracy1 = 10;  // -> bigger angle distance then this will turn very slow

int minAngle = 0; //use when calibrating later
int maxAngle = 0; //use when calibrating later

int byteSending = 1;
int toTransfer = 32766;
int Shift = toTransfer;
int mask = 0xFF;
char toSend = 0;


MLX90316 mlx_1  = MLX90316(); 


void setup() {
  Serial.begin(9600);  // start serial for giving input
  Serial.println("Please enter the address you want to set for this slave");

  mlx_1.attach(pinSS,pinSCK, pinMOSI ); 

  myservo.attach(9);
  myservo.writeMicroseconds(midms);
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
      //parseMessage(); // put this here just to demonstrate the function
      //Serial.end();
      break;
    }
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
  }
  updateAngle();
  delay(100);
}

void updateAngle(){
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
  // read each part of the message
  char* token=strtok(message, ":");
  char command=*token;
  Serial.print("Command Char-");Serial.println(command);
  // if command is 'S'
  if(command=='G') { // don't need to read anymore as it's just a get command from master and no more input is expected in this message
    //requestEvent();
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
  sendInt(ii);
  //char b[4];   //declaring character array
  //String str;  //declaring string
 
  //str=String(ii); //converting integer into a string
  //str.toCharArray(b,4); //passing the value of the string to the character array
  //Serial.println(str);
  //Serial.println(ii);
  //for(int i = 0; i<4; i++){
  //   Wire.write(b[i]); 
  //}
  //respond with message containing angle as expected by master
}

void sendInt(int num){
  Wire.write((byte *)&num, sizeof(unsigned int));
}

