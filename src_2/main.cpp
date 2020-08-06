#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"

/////// Dados sensiveis no arquivo arduino_secrets.h
/////// Wifi Settings ///////
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

using namespace websockets;

WebsocketsServer server;

void setup() {
  Serial.begin(9600);
  // Connect to wifi
  WiFi.begin(ssid, password);

  // Wait some time to connect to wifi
  for(int i = 0; i < 15 && WiFi.status() != WL_CONNECTED; i++) {
      Serial.print(".");
      delay(1000);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   //You can get IP address assigned to ESP

  server.listen(8080);
  Serial.print("Is server live? ");
  Serial.println(server.available());
}

void loop() {
  // auto client = server.accept();
  WebsocketsClient client = server.accept();

  // if(client.available()) {
  while(client.available()) {
    // auto msg = client.readBlocking();
    WebsocketsMessage msg = client.readBlocking();

    // log
    Serial.print("Got Message: ");
    Serial.println(msg.data());

    const size_t capacity = JSON_OBJECT_SIZE(3) + 20;
    DynamicJsonDocument doc(capacity);
    // const char* json = "{\"lat\":-16.96242661161516,\"lon\":-28.714400061048288,\"height\":8901623.110352391}";
    // Ex: {"lat":-16.96242661161516,"lon":-28.714400061048288,"height":8901623.110352391}
    deserializeJson(doc, msg.data());
    float lat = doc["lat"];
    float lon = doc["lon"];
    float height = doc["height"];

    Serial.print(lat);
    Serial.print('\t');
    Serial.print(lon);
    Serial.print('\t');
    Serial.println(height);

    const size_t capacity1 = JSON_OBJECT_SIZE(3) + 20;
    DynamicJsonDocument doc1(capacity1);

    doc1["lat"] = -16.0;
    doc1["lon"] = -28.0;
    doc1["height"] = 8901623.0;

    String output;
    serializeJson(doc1, output);

    // return echo
    // client.send("Echo: " + output);
    client.send(output);

    // close the connection
    // client.close();
  }

  delay(1000);
}
