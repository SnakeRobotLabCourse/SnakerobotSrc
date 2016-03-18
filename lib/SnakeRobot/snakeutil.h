/*
		snakeutil.h ---- public function library for SnakeRobot project. 
		Created by Long Cheng, 18/02/2016, Garching.
		
*/


/* !!! note: this library supposes the client is attached to I2C bus*/
#ifndef SNAKEUTIL_H
#define SNAKEUTIL_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Wire.h"
#include "SnakeRobot.h"

#define MAX_COMMAND_NUMBER 20

/*send an integer to the I2C bus*/
void sendInt(int num);

/*receive an integer via the I2C bus*/
int receiveInt();

/*check if a number is inside in the array */
bool isExist(byte p[], byte sizeofp, byte numbertocheck );

/*split a char array by given token */
void splitString (char* inputstring, const char * spliter, char * result[MAX_COMMAND_NUMBER]);

/*scan slaves that attached on the I2C bus*/
void scanSlaves(byte _slaves[], byte * _count);

/*send a command to a slave*/
unsigned long masterForwardCommands(byte slaveId, char * commandType, int messageValue, byte slaves[], byte count);

void sendingCommands(byte slaveId, uint8_t cmd, int messageValue, byte slaves[], byte count);



extern TwoWire Wire;

#endif