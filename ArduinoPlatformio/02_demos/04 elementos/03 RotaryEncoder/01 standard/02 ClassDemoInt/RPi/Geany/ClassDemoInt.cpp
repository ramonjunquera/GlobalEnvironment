/*
 * Author: Ramón Junquera
 * Subject: Rotary encoder
 * Objecive: Reading one rotary encoder standard by interruptions
 * Material: breadboard, RPi, rotary encoder standard
 * Description:
 * We use only one rotary encoder.
 * We read it by interruptions. So, it's not necessary calling update
 * function in main loop.
 * Steps counter will sent to screen.
 * arduino library for RPi can set interruptions on any pin, so we'll
 * manage button with interruptions also.
 * If button is pressed, steps counter is reset.
 * 
 * Result:
 * We can check steps counter on screen
 */

#include <Arduino.h>
#include "RoJoRotary.cpp" //Library for rotary encoder

using namespace std;

//Global constants definition
const byte encoderPinA = 21;
const byte encoderPinB = 20;
const byte encoderPinSwitch = 16;

//Global variables definition
RoJoRotary encoder = RoJoRotary(encoderPinA,encoderPinB);
int previousStepsCounter = 0;

void update(byte pin,byte status)
{
  //Static function for calling to real update funcion of object encoder
  //It is necessary because attactInterrupt can't call directly to encoder.update
  encoder.update();
  //If steps counter has been changed...
  if (previousStepsCounter != encoder.stepsCounter)
  {
    //Show steps counter on screen
    cout << encoder.stepsCounter << endl;
    //Current counter will be last one
    previousStepsCounter = encoder.stepsCounter;
  }
}

void buttonPressed(byte pin,byte status)
{
  //Function called with button is pressed

  //Reset steps counter
  encoder.stepsCounter = 0;
  //Show in monitor
  update(0,0);
}

int main(int argc, char **argv)
{
	//Switch pin will be for reading & we'll use internal pullup resistors
	pinMode(encoderPinSwitch, INPUT_PULLUP);
	
	//We fix de limits of steps counter
	encoder.maxStepsCounter=10;
	encoder.minStepsCounter=-10;
	
	//We create interruptions for both pins, calling to update function
	attachInterrupt(encoderPinA, update, CHANGE);
	attachInterrupt(encoderPinB, update, CHANGE);
	//Set function when button is pressed
	attachInterrupt(encoderPinSwitch, buttonPressed, FALLING);
		
	cout << "Ready!" << endl;
    
    while(1)
    {
		//Nothing special to do here
	}
}

