#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

#include <Wire.h>

#define MAX_NUMBER_SLAVES 30
#define MAX_INPUT_SIZE 30
#define MAX_COMMAND_NUMBER 20
char theMessage_chararray[MAX_INPUT_SIZE+1];
byte slaves[MAX_NUMBER_SLAVES+1];
byte count = 0;

int msg[1];
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;
int lastmsg = 1;

String theMessage_string = "";

void setup(void){
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  
  Wire.begin();//join I2C bus, no dress as I am the master
  Serial.println ("I2C scanner. Scanning ...");

 
   for (byte i = 1; i < 20; i++)
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

bool isExist(byte *p, byte n, byte f )
{
    bool result;
    if (n <= 0)
    {
     result = false; 
    }
    else
    {
      while (n >0)
      {
        if (*p == f)
        {
          result = true;
          break;
        }
        p++;
        n--;
      }
    }
    
    return result;
}

void splitString (char* inputstring,  char * spliter, char * result[MAX_COMMAND_NUMBER])
{
    for(byte i = 0 ; i <MAX_COMMAND_NUMBER; i++ )
    {
        *(result+i) = NULL;
    }
    *result = strtok(inputstring , spliter ); 
    byte i = 0;
    while ( *(result+i) != NULL)
    {
        i++;
        *(result+i) = strtok(NULL, spliter );
    }
}


void receiveCommandAction(byte slaveId, char * commandType, char * messageValue)
{
      Serial.print("slaveId is ");Serial.println(slaveId);
      Serial.print("Command Char is ");Serial.println(commandType);//print the command char, which is C,G,S
      Serial.print("Command Value is ");Serial.println(messageValue);//print the joint angle

                    

      if (*commandType=='S')//sending the message here
      {
             if (isExist(slaves, count, slaveId) == true )
             {
                    Wire.beginTransmission(slaveId);
                    Wire.write(*commandType);
                    Wire.write(":");
                    Wire.write(messageValue);
                    Wire.endTransmission();
                    Serial.println("Message send successflly");
                    delay(10);
               }
       }
       else// unsupported
       {}
}

void loop(void)
{
  if (radio.available())
  {
      bool done = false;  
      /*done = radio.read(msg, 1);*/      
      radio.read(msg, 1);
      char theChar = msg[0];
      if (msg[0] != 2){
        theMessage_string.concat(theChar);
        }
      else
      {
              char theMessage_chararray[theMessage_string.length() + 1];
              strcpy(theMessage_chararray,theMessage_string.c_str() );
              Serial.println(theMessage_chararray); //print out the string from NRF24L01 wireless sensor

              char * commands[MAX_COMMAND_NUMBER];

              splitString(theMessage_chararray, "&", commands);

              byte id = 0;
              while ( commands[id] != NULL )
              {
                  Serial.print("This is the ");Serial.print( id+1); Serial.println(" th command");
                  char * tempCommand = commands[id];
                  Serial.print("command is ");Serial.println(tempCommand);

                  char * values[MAX_COMMAND_NUMBER];
                  splitString(tempCommand, ":",values);
                  byte valuesNumber = 0;   // indicates how many parts the current command can be splitted to by :
                  while ( commands[valuesNumber] != NULL )
                  {
                    valuesNumber++; 
                  }
                  if (valuesNumber != 3) // not correct format 
                  {  
                  }
                  else
                  {
                    receiveCommandAction( atoi(values[0]), values[1], values[2]);
                  }
                  
                  id++;
              }
  
       theMessage_string= "";
      }
  }
}





/*void breakdown()
{
  Serial.print(theMessage); //print out the string from NRF24L01 wireless sensor
  char *command = strtok(theMessage, "&"); // divide string by &   
  //8:S:3600
  
  while(command != 0)  //split the command into two values
  {
    char *message=strchr(command, ':'); // why it's '', not ""
    if (message != 0)  //actually split the string in 2: replace ':' with 0
    {
      *message = 0;
      int slaveId = atoi(command); // get the slave number
      Serial.print("slaveId is ");Serial.println(slaveId); //print out the slave Id
      ++message;//next splited string
      Serial.print("message is ");Serial.println(message);
      
      char * messageValue = message+2;// store the value; set the pointer to the char in the array where the value starts
      char * token = strtok(message,":");
      
      char command = *token;
      Serial.print("Command Char is ");Serial.println(command);//print the command char, which is C,G,S
      Serial.print("Command Value is ");Serial.println(messageValue);//print the joint angle
      if (command)=='S'//sending the message here
      {
        Wire.beginTransmission(slaveId);
        Wire.write(command);
        Wire.write(":");
        Wire.write(messageValue);
        Wire.endTransmission();
        Serial.println("Message send successflly")
        delay(10);
      }
      else// unsupported
      {
        
      }      
    }
  command = strtok(0, "&");//Find the next command in iput string  
  }
}*/
  
