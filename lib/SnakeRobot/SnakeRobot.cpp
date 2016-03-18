/*
		SnakeRobot.cpp ---- library to control the snake robot built 
		in Informatik 6, TUM, including I2C communication from PC to 
		the robot, controlling the servo, etc.
		Created by Long Cheng, 17/02/2016, Garching.
		
*/

#include <SnakeRobot.h>


/************************************************************************************
*************************************************************************************
*********************************Slave part*****************************************
*************************************************************************************
*************************************************************************************/


SlaveModule::SlaveModule(int stopPoint, int centerAngle):\
_stopPoint(stopPoint), _centerAngle(centerAngle),\
DEFAULTSPEEDLEFT(stopPoint - SERVOSPEEDRANGE),\
DEFAULTSPEEDRIGHT(stopPoint + SERVOSPEEDRANGE),\
safeAngleMax(centerAngle + SAFEANGLERANGE),\
safeAngleMin(centerAngle - SAFEANGLERANGE)
{
	
	targetAngle = centerAngle;  // let the slave module turns to center angle after startup
	isDetached = false; 		// not used now
	
	
}

void SlaveModule::init()
{
	#ifdef DEBUG
	Serial.begin(9600);
	#endif
	
	myservo.attach(9);  	// attach the servo the pin 9 on the arduino nano.
	mlx_1.attach(7, 10, 8); // the angle sensor, attach to pins
	
}

void SlaveModule::updateRelativeAngle(int relativeAngle)
{
	/*given the relativeAngle to center, we calculate the absolute
	target angle*/
	int tArgetAngle = relativeAngle + _centerAngle;
	
	/*update the absolute target angle*/
	updateAngle( tArgetAngle);
}

void SlaveModule::updateAngle(int tArgetAngle)
{ 
	/*check if the target angle out of safe region, if so, do nothing*/
	if(tArgetAngle > safeAngleMax || tArgetAngle < safeAngleMin)
	{
		return;
	}  
	/*if we already reached the targetAngle, do nothing*/
	if( abs(angleRead - tArgetAngle) < ANGLERESOLUTION)
	{
		return;
	}
	
	/****************************************/
	int error = angleRead - tArgetAngle; 

	/*if the angle error is very large, turn in fast speed*/
	if ( abs(error) > PCONTTROLVALIDRANGE )
	{
		/*determine the direction of turning and turn the servo*/
		myservo.writeMicroseconds(  (  (angleRead < tArgetAngle) ? \
		DEFAULTSPEEDLEFT : DEFAULTSPEEDRIGHT ) );
	}
	else
	{
		/*if the angle error is not so large, we use the P control*/
		/*calculate the command*/
		int order = map(error, -PCONTTROLVALIDRANGE, PCONTTROLVALIDRANGE,\
		-SERVOSPEEDRANGE, SERVOSPEEDRANGE ) + _stopPoint;
		myservo.writeMicroseconds(order);
	}
	
}


void SlaveModule::run(int Angle,  uint8_t cmd)
{
	angleRead = readAngle();                                       // read the given angle from sensor
	DEBUG_PRINTLN(angleRead);
	
	/*if the safe region of angle is violated, we let 
	the joint turn to the boundary of the safe region */
	if(  angleRead > safeAngleMax + 50  )
	{
		Angle =  safeAngleMax - 100;                   		// For safety, it is better to detach the servo here
		
	}	
	else
	{	
		if (angleRead < 0)
		{
			cmd = -1; 									// not valid angle. Do nothing but stop the servo
			stopServo();
		}
		/*if the safe region of angle is violated, we let 
	the joint turn to the boundary of the safe region */
		else if (angleRead < safeAngleMin - 50)
		{

			Angle =  safeAngleMin + 100; 
		}
		
	}
	switch ( cmd )
	{
	case 0:
		updateAngle(Angle); 
		break;
	case 1:
		updateRelativeAngle(Angle);
		break;
	default:
		break;
	}
	
	
	
	
}

void SlaveModule::stopServo()
{
	myservo.writeMicroseconds(_stopPoint);
}


int SlaveModule::readAngle()
{
	delay(5);  // wait previours reading to finish to avoid conficts, which may results -1
	return mlx_1.readAngle();
}


/************************************************************************************
*************************************************************************************
*********************************Master part*****************************************
*************************************************************************************
*************************************************************************************/


Master::Master(uint8_t cepin, uint8_t cspin, bool _radioType):pipeFromPc(PIPEFROMPC),pipeToPc(PIPETOPC),radio(cepin, cspin)
{
	count = 0;
	receivedMessageBuffer = "";
	messageToSendBuffer = "";
	done = false;
	radioType = _radioType;
	
}

void Master::init()
{
	init(9600);
}

void Master::init(int baudRate)
{
	#ifdef DEBUG
	Serial.begin(baudRate);
	#endif
	/*begin I2C bus*/
	Wire.begin();
	/*start the wireless module*/
	radio.begin();
	
	if ( radioType ) // form 1
	{
		/*the wireless module can read from another
		wireless module built in the master module
		which is attached to PC by serial port*/
		radio.openReadingPipe(1, pipeFromPc);
		radio.openWritingPipe(pipeToPc);		
		scanSlaves(slaves, &count);	 	// scan the slave modules
		for (byte i = 0 ; i < count ; i++)
		{
			DEBUG_PRINT("found devices:"); 
			
			DEBUG_PRINTLN(slaves[i]);
		}
		
		
	}
	else 		// form 0
	{	
		/*the wireless module can read from another
		wireless module built in the master module
		which is attached to the snake robot*/
		radio.openReadingPipe(1, pipeToPc);
		radio.openWritingPipe(pipeFromPc);
		DEBUG_PRINTLN("transmiter");
	}
	radio.setPayloadSize(128);
	radio.startListening();
	
}

uint8_t Master::getpayload()
{
	return radio.getPayloadSize();
}

void Master::RF24send(String * message)
{
	DEBUG_PRINT("message to send:");
	DEBUG_PRINTLN(*message);
	radio.stopListening();
	int l = message->length();
	char charToSend[l+1] ; // transform the string to char array prepare to send
	strcpy(charToSend, message->c_str()); 
	if ( l < SENDBUFFSIZE )  // we send at most SENDBUFFSIZE at once
	{		
		radio.write(charToSend,strlen(charToSend));
	}
	else
	{
		int n = l / SENDBUFFSIZE + 1; // split the array into several sub-arrays
		char * p = charToSend;
		char bufsend[SENDBUFFSIZE];
		for (int i = 0; i < n; i=i+1)  // send the sub-arrays
		{			 
			strncpy(bufsend, p, SENDBUFFSIZE);
			radio.write(bufsend,SENDBUFFSIZE);
			p = p + SENDBUFFSIZE;
		}
	}
	int charSend[1]; // send the terminator
	charSend[0]  = STOPMARK; 
	radio.write(charSend,1);
	radio.powerDown(); 
	delay(20);
	radio.powerUp();
	radio.startListening();
	
}

void Master::RF24receive()
{
	while (radio.available())
	{
		radio.read(msg, 64); // read 64 chars at once
		//char theChar = msg[0];
		
		if ( (int) msg[0] != STOPMARK)
		{
			
			receivedMessageBuffer.concat(msg); // concatanate the read chars to the buffer
			
			
		}
		else // done reading
		{
			done = true;
			break;
		}
		
	}	
}

unsigned long Master::forwardToSlaves()
{
	unsigned long ret = 0;
	/*  copy the received buffer to an array and then send the 
	array*/
	char theMessage_chararray[receivedMessageBuffer.length() + 1];
	strcpy(theMessage_chararray,receivedMessageBuffer.c_str()); 
	
	ret = forwardChar(theMessage_chararray);
	// clear the buffer... for next receiving from PC
	receivedMessageBuffer = "";
	done = false;
	return ret;

	
}

unsigned long Master::forwardToSlaves(String * message)
{
	/*  copy the input string to an array and then send the 
	array*/
	char theMessage_chararray[message->length() + 1];
	strcpy(theMessage_chararray,message->c_str()); 
	return forwardChar(theMessage_chararray);
}

unsigned long Master::forwardChar(char  theMessage_chararray[])
{	
	unsigned long ret = 0;
	byte ii=0; // records how many commands have been sent
	DEBUG_PRINT("received: ");
	DEBUG_PRINTLN(theMessage_chararray); //print out the string from NRF24L01 wireless sensor
	
	/*the printer array, each element points to the start
	address of each commands in the long command*/
	char * commands[MAX_COMMAND_NUMBER]; 

	/*the format of command received from PC: 1:S:1800&2:S:2700...
	element command 1: 1:S:1800, 1-slave id, S-command type(set joint angle), 1800-command data(target angle)
	element command 2: 2:S:2700
	the element commands combined with each other by '&'
	*/
	/*split command received from PC by '&', save results in commands*/
	splitString(theMessage_chararray, "&", commands);

	byte id = 0; // the element command index
	while ( commands[id] != NULL )
	{
		DEBUG_PRINT("This is the ");DEBUG_PRINT( id+1); DEBUG_PRINTLN(" th command");
		char * tempCommand = commands[id]; // extract idth element command into tempCommand
		DEBUG_PRINT("command is ");DEBUG_PRINTLN(tempCommand);

		char * values[MAX_COMMAND_NUMBER];
		/*split element command by ':', save results in values*/
		splitString(tempCommand, ":",values);
		// how many parts the current command can be splitted to by ':'
		byte valuesNumber = 0;   
		while ( ( values[valuesNumber] != NULL ) && (valuesNumber < MAX_COMMAND_NUMBER) )
		{
			valuesNumber++; 
		}
		
		if (valuesNumber != 3) // not correct format 
		{  // for future use
		}
		else
		{
			// send current element command to slave
			//									slave id   		 command 	command value    
			ret = ret + masterForwardCommands( atoi(values[0]), values[1], atoi(values[2]), slaves, count);
			ii++;
			// accumulate the execution time and the number of element commands 
		}
		
		id++; // go to next element command
	}
	
	if (ii > 0)
	{
		// return the average time
		return ret/ii;  
	}
	else 
	{
		// ii == 0, return 0
		return ret;
	}
	
}


/*collect information from slaves. current version only
request joint angle from slaves*/
void Master::collectFromSlaves()
{
	if (radioType) // must be form 1
	{
		byte i = 0;
		uint8_t howMany;
		int angles;
		for( ; i < count ; i++ )
		{
			/*request from slave i 2 bytes data*/
			howMany = Wire.requestFrom(slaves[i], ( byte)2);
			
			//DEBUG_PRINT("bytes available : "); DEBUG_PRINTLN(Wire.available());
			/* if the data requested is available*/
			if (Wire.available() == howMany)
			{
				angles = receiveInt();
				while (Wire.available())
				{
					Wire.read();
				}
			}
			//DEBUG_PRINT("Collected angle: "); DEBUG_PRINTLN(angles);
			
			/*construct the message, insert & between results*/
			messageToSendBuffer.concat(slaves[i]);
			messageToSendBuffer.concat(":");
			messageToSendBuffer.concat(angles);
			if ( i < ( count - 1 ) )
			{
				messageToSendBuffer.concat("&");	
			}
			
			delay(10);
		}
		DEBUG_PRINTLN(messageToSendBuffer);
	}
}

String Master::getCollectedMessage()
{
	return messageToSendBuffer;
}

void Master::clearMessageToSendBuffer()
{
	messageToSendBuffer = "";
}


bool Master::isDone()
{
	return done;
}

String Master::getReceivedMessage()
{
	return receivedMessageBuffer;
}

void Master::clearReceivedMessageBuffer()
{
	receivedMessageBuffer = "";
	done = false;
}











