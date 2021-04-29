/*
  Author: Ramón Junquera
  Subject: Rotary encoder
  Objecive: Reading one rotary encoder by polling
  Material: breadboard, WeMos D1 Mini, rotary encoder
  Description:
    We use only one rotary encoder.
    We read it by pooling. So, we'll call to update function in main loop.
    Steps counter will be sent through serial port.
    Encoder button is also checked in loop().
    If button is pressed, steps counter is reset.

  Result:
    We can check steps counter in serial monitor.
*/

#include <arduino.h>
#include "RoJoRotary.h" //Library for rotary encoder manage

//Global constants definition
const byte encoderPinA = D5;
const byte encoderPinB = D6;
const byte encoderPinSwitch = D1;

//Global variables definition
RoJoRotary encoder = RoJoRotary(encoderPinA,encoderPinB);
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
