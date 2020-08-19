#include <Arduino.h>
#include <ArduinoOTA.h>
#include "globeOTA.h"
#include "ledActivityIndicator.h"
#include "globeGlobalSettings.h"
#include "globeStepperMotor.h"
#include "remoteLogging.h"

namespace globeOTA {

void start()
{
    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    // ArduinoOTA.setHostname("myesp8266");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
        String type;

        globeStepperMotor::goToOrigin();  // Voltar globo para origem

        if (ArduinoOTA.getCommand() == U_FLASH)
        {
            type = "sketch";
        }
        else
        { // U_FS
            type = "filesystem";
        }

        remoteLogging::sendPacket("ArduinoOTA Start, tipo: " + type);

        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        USE_SERIAL.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        remoteLogging::sendPacket("ArduinoOTA End");
        USE_SERIAL.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        static unsigned int lastPercent = 100;
        unsigned int currentPercent = (progress / (total / 100));

        if (currentPercent != lastPercent && currentPercent % 10 == 0)
        {
            lastPercent = currentPercent;
            // remoteLogging::sendPacket("Progress: " + String((progress / (total / 100))));
            remoteLogging::sendPacket("Progress: " + String(progress) + " bytes de " + String(total) + " bytes |" + String(currentPercent) + "%");
        }

        USE_SERIAL.printf("Progress: %u%%\r", (progress / (total / 100)));

        ledActivityIndicator::loop(50);
    });
    ArduinoOTA.onError([](ota_error_t error) {
        USE_SERIAL.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
        {
            USE_SERIAL.println("Auth Failed");
        }
        else if (error == OTA_BEGIN_ERROR)
        {
            USE_SERIAL.println("Begin Failed");
        }
        else if (error == OTA_CONNECT_ERROR)
        {
            USE_SERIAL.println("Connect Failed");
        }
        else if (error == OTA_RECEIVE_ERROR)
        {
            USE_SERIAL.println("Receive Failed");
        }
        else if (error == OTA_END_ERROR)
        {
            USE_SERIAL.println("End Failed");
        }
    });
    ArduinoOTA.begin();
}


void loop()
{
    ArduinoOTA.handle();
}

}
