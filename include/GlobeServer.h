#ifndef GlobeServer_h
#define GlobeServer_h

#include <ESP8266WebServer.h>
#include "RemoteLogging.h"

class GlobeServer : public RequestHandler {
  public:
    GlobeServer(const char* uri);
    bool canHandle(HTTPMethod method, String uri) override {
        return _canHandle(method, uri);
    };
    bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) override {
        return _handle(server, requestMethod, requestUri);
    };
  protected:
    String _uri;
    RemoteLogging _remLog;
    bool _canHandle(HTTPMethod method, String uri);
    bool _handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri);
};

#endif //GlobeServer_h
