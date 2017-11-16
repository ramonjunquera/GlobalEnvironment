/* Servo - Sweep
 * by BARRAGAN <http://barraganstudio.com>
 * This example code is in the public domain.
 *
 * modified 28 May 2015 by Michael C. Miller
 * modified 8 Nov 2013 by Scott Fitzgerald
 *
 * http://arduino.cc/en/Tutorial/Sweep
 * https://github.com/esp8266/Arduino/blob/master/libraries/Servo/examples/Sweep/Sweep.ino
 */

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
                // twelve servo objects can be created on most boards

void setup()
{
  myservo.attach(2);  // attaches the servo on GPIO2 to the servo object
}

void loop()
{
  int pos;

  for (pos = 0; pos <= 180; pos += 1) // goes from 0 degrees to 180 degrees
  {                                   // in steps of 1 degree
    myservo.write(pos);               // tell servo to go to position in variable 'pos'
    delay(15);                        // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    myservo.write(pos);               // tell servo to go to position in variable 'pos'
    delay(15);                        // waits 15ms for the servo to reach the position
  }
}
