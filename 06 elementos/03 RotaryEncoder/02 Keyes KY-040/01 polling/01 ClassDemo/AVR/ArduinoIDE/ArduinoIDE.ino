/*
  Author: Ramón Junquera
  Subject: Rotary encoder
  Objecive: Reading one rotary encoder KY-040 by polling
  Material: breadboard, Arduino board, rotary encoder Keyes KY-040
  Description:
    We use only one rotary encoder.
    We read it by pooling. So, we'll call to update function in main loop.
    Steps counter will be sent through serial port.
    Encoder button is also checked in loop().
    If button is pressed, steps counter is reset.

  Result:
    We can check steps counter in serial monitor.
*/

#include <Arduino.h>
#include "RoJoRotary040.h" //Library for rotary encoder

//Global constants definition
const byte encoderPinA = 2;
const byte encoderPinB = 3;
const byte encoderPinSwitch = 4;

//Global variables definition
RoJoRotary040 encoder = RoJoRotary040(encoderPinA,encoderPinB);
int previousStepsCounter = 0;

void setup()
{
  //Open serial port
  Serial.begin(115200);
  //Switch pin will be for reading & we'll use internal pullup resistors
  pinMode(encoderPinSwitch, INPUT_PULLUP);
  Serial.println(F("Ready!"));
}

void loop()
{
  //Update encoder steps counter
  encoder.update();
  
  //If switch has been pressed...we reset steps counter
  if (!digitalRead(encoderPinSwitch)) encoder.stepsCounter = 0;

  //If steps counter has been changed...
  if (previousStepsCounter != encoder.stepsCounter)
  {
    //Send steps counter through serial port
    Serial.println(encoder.stepsCounter);
    //Current counter will be last one
    previousStepsCounter = encoder.stepsCounter;
  }
}
