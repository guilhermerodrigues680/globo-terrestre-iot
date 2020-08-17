#include <Arduino.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "globeWebSocket.h"
#include "remoteLogging.h"
#include "globeStepperMotor.h"
#include "globeCoordinates.h"

#define USE_SERIAL Serial

namespace globeWebSocket {

WebSocketsServer webSocket(81);

void startWebSocket()
{
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        USE_SERIAL.printf("[%u] Disconnected!\n", num);
        break;
    case WStype_CONNECTED:
    {
        IPAddress ip = webSocket.remoteIP(num);
        USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "Connected");
    }
    break;
    case WStype_TEXT:
        USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
        remoteLogging::sendBufferPacket(payload, length);
        readWSMessage(payload);

        // send message to client
        // webSocket.sendTXT(num, "message here");

        // send data to all connected clients
        // webSocket.broadcastTXT("message here");
        break;
    case WStype_BIN:
        USE_SERIAL.printf("[%u] get binary length: %u\n", num, length);
        hexdump(payload, length);

        // send message to client
        // webSocket.sendBIN(num, payload, length);
        break;
    }
}


/**
    Le as coordenadas no formato JSON recebidas pelo websocket.

    @param message uma string JSON.
    Ex bom: {"lat":-16.962,"lon":-28.714,"height":8901623}
    Ex ruim: {"lat":-16.96242661161516,"lon":-28.714400061048288,"height":8901623.110352391}
*/
void readWSMessage(uint8_t *message)
{
    const size_t capacity = JSON_OBJECT_SIZE(3) + 20;

    DynamicJsonDocument coordJSON(capacity);
    deserializeJson(coordJSON, message);
    float lat = coordJSON["lat"];
    float lon = coordJSON["lon"];
    long height = coordJSON["height"];

    USE_SERIAL.printf("[*] coordJSON: %f %f %ld\n", lat, lon, height);
    globeCoordinates::setCoordinates(lat, lon, height);
    globeStepperMotor::move(globeStepperMotor::getStepsToPosition(globeStepperMotor::currentPosition()));
}

void loop()
{
    webSocket.loop();
}

}
