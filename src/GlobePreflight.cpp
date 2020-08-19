/**
 * Esta classe lida com requisicoes Preflight
 * emitidas automaticamente por navegadores de internet
 * https://developer.mozilla.org/pt-BR/docs/Glossario/Preflight_request
*/

#include <ESP8266WebServer.h>
#include "GlobePreflight.h"
#include "remoteLogging.h"

GlobePreflight::GlobePreflight(const char* uri)
{

}

bool GlobePreflight::_canHandle(HTTPMethod method, String uri) {
    // remoteLogging::sendPacket("GlobePreflight::_canHandle: " + String(method) + " - " + uri);

    if (method != HTTP_OPTIONS) {
        return false;
    }

    return true;
}

bool GlobePreflight::_handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {

    if (requestMethod != HTTP_OPTIONS) {
        return false;
    }

    remoteLogging::sendPacket("Requisição Preflight Recebiba");

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Max-Age", "86400");
    server.sendHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "*");
    server.send(204);

    return true;
}
