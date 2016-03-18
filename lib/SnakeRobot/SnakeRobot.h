/*
		SnakeRobot.h ---- library to control the snake robot built 
		in Informatik 6, TUM, including I2C communication from PC to 
		the robot, controlling the servo, etc.
		Created by Long Cheng, 17/02/2016, Garching.
		
*/

#ifndef SNAKEROBOT_H
#define SNAKEROBOT_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "MLX90316.h"
#include  "Servo.h"
#include "Wire.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"
#include "SPI.h"

#include "snakeutil.h"
//#define TIMEAWARE            // toggling the code to time aware mode
//#define DEBUG                // toggling the code to print something for debugging, default: off

/****Slave macros definition********/

/* Resolution of updating the slave joint angle. The joint
of every slave module can rotate 180 degrees, generally from
around 90 degree to around 270 degree. The unit of the angle 
feedback from the angle sensor is 0.1 degree. Therefore, the
servo will stop as long as the difference between target angle
and real angle is less than  ANGLERESOLUTION*0.1 degree.*/
#define ANGLERESOLUTION 2 

/* For safety, we do not let the joint rotate full 180 degrees.
Each module joint has a center angle, which is the angle read from
its angle sensor when the robot's shape is a straight line. The
allowed rotate range is [center_angle - SAFEANGLERANGE, 
center_angle + SAFEANGLERANGE]*/                              
#define SAFEANGLERANGE 750 

/* P control effective range. When the difference between real angle
and targe angle is less than PCONTTROLVALIDRANGE, we use P control to
control the servo. Otherwise, we let the servo runs in a constant fast 
speed*/                              
#define PCONTTROLVALIDRANGE 200	

/*The coefficient determing the constant speed, which also determines 
the coefficient of P control. Larger SERVOSPEEDRANGE usually results in
faster speed of rotating*/		
#define SERVOSPEEDRANGE 100


/****Master macros definition********/

/* The maximal number of slaves that can be recognised by the master*/
#define MAX_NUMBER_SLAVES 20

/* The maximal number of commands in a command string that the master 
can receive  from PC*/						
#define MAX_COMMAND_NUMBER 20

/* Stop mark of wireless transimission, which is sent at the end of the
message*/
#define STOPMARK 13

/* Wireless transimission pipe, which is wrote by the wireless module which is close 
to PC and read by the wireless module on the snake*/
#define PIPEFROMPC 	0xE8E8F0F0E0

/* Wireless transimission pipe, which is wrote by the wireless module on the 
snake and read by the wireless module close to PC */
#define PIPETOPC	0xE8E8F0F0E1LL

/* The size of the buffer used to store messages to be sent by wireless module*/
#define SENDBUFFSIZE 25

/* if DEBUG defined, debugging information will be displayed*/
#ifdef DEBUG
#define DEBUG_PRINT(x)         Serial.print(x)
#define DEBUG_PRINTDEC(x)      Serial.print(x, DEC)
#define DEBUG_PRINTLN(x)       Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x) 
#endif

/* The slave class, including the servo, angle sensor...*/
class SlaveModule
{
public:
	
	SlaveModule(int stopPoint, int centerAngle);  	//constructor
	
	
	void init();									// init, should be called in setup
	
	/* this function lets the module run, target: the joint
	angle reach the given target angle. The tArgetAngle has
	two patterns: absolute angle and relative angle.  when cmd
	is 0, tArgetAngle is handled as the absolute target angle;
	when cmd is 1, tArgetAngle is handled as the relative
	angle to the centerAngle of the joint. This function
	should be called inside the loop*/
	void run(int tArgetAngle, uint8_t cmd); 	    // should be called inside a loop
	
	int readAngle();								// get the module joint angle
	
	void stopServo(); 								// stop the servo turning
	
	
	
private:
	
	/*control the servo turns until the absolute target angle is
	reached, only called after the joint angle is already 
	updated. 
	*/
	void updateAngle(int targetAngle);	
	/*the relative target angle version*/
	void updateRelativeAngle(int relativeAngle);
	
	MLX90316 mlx_1; 					// the angle sensor
	
	Servo myservo; 						// the servo
	
	boolean isDetached; 				// flag indicating if the servo is detached, currently not used
	
	int targetAngle;                    // initialize the module to center position
	int angleRead;						// joint angle, unit: 0.1 degree
	
	const int _stopPoint;				// stop the servo command parameter
	const int _centerAngle; 			// the joint center angle of this module 
	
	const int DEFAULTSPEEDLEFT;         // default turning left speed 
	const int DEFAULTSPEEDRIGHT;        // default turning right speed 

	const int safeAngleMax;        		// the max angle in safe range
	const int safeAngleMin;        		// the min angle in safe range
	
	
};


/* The master class, used by the master modules. Has two
forms: form 0, talks with PC via serial port and communicates with
another master object of form 1.  Form 1, talks with slaves via
I2C and communicates with another master object of form 0. When
constructing an object, the form must be choosed among 0 and 1.*/
class Master
{
public:
	/* cepin and cspin are the two pins connected to the
	wireless module, _radioType should be either true or
	false to indicate the form of the object*/
	Master( uint8_t cepin, uint8_t cspin, bool _radioType); 	// constructor
	
	/* get the payload size in the wireless module, 
	for more details see the library 'RF24-master'*/
	uint8_t getpayload();
	
	/*begin the I2C bus with default 9600 baud rate, if DEBUG is enabled.
	begin radio transimission, scan the slaves if the form is 1*/
	void init();
	
	/*if DEBUG is enabled, begin the I2C bus with given baudRate 
	and radio transimission, scan the slaves if the form is 1*/
	void init(int baudRate);

	
	/*wireless can be achieved only when two master
	modules, one has form 0 and one has form 1, have 
	been constructed, they work in pair*/
	
	/*send the message via wireless to another master module*/
	void RF24send(String * message);
	
	/*receive the message sent by another master module and 
	save the message in the receivedMessageBuffer*/
	void RF24receive();
	
	/*indicates if the procedure of receiving message 
	from other module is done*/
	bool isDone();
	
	/*forward the message in receivedMessageBuffer to the slaves,
	return the average execution time of sending one command
	to a slave (in milliseconds)*/
	unsigned long forwardToSlaves();
	
	/*forward the input message to the slaves,
	return the average execution time of sending one command
	to a slave (in milliseconds)*/
	unsigned long forwardToSlaves(String * message);
	
	/*forward the input array of char to the slaves,
	return the average execution time of sending one command
	to a slave (in milliseconds)*/
	unsigned long forwardChar(char  theMessage_chararray[]);
	
	/*return the message collected from slaves*/
	String getCollectedMessage();
	
	/*return the message received from other master module*/
	String getReceivedMessage();
	
	/*clear the buffer*/
	void clearMessageToSendBuffer();
	
	/*clear the buffer*/
	void clearReceivedMessageBuffer();
	
	/*collect the joint angle of the slaves*/
	void collectFromSlaves();
	
	
	
private:
	
	RF24 radio; 						// the wireless module
	char msg[64]; 						// temp char array used to store received message
	const uint64_t pipeToPc; 			// wireless pipe with direction to the computer
	const uint64_t pipeFromPc;			// wireless pipe with direction to the snake robot
	bool   radioType; 					// indicate the form of the object
	bool	 done; 						// indicates if message is completely received
	String receivedMessageBuffer;		// buff to store received message
	String messageToSendBuffer;			// buff to store message that will be sent
	
	byte slaves[MAX_NUMBER_SLAVES];		// array to store slave addresses
	byte count;							// number of slaves
	
	
	
};

extern TwoWire Wire;

#endif