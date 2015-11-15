/*
 * The objective is the master gets a message through Serial, which could be to multiple slaves,  
 * this is then parsed and the messages are sent to the respective slaves
 * Input Example: "8:S:30:500&9:G"
 * This means, send message to Slave:8, to 'S'et it's angle to 30 degrees in 500 milliseconds and 
 * send message to Slave:9, to 'G'et it's current angle in degrees.
 * The char array actually sent to Slave:8 would be "S:30:500" and to Slave:9, "G" which would be
 * parsed at the slaves and executed
 * Output: The addressed slaves respond with ACKs
 */

#include <Wire.h>
#define MAX_INPUT_SIZE 30

String inputString="";
char input[MAX_INPUT_SIZE+1]; // creating a char array and not creating a 'String' object

void setup() {
  // put your setup code here, to run once:
  Wire.begin(); //join i2c bus, no address as I'm master
  Serial.begin(9600);  //start serial for giving input
  //inputString.reserve(200); //reserve 200 bytes for inputString
}

void loop() {
  serialEvent(); //gets input message from the serial port
  readFromSlaves(); // reads everything from the Wire.read
  delay(500);
}

void readFromSlaves() {
  while(Wire.available()){
    byte c=Wire.read(); //receive a byte as character
    Serial.print(c);
  }
}

void serialEvent() {
  while(Serial.available()) {
    byte input_size = Serial.readBytes(input, MAX_INPUT_SIZE);
    Serial.print("input_size: ");Serial.println(input_size);
    input[input_size]=0;
    Serial.print(input);
    // Read each command
    char* command = strtok(input, "&");
    while (command != 0) {
      // Split the command into two values
      char* message = strchr(command, ':');
      if (message != 0) {
        // Actually split the string in 2: replace ':' with 0
        *message = 0;
        int slaveId = atoi(command);
        Serial.print("slaveId-");Serial.println(slaveId);
        ++message;
        Serial.print("message-");Serial.println(message);
        
        // sending the messages here
        Wire.beginTransmission(slaveId);
        Wire.write(message);
        Wire.endTransmission();
        delay(10);
      }
      // Find the next command in input string
      command = strtok(0, "&");
    }
  }
}
