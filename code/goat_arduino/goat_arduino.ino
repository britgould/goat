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

// servo objects to control servos
Servo servoWaist, servoShoulder, servoElbow, servoWrist, servoTwist, servoGrab;
bool servosInitialized = false;
bool motorsInitialized = false;

struct ServoPositions {  // replace these with the starting positions
  int waist = SERVO_NOT_INITIALIZED;
  int shoulder = SERVO_NOT_INITIALIZED;
  int elbow = SERVO_NOT_INITIALIZED;
  int wrist = SERVO_NOT_INITIALIZED;
  int twist = SERVO_NOT_INITIALIZED;
  int grab = SERVO_NOT_INITIALIZED;
};

struct ServoPositions servoInitialPositions;

// communications variables for incoming serial data
String serialData;

typedef enum {ARM, ARMPW, MOVE} COMMAND;
typedef enum {INITIALIZE, SHUTDOWN, WAIST, SHOULDER, ELBOW, WRIST, TWIST, GRAB, FORWARD, BACKWARD, LEFT, RIGHT, ROTATERIGHT, ROTATELEFT} PARAMETER;

struct Message {
  COMMAND command;
  PARAMETER parameter;
  int value;
};

struct Message serialMessage;

// parse the incoming message
bool ParseMessage(String message) {
  int space1, space2;
  String strCommand, strParameter;

  // split the components of the message in the incoming string
  space1 = message.indexOf(" ");
  space2 = message.lastIndexOf(" ");
  strCommand = message.substring(0, space1);
  strParameter = message.substring(space1 + 1, space2);

  // parse the command
  if (strCommand == "arm") {
    serialMessage.command = ARM;
  } else if (strCommand == "armPW") {
    serialMessage.command = ARMPW;
  } else if (strCommand == "move") {
    serialMessage.command = MOVE;
  } else {
    Serial.println("ERROR: Invalid command:" + strCommand);
    return false;
  }

  // parse the parameter
  if (strParameter == "initialize") {
    serialMessage.parameter = INITIALIZE;
  } else if (strParameter == "shutdown") {
    serialMessage.parameter = SHUTDOWN;
  } else {
    switch (serialMessage.command) {
      case ARM:
      case ARMPW:
        if (strParameter == "waist") {
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
        } else {
          Serial.println("ERROR: Command:" + strCommand + ", invalid parameter:" + strParameter);
          return false;
        }
        break;
      case MOVE:
        if (strParameter == "forward") {
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
          Serial.println("ERROR: Command:" + strCommand + ", invalid parameter: " + strParameter);
          return false;
        }
        break;
    }
  }

  serialMessage.value = message.substring(space2 + 1).toInt();

  return true;
}

bool InitializeServos(void) {
  if (!servosInitialized) {
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

    servosInitialized = true;
  }

  return true;
}

bool ShutdownServos(void) {
  if (servosInitialized) {
    servoWaist.detach();
    servoShoulder.detach();
    servoElbow.detach();
    servoWrist.detach();
    servoTwist.detach();
    servoGrab.detach();

    servosInitialized = false;
  }

  return true;
}

int ServoMoveTo(Servo servo, int newPos) {
  if (servo.read() == SERVO_NOT_INITIALIZED) {
    servo.write(newPos);  // move servo
  } else {
    while (servo.read() != newPos) {
      if (servo.read() < newPos) {
        servo.write(servo.read() + 1);
      } else { 
        servo.write(servo.read() - 1);
      }
      delay(SERVO_MOVE_DELAY);  // wait so we don't move the servos too quickly
    }
  }

  return newPos;
}

bool PerformCommand(Message message) {

  return true;
}

void setup() {
  // open up the serial port
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    serialData = Serial.readStringUntil('\n'); // read a line from the serial port

    if (ParseMessage(serialData)) {
      PerformCommand(serialMessage);
    }

    Serial.println("command: " + String(serialMessage.command));
    Serial.println("parameter: " + String(serialMessage.parameter));
    Serial.println("value: " + String(serialMessage.value));
/*
    if (serialData.startsWith("us")) {
      servo1.writeMicroseconds(serialData.substring(2).toInt());
      Serial.println("Servo microseconds set");
    } else {
      servoPosRequested = serialData.toInt(); // convert the position input to an int
      servo1Pos = ServoMoveTo(servo1, servoPosRequested);
      Serial.println("Servo set to " + String(servo1Pos) + " degrees"); // report out through the serial line  
    }
*/
  }
}
