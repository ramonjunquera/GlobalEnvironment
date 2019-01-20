/*
 * Author: Ramón Junquera
 * Subject: Rotary encoder
 * Objecive: Reading one rotary encoder KY-040 by polling
 * Material: breadboard, RPi, rotary encoder Keyes KY-040
 * Description:
 * We use only one rotary encoder.
 * We read it by pooling. So, we'll call to update function in main loop.
 * Steps counter will sent to screen.
 * Encoder button is also checked in loop().
 * If button is pressed, steps counter is reset.
 * 
 * Result:
 * We can check steps counter on screen
 */

#include <Arduino.h>
#include "RoJoRotary040.cpp" //Library for rotary encoder

using namespace std;


int main(int argc, char **argv)
{
    //Global constants definition
	const byte encoderPinA = 21;
	const byte encoderPinB = 20;
	const byte encoderPinSwitch = 16;

	//Global variables definition
	RoJoRotary040 encoder = RoJoRotary040(encoderPinA,encoderPinB);
	int previousStepsCounter = 0;
	
	//Switch pin will be for reading & we'll use internal pullup resistors
	pinMode(encoderPinSwitch, INPUT_PULLUP);
	cout << "Ready!" << endl;
    
    while(1)
    {
		//Update encoder steps counter
		encoder.update();
  
		//If switch has been pressed...we reset steps counter
		if (!digitalRead(encoderPinSwitch)) encoder.stepsCounter = 0;

		//If steps counter has been changed...
		if (previousStepsCounter != encoder.stepsCounter)
		{
			//Send steps counter through serial port
			cout << encoder.stepsCounter << endl;
			//Current counter will be last one
			previousStepsCounter = encoder.stepsCounter;
		}
	}
}

