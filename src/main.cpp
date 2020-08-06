/**
    Globo Terreste com ESP8266
    main.cpp
    Projeto: Globo Terreste com ESP8266

    @author Guilherme Rodrigues
    @version 1.0 01/08/2020
*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <AccelStepper.h>
#include <Hash.h>
#include <ArduinoJson.h>
#include "arduinoSecrets.h"
#include "staticHtmlPages.h"
#include "StepperMotorGlobe.h"

// Motor pin definitions:
#define IN1   D5   // IN1 on the ULN2003 driver. Is connected to NodeMCU pin D5
#define IN2   D6   // IN2 on the ULN2003 driver. Is connected to NodeMCU pin D6
#define IN3   D7   // IN3 on the ULN2003 driver. Is connected to NodeMCU pin D7
#define IN4   D8   // IN4 on the ULN2003 driver. Is connected to NodeMCU pin D8

// Define the AccelStepper interface type; 4 wire motor in half step mode:
#define MotorInterfaceType 8

#define USE_SERIAL Serial


ESP8266WiFiMulti WiFiMulti;
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper library with 28BYJ-48 stepper motor:
AccelStepper stepper = AccelStepper(8, IN1, IN3, IN2, IN4);
Globe globe = Globe();

void teste();
void readWSMessage(uint8_t *message);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
void startWiFi();
void startArduinoOTA();
void startWebSocket();
void startMDNS();
void startServer();
void startStepper();
void startLedActivityIndicator();
void loopLedActivityIndicator(uint16_t interval);

void setup()
{
    // USE_SERIAL.begin(921600);
    USE_SERIAL.begin(9600);
    //Serial.setDebugOutput(true);
    USE_SERIAL.setDebugOutput(true);
    USE_SERIAL.println("\n\n\n");

    for (uint8_t t = 4; t > 0; t--)
    {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    startWiFi();
    startArduinoOTA();
    startWebSocket();
    startMDNS();
    startServer();
    startStepper();
    startLedActivityIndicator();
}

void loop()
{
    webSocket.loop();
    server.handleClient();
    ArduinoOTA.handle();
    // MDNS.update();
    loopLedActivityIndicator(1000);

    if(stepper.currentPosition() == 2048) {
        stepper.setCurrentPosition(-2048);
        stepper.setMaxSpeed(1000);
        // stepper.setAcceleration(200);
        // stepper.setSpeed(1000);
    } else if(stepper.currentPosition() == -2048) {
        stepper.setCurrentPosition(2048);
        stepper.setMaxSpeed(1000);
        // stepper.setAcceleration(-200);
    }
    stepper.run();
    // if (stepper.distanceToGo() > 0) {
    //     // stepper.runSpeed();
    //     // stepper.setSpeed(100);
    //     // stepper.setAcceleration(200);
    //     stepper.run();
    // } else if (stepper.distanceToGo() < 0) {
    //     // stepper.runSpeed();
    //     // stepper.setSpeed(-100);
    //     // stepper.setAcceleration(-200);
    //     stepper.run();
    // } else {
    //     // Nada
    // }
}

void teste()
{
    stepper.moveTo(4096);
    // Run to position with set speed and acceleration:
    stepper.runToPosition();

    // delay(1000);

    // Move back to original position:
    stepper.moveTo(0);
    // Run to position with set speed and acceleration:
    stepper.runToPosition();

    // delay(1000);
}

/**
    Le as coordenadas no formato JSON recebidas pelo websocket.

    @param message uma string JSON.
    Ex bom: {"lat":-16.962,"lon":-28.714,"height":8901623}
    Ex ruim: {"lat":-16.96242661161516,"lon":-28.714400061048288,"height":8901623.110352391}
*/
void readWSMessage(uint8_t *message)
{
    const size_t capacity = JSON_OBJECT_SIZE(3) + 20;

    DynamicJsonDocument coordJSON(capacity);
    deserializeJson(coordJSON, message);
    float lat = coordJSON["lat"];
    float lon = coordJSON["lon"];
    long int height = coordJSON["height"];

    USE_SERIAL.printf("[*] coordJSON: %f %f %ld\n", lat, lon, height);
    // stepper.step(1024);
    globe.setCoordinates(lat, lon, height);
    // stepper.setSpeed(1000);
    // stepper.stop();
    // stepper.moveTo(globe.getStepperPosition());
    // stepper.runToPosition();
    // stepper.runSpeedToPosition();
    // stepper.runToNewPosition(globe.getStepperPosition());
    // stepper.stop();
    // stepper.moveTo(globe.getStepperPosition());
    // stepper.move(globe.getStepperPosition());
    stepper.move(globe.getStepsToPosition(stepper.currentPosition()));
    // stepper.setSpeed(1000);
    // stepper.runSpeed();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        USE_SERIAL.printf("[%u] Disconnected!\n", num);
        break;
    case WStype_CONNECTED:
    {
        IPAddress ip = webSocket.remoteIP(num);
        USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "Connected");
    }
    break;
    case WStype_TEXT:
        USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
        readWSMessage(payload);

        // send message to client
        // webSocket.sendTXT(num, "message here");

        // send data to all connected clients
        // webSocket.broadcastTXT("message here");
        break;
    case WStype_BIN:
        USE_SERIAL.printf("[%u] get binary length: %u\n", num, length);
        hexdump(payload, length);

        // send message to client
        // webSocket.sendBIN(num, payload, length);
        break;
    }
}

void startWiFi()
{
    WiFiMulti.addAP(SECRET_SSID, SECRET_PASS);

    while (WiFiMulti.run() != WL_CONNECTED)
    {
        delay(100);
    }

    USE_SERIAL.println("Ready");
    USE_SERIAL.print("IP address: ");
    USE_SERIAL.println(WiFi.localIP());
}

void startArduinoOTA()
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
        if (ArduinoOTA.getCommand() == U_FLASH)
        {
            type = "sketch";
        }
        else
        { // U_FS
            type = "filesystem";
        }

        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        USE_SERIAL.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        USE_SERIAL.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        USE_SERIAL.printf("Progress: %u%%\r", (progress / (total / 100)));
        loopLedActivityIndicator(50);
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

void startWebSocket()
{
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void startMDNS()
{
    if(MDNS.begin("esp8266")) {
        USE_SERIAL.println("MDNS responder started");
        USE_SERIAL.println("mDNS responder started: http://esp8266.local");
    } else {
        USE_SERIAL.println("MDNS responder not started!!!");
    }

    // Add service to MDNS
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);
}

void startServer()
{
    // handle `default-page.css`
    server.on("/default-page.css", []() {
        server.send(200, "text/css", defaultPageCss);
    });

    // handle `index`
    server.on("/", []() {
        // send index.html
        // server.send(200, "text/html", "<html><head><script>var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);connection.onopen = function () {  connection.send('Connect ' + new Date()); }; connection.onerror = function (error) {    console.log('WebSocket Error ', error);};connection.onmessage = function (e) {  console.log('Server: ', e.data);};function sendRGB() {  var r = parseInt(document.getElementById('r').value).toString(16);  var g = parseInt(document.getElementById('g').value).toString(16);  var b = parseInt(document.getElementById('b').value).toString(16);  if(r.length < 2) { r = '0' + r; }   if(g.length < 2) { g = '0' + g; }   if(b.length < 2) { b = '0' + b; }   var rgb = '#'+r+g+b;    console.log('RGB: ' + rgb); connection.send(rgb); }</script></head><body>LED Control:<br/><br/>R: <input id=\"r\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>G: <input id=\"g\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>B: <input id=\"b\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/></body></html>");
        server.send (200, "text/html", htmlIndexPage);
    });

    // handle `/status`
    server.on("/status", []() {
        // send status.html
        server.send(200, "text/plain", MDNS.isRunning() ? "true" : "false");
    });

    // handle `/globe`
    server.on("/globe", []() {
        // redirect to the globe website
        server.sendHeader("Location", String("https://guilhermerodrigues680.github.io/website-terrestrial-globe-with-esp8266/"), true);
        server.send (302, "text/plain", "");
    });

    // handle `/config`
    server.on("/config", []() {
        // adjust global variables and send config.html
        String message = "Number of args received:";
        message += server.args();
        for (int i = 0; i < server.args(); i++)
        {
            message += "Arg n:" + (String)i + " –> ";
            message += server.argName(i) + ": ";
            message += server.arg(i) + "\n";
        }

        if (server.arg("vel") == "")
        {
            message += "\nvel not found";
        }
        else
        {
            message += "\nvel found: ";
            message += server.arg("vel");
            stepper.setSpeed(server.arg("vel").toInt());
        }

        server.send (200, "text/plain", message);
    });

    // handle `/test`
    server.on("/test", []() {
        // do some test and send test.html
        teste();
        server.send (200, "text/plain", "Teste OK!");
    });

    // handle `/coord`
    server.on("/coord", []() {
        // send text with coordinates
        String message = "";
        message += "-- GLOBE --\n";
        message += "Lat: " + (String)globe.getLat() + "\n";
        message += "Lon: " + (String)globe.getLon() + "\n";
        message += "Height: " + (String)globe.getHeight() + "\n";
        message += "StepperPosition: " + (String)globe.getStepperPosition() + "\n";
        message += "-- STEPPER --\n";
        message += "currentPosition " + (String)stepper.currentPosition() + "\n";
        message += "isRunning " + (String)stepper.isRunning() + "\n";
        message += "distanceToGo " + (String)stepper.distanceToGo() + "\n";
        message += "speed " + (String)stepper.speed() + "\n";
        server.send (200, "text/plain", message);
    });

    // handle `center-globe`
    server.on("/center-globe", []() {
        if (server.arg("steps") != "") {
            stepper.move(server.arg("steps").toInt());
            stepper.runToPosition();
        }
        if (server.arg("center") != "") {
            stepper.setCurrentPosition(0);
        }
        server.send(200, "text/html", centerGlobePage);
    });

    // handle `health`
    server.on("/health", []() {
        const size_t capacity = JSON_OBJECT_SIZE(3) + 60;
        String resJSON;
        DynamicJsonDocument resHealth(capacity);

        resHealth["status"] = "OK";
        resHealth["errorCode"] = 0;
        resHealth["message"] = "";
        serializeJson(resHealth, resJSON);
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "application/json", resJSON);
    });

    // handle `/404`
    server.onNotFound([]() {
        // server.send(404, "text/plain", "404: File Not Found");
        server.send(404, "text/html", html404Page);
    });

    server.begin();
}

void startStepper()
{
    // Set the maximum steps per second:
    stepper.setMaxSpeed(1000);
    // Set the maximum acceleration in steps per second^2:
    stepper.setAcceleration(200);

    // stepper.setSpeed(1000);
    // stepper.moveTo(0);
}

void startLedActivityIndicator()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_BUILTIN_AUX, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(LED_BUILTIN_AUX, LOW);
}

void loopLedActivityIndicator(uint16_t interval)
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
