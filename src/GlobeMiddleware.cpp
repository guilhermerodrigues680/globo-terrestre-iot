#include <ESP8266WebServer.h>
#include "GlobeMiddleware.h"
#include "remoteLogging.h"

GlobeMiddleware::GlobeMiddleware(const char* uri)
{

}

bool GlobeMiddleware::_canHandle(HTTPMethod method, String uri) {
    remoteLogging::sendPacket("REQ.: METODO: " + String(method) + " - URI: " + uri);
    return false;

    // if (method != HTTP_GET || uri != _uri) {
    //     return false;
    // }

    // return true;
}

bool GlobeMiddleware::_handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
    remoteLogging::sendPacket("GlobeMiddleware::_handle");

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
