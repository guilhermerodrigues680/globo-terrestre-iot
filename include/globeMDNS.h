#ifndef globeMDNS_h
#define globeMDNS_h

#include <Arduino.h>

namespace globeMDNS {

void start();
void loop();
const char* getMDNS_ESP_HOSTNAME();
bool isRunning();

}

#endif
