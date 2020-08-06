/*
  StepperMotorGlobe.cpp - Helper class to control the terrestrial globe.
  Created by Guilherme Rodrigues, guilhermerodrigues680, August, 2020.
*/
#include "StepperMotorGlobe.h"

Globe::Globe() {}

void Globe::setCoordinates(float lat, float lon, int32_t height)
{
    this->lat = lat;
    this->lon = lon;
    this->height = height;
}

float Globe::getLat()
{
    return this->lat;
}

float Globe::getLon()
{
    return this->lon;
}

int32_t Globe::getHeight()
{
    return this->height;
}

int16_t Globe::convertLongitudeIntoSteps(int16_t longitude)
{
    return longitude * 10;
}

int16_t Globe::getStepperPosition()
{
    // int16_t position = map((int)this->lon, -180, 180, -2048, 2048);
    int16_t position = map((int)this->lon, -180, 180, -this->stepsPerRevolution/2, this->stepsPerRevolution/2);
    return position;
}

int16_t Globe::getStepsToPosition(int16_t currentStepperPosition)
{
    // lon - currentPosition
    int16_t targetPosition = this->getStepperPosition(); // destination position
    int16_t steps = targetPosition - currentStepperPosition;
    return steps;
}
