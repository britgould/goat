/*
  Copyright 2022-2023 Raquel Gould and Abi Sullivan. All Rights Reserved.
*/

#include <Servo.h>
#include <AccelStepper.h>

#define SERVO_WAIST_PIN 22
#define SERVO_SHOULDER_PIN 24
#define SERVO_ELBOW_PIN 26
#define SERVO_WRIST_PIN 28
#define SERVO_TWIST_PIN 30
#define SERVO_GRAB_PIN 32

#define SERVO_MOVE_DELAY 25
#define SERVO_NOT_INITIALIZED -1

// servo objects to control servos
Servo servoWaist, servoShoulder, servoElbow, servoWrist, servoTwist, servoGrab;
bool servosInitialized = false;
bool motorsInitialized = false;

// initial servo positions
struct ServoDegrees {
  int waist;
  int shoulder;
  int elbow;
  int wrist;
  int twist;
  int grab;
};

struct ServoDegrees servoInitialPositions;
struct ServoDegrees servoOffsets;

#define GRAB_OPEN_POS 140
#define GRAB_TP_ROLL_POS 87
#define GRAB_CLOSE_POS 70

// communications variables for incoming serial data
String serialData;

typedef enum {NO_COMMAND, ARM, ARMPW, MOVE} COMMAND;
typedef enum {NO_PARAMETER, INITIALIZE, SHUTDOWN, REST, UP, WAIST, SHOULDER, ELBOW, WRIST, TWIST, GRAB, GRAB_OPEN, GRAB_TP_ROLL, GRAB_CLOSE, FORWARD, BACKWARD, LEFT, RIGHT, ROTATERIGHT, ROTATELEFT} PARAMETER;

struct Message {
  COMMAND command = NO_COMMAND;
  PARAMETER parameter = NO_PARAMETER;
  int value = 0;
};

struct Message serialMessage;

// parse the incoming message
bool ParseMessage(String message) {
  int space1, space2;
  String commandString, parameterString;
  
  // the message must include command, parameter AND value
  // split the components of the message in the incoming string
  space1 = message.indexOf(" ");
  space2 = message.lastIndexOf(" ");
  commandString = message.substring(0, space1);
  parameterString = message.substring(space1 + 1, space2);

  // parse the command
  if (commandString == "arm") {
    serialMessage.command = ARM;
  } else if (commandString == "armPW") {
    serialMessage.command = ARMPW;
  } else if (commandString == "move") {
    serialMessage.command = MOVE;
  } else {
    Serial.println("ERROR: Invalid command:" + commandString);
    return false;
  }

  // parse the parameter
  if (parameterString == "initialize") {
    serialMessage.parameter = INITIALIZE;
  } else if (parameterString == "shutdown") {
    serialMessage.parameter = SHUTDOWN;
  } else {
    switch (serialMessage.command) {
      case ARM:
      case ARMPW:
        if (parameterString == "rest") {
          serialMessage.parameter = REST;
        } else if (parameterString == "up") {
          serialMessage.parameter = UP;
        } else if (parameterString == "waist") {
          serialMessage.parameter = WAIST;
        } else if (parameterString == "shoulder") {
          serialMessage.parameter = SHOULDER;
        } else if (parameterString == "elbow") {
          serialMessage.parameter = ELBOW;
        } else if (parameterString == "wrist") {
          serialMessage.parameter = WRIST;
        } else if (parameterString == "twist") {
          serialMessage.parameter = TWIST;
        } else if (parameterString == "grab") {
          serialMessage.parameter = GRAB;
        } else if (parameterString == "grab_open") {
          serialMessage.parameter = GRAB_OPEN;
        } else if (parameterString == "grab_tp_roll") {
          serialMessage.parameter = GRAB_TP_ROLL;
        } else if (parameterString == "grab_close") {
          serialMessage.parameter = GRAB_CLOSE;
        } else {
          Serial.println("ERROR: Command:" + commandString + ", invalid parameter:" + parameterString);
          return false;
        }
        break;
      case MOVE:
        if (parameterString == "forward") {
          serialMessage.parameter = FORWARD;
        } else if (parameterString == "backward") {
          serialMessage.parameter = BACKWARD;
        } else if (parameterString == "left") {
          serialMessage.parameter = LEFT;
        } else if (parameterString == "right") {
          serialMessage.parameter = RIGHT;
        } else if (parameterString == "rotateRight") {
          serialMessage.parameter = ROTATERIGHT;
        } else if (parameterString == "rotateLeft") {
          serialMessage.parameter = ROTATELEFT;
        } else {
          Serial.println("ERROR: Command:" + commandString + ", invalid parameter: " + parameterString);
          return false;
        }
        break;
    }
  }

  serialMessage.value = message.substring(space2 + 1).toInt();

  return true;
}

int ServoMoveTo(Servo servo, int newPos) {
  if (newPos < 0) {
    newPos = 0;
  } else if (newPos > 180) {
    newPos = 180;
  }
  
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

bool InitializeServos(void) {
  if (!servosInitialized) {
    servoOffsets.waist = 10;
    servoOffsets.shoulder = 14;
    servoOffsets.elbow = -4;
    servoOffsets.wrist = 2;
    servoOffsets.twist = -20;
    servoOffsets.grab = 0;

    servoInitialPositions.waist = 90 + servoOffsets.waist;
    servoInitialPositions.shoulder = 60 + servoOffsets.shoulder;
    servoInitialPositions.elbow = 0 + servoOffsets.elbow;
    servoInitialPositions.wrist = 0 + servoOffsets.wrist;
    servoInitialPositions.twist = 90 + servoOffsets.twist;
    servoInitialPositions.grab = GRAB_OPEN_POS;
  
    // attach servos
    servoWaist.attach(SERVO_WAIST_PIN, 500, 2400);
    servoWaist.write(servoInitialPositions.waist);
    servoShoulder.attach(SERVO_SHOULDER_PIN, 500, 2400);
    servoShoulder.write(servoInitialPositions.shoulder);
    servoElbow.attach(SERVO_ELBOW_PIN, 500, 2400);
    servoElbow.write(servoInitialPositions.elbow);
    servoWrist.attach(SERVO_WRIST_PIN, 500, 2400);
    servoWrist.write(servoInitialPositions.wrist);
    servoTwist.attach(SERVO_TWIST_PIN, 600, 2600);
    servoTwist.write(servoInitialPositions.twist);
    servoGrab.attach(SERVO_GRAB_PIN, 600, 2600);
    servoGrab.write(servoInitialPositions.grab);

    Serial.println("DEBUG: Servos initialized");
    servosInitialized = true;
  }

  return true;
}

void ServosToRestPositions(void) {
  ServoMoveTo(servoWaist, servoInitialPositions.waist);
  ServoMoveTo(servoShoulder, servoInitialPositions.shoulder);
  ServoMoveTo(servoElbow, servoInitialPositions.elbow);
  ServoMoveTo(servoWrist, servoInitialPositions.wrist);
  ServoMoveTo(servoTwist, servoInitialPositions.twist);
  ServoMoveTo(servoGrab, GRAB_OPEN_POS);
  return;  
}

void ServosToUpPositions(void) {
  ServoMoveTo(servoShoulder, 90 + servoOffsets.shoulder);
  ServoMoveTo(servoElbow, 90 + servoOffsets.elbow);
  ServoMoveTo(servoWrist, 90 + servoOffsets.wrist);
  ServoMoveTo(servoTwist, 90 + servoOffsets.twist);
  ServoMoveTo(servoWaist, 90 + servoOffsets.waist);
  //ServoMoveTo(servoGrab, GRAB_OPEN_POS);
  return;  
}

bool ShutdownServos(void) {
  if (servosInitialized) {
    ServosToRestPositions();
    delay(3000);
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

// Define the stepper motors and the pins the will use
AccelStepper LeftBackWheel(1, 2, 5);   // (Type:driver, STEP, DIR) - Stepper1
AccelStepper LeftFrontWheel(1, 3, 6);  // Stepper2
AccelStepper RightBackWheel(1, 4, 7);  // Stepper3
AccelStepper RightFrontWheel(1, 12, 13); // Stepper4

int wheelSpeed = 1500;

void moveForward(int value) {
  int i; 
  
  LeftFrontWheel.setSpeed(wheelSpeed);
  LeftBackWheel.setSpeed(wheelSpeed);
  RightFrontWheel.setSpeed(wheelSpeed);
  RightBackWheel.setSpeed(wheelSpeed);

  // Execute the steps
  for (i = 0; i < value; i = i + 1) {
    LeftFrontWheel.runSpeed();
    LeftBackWheel.runSpeed();
    RightFrontWheel.runSpeed();
    RightBackWheel.runSpeed();
  }
}

bool PerformCommand(Message message) {
  Serial.println("in PerformCommand()");

  if (message.command == ARM) {
    switch (message.parameter) {
      case INITIALIZE:
        InitializeServos();
        break;
      case SHUTDOWN:
        ShutdownServos();
        break;
      case REST:
        ServosToRestPositions();
        break;
      case UP:
        ServosToUpPositions();
        break;
      case WAIST:
        ServoMoveTo(servoWaist, message.value + servoOffsets.waist);
        break;
      case SHOULDER:
        ServoMoveTo(servoShoulder, message.value + servoOffsets.shoulder);
        break;
      case ELBOW:
        ServoMoveTo(servoElbow, message.value + servoOffsets.elbow);
        break;
      case WRIST:
        ServoMoveTo(servoWrist, message.value + servoOffsets.wrist);
        break;
      case TWIST:
        ServoMoveTo(servoTwist, message.value + servoOffsets.twist);
        break;
      case GRAB:
        ServoMoveTo(servoGrab, message.value + servoOffsets.grab);
        break;
      case GRAB_OPEN: 
        ServoMoveTo(servoGrab, GRAB_OPEN_POS);
        break;
      case GRAB_TP_ROLL:
        ServoMoveTo(servoGrab, GRAB_TP_ROLL_POS);
        break;
      case GRAB_CLOSE:
        ServoMoveTo(servoGrab, GRAB_TP_ROLL_POS);
        break;
    }
  } else if (message.command == ARMPW) {
    switch (message.parameter) {
      case WAIST:
        servoWaist.writeMicroseconds(message.value);
        break;
      case SHOULDER:
        servoShoulder.writeMicroseconds(message.value);
        break;
      case ELBOW:
        servoElbow.writeMicroseconds(message.value);
        break;
      case WRIST:
        servoWrist.writeMicroseconds(message.value);
        break;
      case TWIST:
        servoTwist.writeMicroseconds(message.value);
        break;
      case GRAB:
        servoGrab.writeMicroseconds(message.value);
        break;
    }
  } else if (message.command == MOVE) {
    switch (message.parameter) {
      case FORWARD:
        moveForward(message.value);
        break;
    }
  }

  return true;
}

void setup() {
  // open up the serial port
  Serial.begin(9600);
  Serial.println("");
  Serial.println("Setup complete");
/*
  // Set initial seed values for the steppers
  LeftFrontWheel.setMaxSpeed(3000);
  LeftBackWheel.setMaxSpeed(3000);
  RightFrontWheel.setMaxSpeed(3000);
  RightBackWheel.setMaxSpeed(3000);
*/
}

void loop() {
  if (Serial.available() > 0) {
    serialData = Serial.readStringUntil('\n'); // read a line from the serial port

    bool messageParsedSuccessfully;
    messageParsedSuccessfully = ParseMessage(serialData);

    Serial.println("Message parsed successfully: " + String(messageParsedSuccessfully));
    Serial.println("command: " + String(serialMessage.command));
    Serial.println("parameter: " + String(serialMessage.parameter));
    Serial.println("value: " + String(serialMessage.value));
    
    if (messageParsedSuccessfully) {
      PerformCommand(serialMessage);
      Serial.println("OK");
    }
  }
}
