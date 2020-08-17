#include <Arduino.h>
#include "ledActivityIndicator.h"

namespace ledActivityIndicator {

void start()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_BUILTIN_AUX, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(LED_BUILTIN_AUX, LOW);
}

void loop(uint16_t interval)
{
    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis;
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        digitalWrite(LED_BUILTIN_AUX, !digitalRead(LED_BUILTIN));
    }
}

}
