#ifndef GlobeWebSocket_h
#define GlobeWebSocket_h

#include <Arduino.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

namespace globeWebSocket {

void startWebSocket();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
void readWSMessage(uint8_t *message);
void loop();

}

#endif
