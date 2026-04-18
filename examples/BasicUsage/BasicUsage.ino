/*
  EVolocity Chassis Controller — Basic Usage
  -------------------------------------------
  This is all the code you need to drive the car!

  The library handles everything in the background:
    • Receiving signals from the handheld controller over radio
    • Checking the battery and turning on the low-battery LED

  You just need to:
    1. Read the steering and motor values from the controller
    2. Send those values to the car
*/

#include <EVolocityChassisController.h>

EVolocityChassisController chassis;

void setup() {
  chassis.begin();
}

void loop() {
  // Step 1: read what the controller is asking for
  int angle     = chassis.getSteeringAngle();   // 0 to 180 degrees
  int speed     = chassis.getMotorSpeed();       // 0 to 255
  int direction = chassis.getMotorDirection();   // -1, 0, or 1

  // Step 2: make the car do it
  chassis.setSteering(angle);
  chassis.setMotor(speed, direction);
}
