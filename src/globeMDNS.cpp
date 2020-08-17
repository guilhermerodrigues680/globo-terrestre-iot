#include <Arduino.h>
#include <ESP8266mDNS.h>
#include "globeMDNS.h"
#include "globeGlobalSettings.h"

namespace globeMDNS {

// hostname para o mDNS
const char* MDNS_ESP_HOSTNAME = "globo";

void start() {
    if(MDNS.begin(MDNS_ESP_HOSTNAME)) {
        // Add service to MDNS
        MDNS.addService("http", "tcp", 80);
        MDNS.addService("ws", "tcp", 81);
        USE_SERIAL.println("MDNS responder started");
        USE_SERIAL.println("mDNS responder started: http://esp8266.local");
    } else {
        USE_SERIAL.println("MDNS responder not started!!!");
    }
}

void loop() {
    MDNS.update();
}

const char* getMDNS_ESP_HOSTNAME() {
    return MDNS_ESP_HOSTNAME;
}

bool isRunning() {
    return MDNS.isRunning();
}

}
