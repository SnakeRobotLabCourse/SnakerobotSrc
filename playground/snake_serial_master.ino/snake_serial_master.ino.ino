#include <Wire.h>
#define MAX_INPUT_SIZE 30
#define MAX_NUMBER_SLAVES 30

char input[MAX_INPUT_SIZE+1]; // creating a char array and not creating a 'String' object
byte slaves[MAX_NUMBER_SLAVES+1];

void setup() {
  // put your setup code here, to run once:
  Wire.begin(); //join i2c bus, no address as I'm master
  Serial.begin(9600);  //start serial for giving input
  //inputString.reserve(200); //reserve 200 bytes for inputString

Serial.println ("I2C scanner. Scanning ...");
 byte count = 0;
 
 Wire.begin();
 for (byte i = 1; i < 120; i++)
 {
   Wire.beginTransmission (i);
   if (Wire.endTransmission () == 0)
     {
     Serial.print ("Found address: ");
     Serial.print (i, DEC);
     Serial.print (" (0x");
     Serial.print (i, HEX);
     Serial.println (")");
     slaves[count] = i;
     count++;
     } // end of good response
    delay (5);  // give devices time to recover
 } // end of for loop
 Serial.println ("Done.");
 Serial.print ("Found ");
 Serial.print (count, DEC);
 Serial.println (" device(s).");
  
}

void loop() {
  serialEvent(); //gets input message from the serial port
  delay(500);
}

void readFromSlave(int slaveId, boolean calibrating) {
  if(calibrating){
    delay(5000);// give the slave time to calibrate its angle sensor
     //Requesting the angle range from a slave
   Wire.requestFrom(slaveId, 2);     // request 2 bytes from slave device #2
      
   while(Wire.available())    // slave may send less than requested
    { 
       int x = Wire.read();         
       int y = Wire.read();         
       int z = y * 256 + x;
       
       Serial.print("CR:");
       Serial.print(slaveId);
       Serial.print(":"); 
       Serial.println(z);
    }
  } else{
    //READ angle value from slave with adress 8
  
    //Requesting a angle value from a slave takes about 700µs
   Wire.requestFrom(slaveId, 2);     // request 2 bytes from slave device #2
      
   while(Wire.available())    // slave may send less than requested
    { 
       int x = Wire.read();         
       int y = Wire.read();         
       int z = y * 256 + x;    
       Serial.println(z);//0-3600
    }
  }
}

void rosCallback(String receivedMsg){
  
}

void serialEvent() {
  while(Serial.available()) {//if there is input do something
    byte input_size = Serial.readBytes(input, MAX_INPUT_SIZE);
    Serial.print("input_size: ");Serial.println(input_size);
    input[input_size]=0;
    Serial.print(input);
    // Read each command
    char* command = strtok(input, "&");
    // 8:S:3600
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

        char * messageValue = message + 2;  //store the value here (set the pointer to the char in the array where the value starts)
        char * token=strtok(message, ":");
        
        char command=*token;
        Serial.print("Command Char-");Serial.println(command);
  
        if(command=='G') { // don't need to read anymore as it's just a get
          //command from master and no more input is expected in this message
          readFromSlave(slaveId, false);
        } else if(command=='C'){
           // sending the messages here
          Wire.beginTransmission(slaveId);
          Wire.write(message);
          Wire.endTransmission();
          delay(10);
          readFromSlave(slaveId, true);
        }else if (command=='S'){
          // sending the messages here
          Wire.beginTransmission(slaveId);
          Wire.write(command);
          Wire.write(":");
          Wire.write(messageValue);
          Wire.endTransmission();
          delay(10);
        }
        else {
          //unsupporte
        }
      }
      // Find the next command in input string
      command = strtok(0, "&");
    }
  }
}
