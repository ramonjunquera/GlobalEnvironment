/*
  Author: Ramón Junquera
  Subject: Rotary encoder
  Objecive: Reading two rotary encoders KY-040 by polling
  Material: breadboard, ESP8266 board, 2x rotary encoder Keyes KY-040
  Description:
    We use two rotary encoders.
    We read them by polling. So, we'll call to update function in main loop.
    Steps counter will be sent through serial port.
    Encoder button is also checked in loop().
    If button is pressed, steps counter is reset.
  Result:
    We can check steps counters in serial monitor.
*/

#include <arduino.h>
#include "RoJoRotary040.h" //Library for rotary encoder

//Global constants definition
const byte encoder1PinA = D5;
const byte encoder1PinB = D6;
const byte encoder1PinSwitch = D7;
const byte encoder2PinA = D1;
const byte encoder2PinB = D2;
const byte encoder2PinSwitch = D3;

//Global variables definition
RoJoRotary040 encoder1 = RoJoRotary040(encoder1PinA,encoder1PinB);
RoJoRotary040 encoder2 = RoJoRotary040(encoder2PinA,encoder2PinB);
int previousStepsCounter1 = 0;
int previousStepsCounter2 = 0;

void setup()
{
  //Open serial port
  Serial.begin(115200);
  //Switch pin will be for reading & we'll use internal pullup resistors
  pinMode(encoder1PinSwitch, INPUT_PULLUP);
  pinMode(encoder2PinSwitch, INPUT_PULLUP);
  Serial.println(F("Ready!"));
}

void loop()
{
  //Update encoder steps counters
  encoder1.update();
  encoder2.update();

  //If switch has been pressed...we reset steps counter
  if (!digitalRead(encoder1PinSwitch)) encoder1.stepsCounter = 0;
  if (!digitalRead(encoder2PinSwitch)) encoder2.stepsCounter = 0;

  //If any steps counter has changed...
  if ((previousStepsCounter1 != encoder1.stepsCounter) || (previousStepsCounter2 != encoder2.stepsCounter))
  {
    //Send steps counters through serial port
    Serial.print(encoder1.stepsCounter);
    Serial.print(" : ");
    Serial.println(encoder2.stepsCounter);
    //Current counters will be last ones
    previousStepsCounter1 = encoder1.stepsCounter;
    previousStepsCounter2 = encoder2.stepsCounter;
  }
}
