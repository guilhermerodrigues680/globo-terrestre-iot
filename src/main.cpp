/**
    Globo Terreste com ESP8266
    main.cpp
    Projeto: Globo Terreste com ESP8266

    @author Guilherme Rodrigues
    @version 1.0 01/08/2020
*/

#include <Arduino.h>
#include "globeOTA.h"
#include "globeWiFi.h"
#include "globeServer.h"
#include "globeMDNS.h"
#include "globeLittleFS.h"
#include "globeWebSocket.h"
#include "globeStepperMotor.h"
#include "ledActivityIndicator.h"
#include "globeGlobalSettings.h"

void setup() {
    // USE_SERIAL.begin(921600);
    USE_SERIAL.begin(9600);
    //Serial.setDebugOutput(true);
    USE_SERIAL.setDebugOutput(true);
    USE_SERIAL.println("\n\n\n");

    for (uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    globeWiFi::start();
    globeMDNS::start();
    globeOTA::start();
    globeWebSocket::startWebSocket();
    globeLittleFS::start();
    globeServer::start();
    globeStepperMotor::startStepper();
    ledActivityIndicator::start();
}

void loop() {
    globeWebSocket::loop();
    globeServer::loop();
    globeOTA::loop();
    globeMDNS::loop();
    ledActivityIndicator::loop(1000);
    globeStepperMotor::loop();
}
