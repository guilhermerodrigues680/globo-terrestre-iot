#ifndef globeCoordinates_h
#define globeCoordinates_h

#include <Arduino.h>

namespace globeCoordinates {

void setCoordinates(float lat, float lon, int height);
float getLat();
float getLon();
int getHeight();

}

#endif
