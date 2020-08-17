#ifndef GlobeStepperMotor_h
#define GlobeStepperMotor_h

#include <Arduino.h>
#include <AccelStepper.h>

namespace globeStepperMotor {

void startStepper();
void loop();
long currentPosition();
bool isRunning();
long distanceToGo();
float speed();
void setCurrentPosition(long position);
void move(long relative);
void runToPosition();
void goToOrigin();
int16_t getStepperPosition();
int16_t getStepsToPosition(int16_t currentStepperPosition);

}

#endif
