/*
		snakeutil.c ---- public function library for SnakeRobot project. 
		Created by Long Cheng, 18/02/2016, Garching.
		
*/

/* !!! note: this library supposes the client is attached to I2C bus*/
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif 
#include <snakeutil.h>

#include "Wire.h"

void sendInt(int num)
{
	uint8_t buffer[2];
	buffer[0] = num >> 8;
	buffer[1] = num & 0xff;
	Wire.write(buffer,  2);
}

int receiveInt()
{
	int ret = 0; 
	ret = Wire.read() << 8;
	ret |= Wire.read();
	return ret;
}

//		array to check  array size     number to check
bool isExist(byte p[], byte sizeofp, byte numtocheck )
{
	bool result = false;
	if (sizeofp <= 0) 		// empty array
	{
	}
	else
	{
		while (sizeofp >0)
		{
			if (*p == numtocheck )
			{
				result = true;
				break;
			}
			p++;
			sizeofp--;
		}
	}
	
	return result;
}


void splitString (char* inputstring, const char * spliter, char * result[MAX_COMMAND_NUMBER])
{
	byte i = 0;
	// initialize the result array as null
	for( ; i < MAX_COMMAND_NUMBER; i++ )
	{
		*(result+i) = NULL;
	}
	*result = strtok(inputstring , spliter ); 
	i = 0;
	// splitting
	while ( *(result+i) != NULL)
	{
		i++;
		*(result+i) = strtok(NULL, spliter );
	}
}


void scanSlaves(byte _slaves[], byte * _count)
{
	byte i = 1;
	* _count = 0; 								// initialize slave number to 0
	for (; i < MAX_NUMBER_SLAVES; i++) 			// assume at most 20 slaves
	{
		//Serial.println(i);       	// remove comments for debugging
		Wire.beginTransmission (i);
		if (Wire.endTransmission () == 0)
		{
			//Serial.println("found");
			* _slaves = i; 			//record found device address 
			(* _count) ++;
			_slaves++;
		} 							// end of good response
		delay (5);  				// give devices time to recover
	}
	
}

unsigned long masterForwardCommands(byte slaveId, char * commandType, int messageValue, byte slaves[], byte count)
{       
	unsigned long s=millis();  		// record enter time stamp
	unsigned long ret;

	
	// sending the message
	switch (*commandType)
	{
	case 'S':
		sendingCommands(slaveId, 0x00, messageValue, slaves, count);
		break;
	case 'R':
		sendingCommands(slaveId, 0x1, messageValue, slaves, count);
		break;
	default:
		break;
	}
	ret = millis() - s;
	return ret;
}

/* this function sends three bytes data, the type and the messageValue, 
to the slave slaveId attached on I2C bus*/
void sendingCommands(byte slaveId, uint8_t type, int messageValue, byte slaves[], byte count)
{
	if (isExist(slaves, count, slaveId) )
	{	
		uint8_t cmd[1] ;
		cmd[0] = type; 				
		Wire.beginTransmission(slaveId);
		Wire.write(cmd,1); 				// first send the command type indicator as uint8_t
		sendInt(messageValue);			// send messageValue as an integer
		Wire.endTransmission();
		delay(10); 						// wait the I2C bus to recover
	}
}



/************************Not used**********************************/
/* if (*commandType=='S')		
	{
		if (isExist(slaves, count, slaveId) )
		{	
			uint8_t cmd[1] ;
			cmd[0] = 0x00;
			Wire.beginTransmission(slaveId);
			Wire.write(cmd,1);
			sendInt(messageValue);
			Wire.endTransmission();
			delay(10);
		}
	}
	else// unsupported
	{
		if (*commandType=='R')
		{
			if (isExist(slaves, count, slaveId) )
			{	
				uint8_t cmd[1] ;
				cmd[0] = 0x1;
				Wire.beginTransmission(slaveId);
				Wire.write(cmd,1);
				sendInt(messageValue);
				Wire.endTransmission();
				delay(10);
			}
		}
	}
*/