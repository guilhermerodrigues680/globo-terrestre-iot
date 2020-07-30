#include <Arduino.h>

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_BUILTIN_AUX, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED_BUILTIN_AUX, LOW);
}

void loop() {
  delay(1000);
  Serial.println("Hello World!");
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  digitalWrite(LED_BUILTIN_AUX, !digitalRead(LED_BUILTIN_AUX));
}
