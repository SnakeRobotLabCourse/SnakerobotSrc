/*
  MLX90316.h -  Library to use with Melexis MLX90316 rotary encoder chip.
  Created by Martin Nawrath KHM 2010.
  http://interface.khm.de
  Released into the public domain.
*/

#ifndef MLX90316_h
#define MLX90316_h

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif


class MLX90316
{
  public:
	// constructor
    MLX90316();
	// attach
	void attach(int pin1, int pin2, int pin3);
      // read sensor angle value

	int readAngle();
	
   //  private:
   
int _pinSCK;
int _pinSS;
int _pinMOSI;
char  _spiByte(char  tx);

};


#endif
