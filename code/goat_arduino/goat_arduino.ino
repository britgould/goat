/*
 GOAT STEM Project
 by Raquel G and Abi S, assisted by Brit G.

 ADD URL to GitHub
*/

#include <Servo.h>

#define SERVO_WAIST_PIN 22
#define SERVO_SHOULDER_PIN 23
#define SERVO_ELBOW_PIN 24
#define SERVO_WRIST_PIN 25
#define SERVO_TWIST_PIN 26
#define SERVO_GRAB_PIN 27

#define SERVO_MOVE_DELAY 25
#define SERVO_NOT_INITIALIZED -1

// create servo objects to control a servo
Servo servoWaist, servoShoulder, servoElbow, servoWrist, servoTwist, servoGrab;

struct ServoPositions {
  int waist = SERVO_NOT_INITIALIZED;
  int shoulder = SERVO_NOT_INITIALIZED;
  int elbow = SERVO_NOT_INITIALIZED;
  int wrist = SERVO_NOT_INITIALIZED;
  int twist = SERVO_NOT_INITIALIZED;
  int grab = SERVO_NOT_INITIALIZED;
};

struct ServoPositions servoPosition;

// set up communications variables for incoming serial data
String serialData;

typedef enum {INITIALIZE, SHUTDOWN, ARM, ARMPW, MOVE} COMMAND;
typedef enum {NONE, WAIST, SHOULDER, ELBOW, WRIST, TWIST, GRAB, FORWARD, BACKWARD, LEFT, RIGHT, ROTATERIGHT, ROTATELEFT} PARAMETER;

struct Message {
  COMMAND command;
  PARAMETER parameter;
  int value;
};

struct Message serialMessage;

// parse the incoming message
void ParseMessage(String message) {
  int space1, space2;
  String strCommand, strParameter;

  space1 = message.indexOf(" ");
  space2 = message.lastIndexOf(" ");
  strCommand = message.substring(0, space1);
  strParameter = message.substring(space1 + 1, space2);

  if (strCommand == "initialize") {
    serialMessage.command = INITIALIZE;
  } else if (strCommand == "shutdown") {
    serialMessage.command = SHUTDOWN;
  } else if (strCommand == "arm") {
    serialMessage.command = ARM;
  } else if (strCommand == "armPW") {
    serialMessage.command = ARMPW;
  } else if (strCommand == "move") {
    serialMessage.command = MOVE;
  } else {
    Serial.println("ERROR!");
  }

  if (strParameter == "none") {
    serialMessage.parameter = NONE;
  } else if (strParameter == "waist") {
    serialMessage.parameter = WAIST;
  } else if (strParameter == "shoulder") {
    serialMessage.parameter = SHOULDER;
  } else if (strParameter == "elbow") {
    serialMessage.parameter = ELBOW;
  } else if (strParameter == "wrist") {
    serialMessage.parameter = WRIST; 
  } else if (strParameter == "twist") {
    serialMessage.parameter = TWIST;
  } else if (strParameter == "grab") {
    serialMessage.parameter = GRAB;
  } else if (strParameter == "forward") {
    serialMessage.parameter = FORWARD;
  } else if (strParameter == "backward") {
    serialMessage.parameter = BACKWARD;
  } else if (strParameter == "left") {
    serialMessage.parameter = LEFT;
  } else if (strParameter == "right") {
    serialMessage.parameter = RIGHT;
  } else if (strParameter == "rotateRight") {
    serialMessage.parameter = ROTATERIGHT;
  } else if (strParameter == "rotateLeft") {
    serialMessage.parameter = ROTATELEFT;
  } else {
    Serial.println("ERROR!");
  }

  serialMessage.value = message.substring(space2 + 1).toInt();
}

int ServoMoveTo(Servo servo, int currentPos, int newPos) {
  if (currentPos == SERVO_NOT_INITIALIZED) {
    currentPos = newPos;      // we don't know the current position, so start at newPos
    servo.write(currentPos);  // move servo
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
  // open up the serial port
  Serial.begin(9600);
    
  // attach servos
  servoWaist.attach(SERVO_WAIST_PIN, 500, 2400);
  servoWaist.writeMicroseconds(0);  // set the servo initially to disabled
  servoShoulder.attach(SERVO_SHOULDER_PIN, 500, 2400);
  servoShoulder.writeMicroseconds(0);
  servoElbow.attach(SERVO_ELBOW_PIN, 500, 2400);
  servoElbow.writeMicroseconds(0);
  servoWrist.attach(SERVO_WRIST_PIN, 500, 2400);
  servoWrist.writeMicroseconds(0);
  servoTwist.attach(SERVO_TWIST_PIN, 600, 2600);
  servoTwist.writeMicroseconds(0);
  servoGrab.attach(SERVO_GRAB_PIN, 600, 2600);
  servoGrab.writeMicroseconds(0);
}

void loop() {
  if (Serial.available() > 0) {
    serialData = Serial.readStringUntil('\n'); // read a line from the serial port
    ParseMessage(serialData);

    Serial.println("command: " + String(serialMessage.command));
    Serial.println("parameter: " + String(serialMessage.parameter));
    Serial.println("value: " + String(serialMessage.value));
/*
    if (serialData.startsWith("us")) {
      servo1.writeMicroseconds(serialData.substring(2).toInt());
      Serial.println("Servo microseconds set");
    } else {
      servoPosRequested = serialData.toInt(); // convert the position input to an int
      servo1Pos = ServoMoveTo(servo1, servo1Pos, servoPosRequested);
      Serial.println("Servo set to " + String(servo1Pos) + " degrees"); // report out through the serial line  
    }
*/
  }
}
