#include <ESP8266WebServer.h>
#include "GlobeMiddleware.h"
#include "remoteLogging.h"

GlobeMiddleware::GlobeMiddleware(const char* uri)
{
    remoteLogging::sendPacket("Iniciado");
}

bool GlobeMiddleware::_canHandle(HTTPMethod method, String uri) {
    remoteLogging::sendPacket("_canHandle: " + String(method) + " - " + uri);
    // return false;

    // if (method != HTTP_GET || uri != _uri) {
    //     return false;
    // }

    if (method != HTTP_OPTIONS) {
        return false;
    }

    return true;
}

bool GlobeMiddleware::_handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
    remoteLogging::sendPacket("handle 0");

    if (requestMethod == HTTP_OPTIONS) {
        remoteLogging::sendPacket("Preflight request HTTP_OPTIONS");
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Max-Age", "86400");
        server.sendHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "*");
        server.send(204);
        return true;
    }

    return false;

    // if (requestMethod != HTTP_GET || requestUri != _uri) {
    //     return false;
    // }

    // _remLog.sendPacket("handle 1");
    // // if (!canHandle(requestMethod, requestUri)) {
    // //     return false;
    // // }

    // server.send(200, "text/plain", "This is an about page");

    // return true;
}
