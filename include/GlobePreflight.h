#ifndef GlobePreflight_h
#define GlobePreflight_h

#include <ESP8266WebServer.h>

class GlobePreflight : public RequestHandler {
  public:
    GlobePreflight(const char* uri);
    bool canHandle(HTTPMethod method, String uri) override {
        return _canHandle(method, uri);
    };
    bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) override {
        return _handle(server, requestMethod, requestUri);
    };
  protected:
    String _uri;
    bool _canHandle(HTTPMethod method, String uri);
    bool _handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri);
};

#endif //GlobePreflight_h
