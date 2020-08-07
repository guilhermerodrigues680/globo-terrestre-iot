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
#include <LittleFS.h>
#include <AccelStepper.h>
#include <Hash.h>
#include <ArduinoJson.h>
#include "arduinoSecrets.h"
#include "StepperMotorGlobe.h"
#include "RemoteLogging.h"
#include "GlobeServer.h"

// Motor pin definitions:
#define IN1   D5   // IN1 on the ULN2003 driver. Is connected to NodeMCU pin D5
#define IN2   D6   // IN2 on the ULN2003 driver. Is connected to NodeMCU pin D6
#define IN3   D7   // IN3 on the ULN2003 driver. Is connected to NodeMCU pin D7
#define IN4   D8   // IN4 on the ULN2003 driver. Is connected to NodeMCU pin D8

// Define the AccelStepper interface type; 4 wire motor in half step mode:
#define MotorInterfaceType 8

#define USE_SERIAL Serial

// hostname para o mDNS
const char* MDNS_ESP_HOSTNAME = "globo";

ESP8266WiFiMulti WiFiMulti;
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper library with 28BYJ-48 stepper motor:
AccelStepper stepper = AccelStepper(8, IN1, IN3, IN2, IN4);
Globe globe = Globe();
RemoteLogging remLog;

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
String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);       // send the right file to the client (if it exists)

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
    startMDNS();
    startArduinoOTA();
    startWebSocket();
    LittleFS.begin(); // Start the SPI Flash Files System
    startServer();
    startStepper();
    startLedActivityIndicator();
}

void loop()
{
    webSocket.loop();
    server.handleClient();
    ArduinoOTA.handle();
    MDNS.update();
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
        remLog.sendBufferPacket(payload, length);
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
    remLog.sendPacket(WiFi.localIP().toString());
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
    if(MDNS.begin(MDNS_ESP_HOSTNAME)) {
        // Add service to MDNS
        MDNS.addService("http", "tcp", 80);
        MDNS.addService("ws", "tcp", 81);
        USE_SERIAL.println("MDNS responder started");
        USE_SERIAL.println("mDNS responder started: http://esp8266.local");
    } else {
        USE_SERIAL.println("MDNS responder not started!!!");
    }
}

void startServer()
{
    // Custom ESP8266WebServer RequestHandler
    server.addHandler(new GlobeServer("/aboutss1"));

    // handle `/coord`
    server.on("/coord", []() {
        // send json with coordinates
        const size_t capacity = JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(4) + 100;
        DynamicJsonDocument doc(capacity);

        JsonObject globeObj = doc.createNestedObject("globe");
        globeObj["lat"] = globe.getLat();
        globeObj["lon"] = globe.getLon();
        globeObj["height"] = globe.getHeight();
        globeObj["stepperPosition"] = globe.getStepperPosition();

        JsonObject stepperObj = doc.createNestedObject("stepper");
        stepperObj["currentPosition"] = stepper.currentPosition();
        stepperObj["isRunning"] = stepper.isRunning();
        stepperObj["distanceToGo"] = stepper.distanceToGo();
        stepperObj["speed"] = stepper.speed();

        String res;
        serializeJson(doc, res);
        server.send(200, "application/json", res);
    });

    // handle `center-globe`
    server.on("/center-globe", HTTP_POST, []() {
        //Check if body received
        if (server.hasArg("plain") == false)
        {
            const size_t capacity = JSON_OBJECT_SIZE(2) + 40;
            DynamicJsonDocument doc(capacity);

            doc["data"] = "Body not received";
            doc["errorCode"] = 1;

            String res;
            serializeJson(doc, res);

            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(400, "application/json", res);
        }
        else
        {
            // --- BODY ---
            const size_t capacityBody = JSON_OBJECT_SIZE(2) + 20;
            DynamicJsonDocument bodyDoc(capacityBody);
            // Parsing body
            deserializeJson(bodyDoc, server.arg("plain"));
            bool setCenter = bodyDoc["setCenter"];
            int steps = bodyDoc["steps"];

            // --- RESPONSE ---
            String res;
            const size_t capacityRes = JSON_OBJECT_SIZE(2) + 40;
            DynamicJsonDocument resDoc(capacityRes);

            if (setCenter && steps == 0)
            {
                stepper.setCurrentPosition(0);

                resDoc["data"] = "setCenter: " + String(setCenter);
                resDoc["errorCode"] = 0;
                serializeJson(resDoc, res);
                server.sendHeader("Access-Control-Allow-Origin", "*");
                server.send(200, "application/json", res);
            }
            else if (steps != 0 && setCenter == false)
            {
                stepper.move(steps);
                stepper.runToPosition();

                resDoc["data"] = "steps: " + String(steps) + ", setCenter: " + String(setCenter);
                resDoc["errorCode"] = 0;
                serializeJson(resDoc, res);
                server.sendHeader("Access-Control-Allow-Origin", "*");
                server.send(200, "application/json", res);
            }
            else
            {
                resDoc["data"] = "Operacao invalida";
                resDoc["errorCode"] = 1;
                serializeJson(resDoc, res);
                server.sendHeader("Access-Control-Allow-Origin", "*");
                server.send(400, "application/json", res);
            }
        }
    });

    // handle `/health`
    server.on("/health", []() {
        // https://arduinojson.org/v6/assistant/
        const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(8) + 180;
        DynamicJsonDocument doc(capacity);

        doc["status"] = "OK";
        doc["errorCode"] = 0;

        JsonObject data = doc.createNestedObject("data");
        data["ssid"] = WiFi.SSID();
        data["ip"] = WiFi.localIP().toString();
        data["mDNSHostname"] = String(MDNS_ESP_HOSTNAME) + ".local";
        data["mDNS"] = MDNS.isRunning();
        data["hostname"] = WiFi.hostname();
        data["macAddress"] = WiFi.macAddress();
        data["chipId"] = "0x" + String(ESP.getChipId(), HEX);
        data["msStarted"] = millis();

        String res;
        serializeJson(doc, res);

        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "application/json", res);
    });

    // handle `/restart`
    server.on("/restart", []() {
        const size_t capacity = JSON_OBJECT_SIZE(2) + 40;
        DynamicJsonDocument doc(capacity);

        doc["status"] = "restarting";
        doc["errorCode"] = 0;

        String res;
        serializeJson(doc, res);

        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "application/json", res);
        delay(2000);
        ESP.restart();
    });

    // Lida com uri's nao listados acima
    server.onNotFound([]() {
        // Procura uma arquivo no Filesystem e envia se existir
        // se não encontrar tenta enviar a pagina 404.html
        // se não estiver disponivel a página envia uma mensagem 404 (Not Found)
        if (handleFileRead(server.uri()) == false)
        {
            server.send(404, "text/plain", "404: Not Found - " + server.uri());
        }
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

// convert the file extension to the MIME type
String getContentType(String filename) {
    if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    return "text/plain";
}

// send the right file to the client (if it exists)
bool handleFileRead(String path) {
    const char* path404 = "/404.html";
    USE_SERIAL.println("handleFileRead: " + path);

    // If a folder is requested, send the index file
    if (path.endsWith("/"))
    {
        path += "index.html";
    }

    // Get the MIME type
    String contentType = getContentType(path);

    // If the file exists
    if (LittleFS.exists(path))
    {
        // Open it
        File file = LittleFS.open(path, "r");
        // And send it to the client
        size_t sent = server.streamFile(file, contentType);
        // Then close the file again
        file.close();
        return true;
    } else if(LittleFS.exists(path404)) {
        // Open it
        File file404 = LittleFS.open(path404, "r");
        // And send it to the client
        size_t sent = server.streamFile(file404, "text/html");
        // Then close the file again
        file404.close();
        return true;
    }

    // If the file doesn't exist, return false
    USE_SERIAL.println("\tFile Not Found");
    return false;
}
