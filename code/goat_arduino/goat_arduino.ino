/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 https://www.arduino.cc/en/Tutorial/LibraryExamples/Sweep
*/

#include <Servo.h>

Servo servo1;  // create servo object to control a servo
// twelve servo objects can be created on most boards

#define SERVO1_PIN 22

#define SERVO_MOVE_DELAY 25
#define SERVO_NOT_INITIALIZED -1

int servo1Pos = SERVO_NOT_INITIALIZED; // variable to store the servo's current position
int servoPosRequested = SERVO_NOT_INITIALIZED; // variable to store the requested position

String serialData; // variable to store incoming serial data

int MoveServoTo(Servo servo, int currentPos, int newPos) {
  if (currentPos == SERVO_NOT_INITIALIZED) {
    currentPos = newPos;       // we don't know the current position, so start at newPos
    servo.write(currentPos); // move servo
  } else {
    while (currentPos != newPos) {
      if (currentPos < newPos) {
        currentPos++;
      } else { 
        currentPos--;
      }
      servo.write(currentPos);  // move servo
      delay(SERVO_MOVE_DELAY);  // wait so we don't move the servos too quickly
    }
  }

  return currentPos;
}

void setup() {
  Serial.begin(9600); // open up the serial port
  
  // small servo
  //servo1.attach(SERVO1_PIN, 600, 2600);  // attaches the servo on pin 22 to the servo object

  // wrist
  servo1.attach(SERVO1_PIN, 500, 2400);  // attaches the servo on pin 22 to the servo object

  // set the servo initially to disabled
  servo1.writeMicroseconds(0);
}

void loop() {
  if (Serial.available() > 0) {
    serialData = Serial.readStringUntil('\n'); // read a line from the serial port

    if (serialData.startsWith("us")) {
      servo1.writeMicroseconds(serialData.substring(2).toInt());
      Serial.println("Servo microseconds set");
    } else {
      servoPosRequested = serialData.toInt(); // convert the position input to an int
      servo1Pos = MoveServoTo(servo1, servo1Pos, servoPosRequested);
      Serial.println("Servo set to " + String(servo1Pos) + " degrees"); // report out through the serial line  
    }
  }
}
