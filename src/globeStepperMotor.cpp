#include <Arduino.h>
#include <AccelStepper.h>
#include "globeStepperMotor.h"
#include "globeCoordinates.h"

// Motor pin definitions:
#define IN1   D5   // IN1 on the ULN2003 driver. Is connected to NodeMCU pin D5
#define IN2   D6   // IN2 on the ULN2003 driver. Is connected to NodeMCU pin D6
#define IN3   D7   // IN3 on the ULN2003 driver. Is connected to NodeMCU pin D7
#define IN4   D8   // IN4 on the ULN2003 driver. Is connected to NodeMCU pin D8

// Define the AccelStepper interface type; 4 wire motor in half step mode:
#define MotorInterfaceType 8

namespace globeStepperMotor {

int16_t _stepsPerRevolution = 4096;

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper library with 28BYJ-48 stepper motor:
AccelStepper stepper = AccelStepper(MotorInterfaceType, IN1, IN3, IN2, IN4);

void startStepper()
{
    // Set the maximum steps per second:
    stepper.setMaxSpeed(1000);
    // Set the maximum acceleration in steps per second^2:
    stepper.setAcceleration(200);

    // stepper.setSpeed(1000);
    // stepper.moveTo(0);
}

void loop()
{

    if(stepper.currentPosition() == 2048) {
    // if(stepper.currentPosition() > 2048) {
        stepper.setCurrentPosition(-2048);
        stepper.setMaxSpeed(1000);
        // stepper.setAcceleration(200);
        // stepper.setSpeed(1000);
    // } else if(stepper.currentPosition() < -2048) {
    } else if(stepper.currentPosition() == -2048) {
        stepper.setCurrentPosition(2048);
        stepper.setMaxSpeed(1000);
        // stepper.setAcceleration(-200);
    }
    stepper.run();
    // if (stepper.distanceToGo() > 0) {
    //     // stepper.runSpeed();
    //     // stepper.setSpeed(100);
    //     // stepper.setAcceleration(200);
    //     stepper.run();
    // } else if (stepper.distanceToGo() < 0) {
    //     // stepper.runSpeed();
    //     // stepper.setSpeed(-100);
    //     // stepper.setAcceleration(-200);
    //     stepper.run();
    // } else {
    //     // Nada
    // }
}

long currentPosition() {
    return stepper.currentPosition();
}

bool isRunning() {
    return stepper.isRunning();
}

long distanceToGo() {
    return stepper.distanceToGo();
}

float speed() {
    return stepper.speed();
}

void setCurrentPosition(long position) {
    stepper.setCurrentPosition(position);
}

void move(long relative) {
    stepper.move(relative);
}

void runToPosition() {
    stepper.runToPosition();
}

//// Globe ////

void goToOrigin(){
    // Voltar globo para origem
    long globeCurrentPosition = stepper.currentPosition();
    if (globeCurrentPosition != 0) {
        globeStepperMotor::move(-globeCurrentPosition);
        globeStepperMotor::runToPosition();
    }
}

int16_t getStepperPosition() {
    int16_t position = map((int)globeCoordinates::getLon(), -180, 180, -_stepsPerRevolution/2, _stepsPerRevolution/2);
    return position;
}

int16_t getStepsToPosition(int16_t currentStepperPosition) {
    int16_t targetPosition = getStepperPosition(); // destination position
    int16_t steps = targetPosition - currentStepperPosition;
    return steps;
}

}
