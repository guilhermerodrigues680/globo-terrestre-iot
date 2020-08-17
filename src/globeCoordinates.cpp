#include <Arduino.h>
#include "globeCoordinates.h"
#include "remoteLogging.h"

namespace globeCoordinates {

float _lat = 0.0;
float _lon = 0.0;
int _height = 0;

void setCoordinates(float lat, float lon, int height) {
    _lat = lat;
    _lon = lon;
    _height = height;
    remoteLogging::sendPacket("setCoordinates" + (String)_lat + " " + (String)_lon + " " + (String)_height);
}

float getLat() {
    return _lat;
}

float getLon() {
    remoteLogging::sendPacket("getLon()" + (String)_lon);
    return _lon;
}

int getHeight() {
    return _height;
}

}
