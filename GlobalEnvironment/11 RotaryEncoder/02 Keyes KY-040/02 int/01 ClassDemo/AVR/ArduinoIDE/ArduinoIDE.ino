/*
  Author: Ramón Junquera
  Subject: Rotary encoder
  Objecive: Reading one rotary encoder KY-040 by interruptions
  Material: breadboard, Arduino board, rotary encoder Keyes KY-040
  Description:
    We use only one rotary encoder.
    We read it by interruptions. So, it's not necessary calling update function in main loop
    Steps counter will be sent through serial port.
    Encoder button is checked in loop().
    If button is pressed, steps counter is reset.
  Result:
    We can check steps counter in serial monitor.
*/

#include <arduino.h>
#include "RoJoRotary040.h" //Library for rotary encoder

//Global constants definition
const byte encoderPinA = 2;
const byte encoderPinB = 3;
const byte encoderPinSwitch = 4;
const byte encoderPinAint = 0;
const byte encoderPinBint = 1;


//Global variables definition
RoJoRotary040 encoder = RoJoRotary040(encoderPinA,encoderPinB);
int previousStepsCounter = 0;

void update()
{
  //Static function for calling to real update funcion of object encoder
  //It is necessary because attactInterrupt can't call directly to encoder.update
  encoder.update();
  //If steps counter has been changed...
  if (previousStepsCounter != encoder.stepsCounter)
  {
    //Send steps counter through serial port
    Serial.println(encoder.stepsCounter);
    //Current counter will be last one
    previousStepsCounter = encoder.stepsCounter;
  }
}

void setup()
{
  //Open serial port
  Serial.begin(115200);
  //Switch pin will be for reading & we'll use internal pullup resistors
  pinMode(encoderPinSwitch, INPUT_PULLUP);
  //We fix de limits of steps counter
  encoder.maxStepsCounter=10;
  encoder.minStepsCounter=-10;
  //We create interruptions for both pins, calling to update function
  attachInterrupt(encoderPinAint, update, CHANGE);
  attachInterrupt(encoderPinBint, update, CHANGE);
  Serial.println(F("Ready!"));
}

void loop()
{
  //We only manage the button
  
  //If switch has been pressed...
  if (!digitalRead(encoderPinSwitch))
  {
    //...we reset steps counter
    encoder.stepsCounter = 0;
    //Show in monitor
    update();
  }
}
