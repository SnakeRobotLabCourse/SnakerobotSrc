#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

#include <Wire.h>

#define MAX_NUMBER_SLAVES 30
#define MAX_INPUT_SIZE 30
#define MAX_COMMAND_NUMBER 20


char input[MAX_INPUT_SIZE+1]; 
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
    bool result = false;
    if (n <= 0)
    {
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
             if (isExist(slaves, count, slaveId) )
             {
               Serial.print("slaveId is ");Serial.println(slaveId);
                    Wire.beginTransmission(slaveId);
                    Wire.write(commandType);
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
  while (Serial.available())
  {
      bool done = false;  
      /*done = radio.read(msg, 1);*/  
  byte input_size = Serial.readBytes(input, MAX_INPUT_SIZE);    
      
              char theMessage_chararray[MAX_INPUT_SIZE];
              strcpy(theMessage_chararray, input );
              
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
                  while ( values[valuesNumber] != NULL )
                  {
                    valuesNumber++; 
                  }
                  Serial.println(valuesNumber);
                  if (valuesNumber != 3) // not correct format 
                  {  
                  }
                  else
                  {
                    receiveCommandAction( atoi(values[0]), values[1], values[2]);
                  }
                  
                  id++;
              }
  
       //theMessage_string= "";
      }
  }




