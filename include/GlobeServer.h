#ifndef globeServer_h
#define globeServer_h

#include <Arduino.h>

namespace globeServer {

String getContentType(String filename);
bool handleFileRead(String path);
void coordHandle();
void centerGlobeHandle();
void healthHandle();
void restartHandle();
void notFoundHandle();
void start();
void loop();

}

#endif
