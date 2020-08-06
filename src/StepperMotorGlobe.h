/*
  StepperMotorGlobe.h - Helper class to control the terrestrial globe.
  Created by Guilherme Rodrigues, guilhermerodrigues680, August, 2020.
*/
#ifndef StepperMotorGlobe_h
#define StepperMotorGlobe_h

#include <Arduino.h>

class Globe
{
  public:
    Globe();
    float getLat();
    float getLon();
    int32_t getHeight();
    void setCoordinates(float lat, float lon, int32_t height);
    static int16_t convertLongitudeIntoSteps(int16_t longitude);
    int16_t getStepperPosition();
    int16_t getStepsToPosition(int16_t currentStepperPosition);
  private:
    float lat = 0.0;
    float lon = 0.0;
    int32_t height = 0;
    int16_t stepsPerRevolution = 4096;
};

#endif
