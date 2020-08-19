#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "globeWiFi.h"
#include "globeSecrets.h"
#include "remoteLogging.h"
#include "globeGlobalSettings.h"

namespace globeWiFi {

ESP8266WiFiMulti WiFiMulti;

void start()
{
    WiFiMulti.addAP(SECRET_SSID, SECRET_PASS);

    while (WiFiMulti.run() != WL_CONNECTED)
    {
        delay(100);
    }

    USE_SERIAL.println("Ready");
    USE_SERIAL.print("IP address: ");
    USE_SERIAL.println(WiFi.localIP());

    remoteLogging::sendPacket("Globo Terrestre IoT iniciado! IP: " + WiFi.localIP().toString());
}

}
