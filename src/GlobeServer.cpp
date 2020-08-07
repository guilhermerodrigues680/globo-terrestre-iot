#include <ESP8266WebServer.h>
#include "GlobeServer.h"

GlobeServer::GlobeServer(const char* uri)
    : _uri(uri), _remLog()
{
    _remLog.sendPacket("Iniciado");
}

bool GlobeServer::_canHandle(HTTPMethod method, String uri) {
    _remLog.sendPacket("canHandle");
    _remLog.sendPacket(String(method));
    _remLog.sendPacket(uri);
    if (method != HTTP_GET || uri != _uri) {
        return false;
    }

    return true;
}

bool GlobeServer::_handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
    _remLog.sendPacket("handle 0");

    if (requestMethod != HTTP_GET || requestUri != _uri) {
        return false;
    }

    _remLog.sendPacket("handle 1");
    // if (!canHandle(requestMethod, requestUri)) {
    //     return false;
    // }
    next();

    server.send(200, "text/plain", "This is an about page");

    return true;
}
