#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "globeServer.h"
#include "GlobeMiddleware.h"
#include "globeStepperMotor.h"
#include "globeMDNS.h"
#include "globeLittleFS.h"
#include "globeGlobalSettings.h"
#include "globeStepperMotor.h"
#include "globeCoordinates.h"

namespace globeServer {

ESP8266WebServer server(80);

// convert the file extension to the MIME type
String getContentType(String filename) {
    if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    return "text/plain";
}

String getObjectCoordinatesGlobeAndStepper() {
    const size_t capacity = 2*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(4) + 120;
    DynamicJsonDocument doc(capacity);

    JsonObject data = doc.createNestedObject("data");

    JsonObject globeObj = data.createNestedObject("globe");
    globeObj["lat"] = globeCoordinates::getLat();
    globeObj["lon"] = globeCoordinates::getLon();
    globeObj["height"] = globeCoordinates::getHeight();
    globeObj["stepperPosition"] = globeStepperMotor::getStepperPosition();

    JsonObject stepperObj = data.createNestedObject("stepper");
    stepperObj["currentPosition"] = globeStepperMotor::currentPosition();
    stepperObj["isRunning"] = globeStepperMotor::isRunning();
    stepperObj["distanceToGo"] = globeStepperMotor::distanceToGo();
    stepperObj["speed"] = globeStepperMotor::speed();

    doc["errorCode"] = 0;

    String res;
    serializeJson(doc, res);

    return res;
}

// send the right file to the client (if it exists)
bool handleFileRead(String path) {
    const char* path404 = "/404.html";
    USE_SERIAL.println("handleFileRead: " + path);

    // If a folder is requested, send the index file
    if (path.endsWith("/")) {
        path += "index.html";
    }

    String contentType = getContentType(path);

    if (globeLittleFS::exists(path)) {
        // Open it
        File file = globeLittleFS::open(path, "r");
        // And send it to the client
        size_t sent = server.streamFile(file, contentType);
        // Then close the file again
        file.close();
        return true;
    } else if(globeLittleFS::exists(path404)) {
        // Open it
        File file404 = globeLittleFS::open(path404, "r");
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

void coordHandle() {
    // send json with coordinates
    String res = getObjectCoordinatesGlobeAndStepper();

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", res);
}

void centerGlobeHandle() {
    server.sendHeader("Access-Control-Allow-Origin", "*");

    // Checa se foi recebido o body da requisicao
    if (server.hasArg("plain") == false)
    {
        const size_t capacity = JSON_OBJECT_SIZE(2) + 40;
        DynamicJsonDocument doc(capacity);

        doc["data"] = "Body não recebido";
        doc["errorCode"] = 1;

        String res;
        serializeJson(doc, res);
        server.send(400, "application/json", res);

        return;
    }

    // --- BODY ---
    const size_t capacityBody = JSON_OBJECT_SIZE(2) + 20;
    DynamicJsonDocument bodyDoc(capacityBody);

    // Parsing body
    deserializeJson(bodyDoc, server.arg("plain"));
    bool setCenter = bodyDoc["setCenter"];
    int steps = bodyDoc["steps"];

    if (setCenter && steps == 0) {
        globeStepperMotor::setCurrentPosition(0);
    } else if (steps != 0 && setCenter == false) {
        globeStepperMotor::move(steps);
        globeStepperMotor::runToPosition();
    } else {
        const size_t capacityRes = JSON_OBJECT_SIZE(2) + 40;
        DynamicJsonDocument resDoc(capacityRes);

        resDoc["data"] = "Operacao invalida";
        resDoc["errorCode"] = 1;

        String res;
        serializeJson(resDoc, res);
        server.send(400, "application/json", res);

        return;
    }

    // --- RESPONSE --- Json com as coordenadas do Globo e Motor
    String res = getObjectCoordinatesGlobeAndStepper();
    server.send(200, "application/json", res);
}

void healthHandle() {
    // https://arduinojson.org/v6/assistant/
    const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(8) + 180;
    DynamicJsonDocument doc(capacity);

    doc["status"] = "OK";
    doc["errorCode"] = 0;

    JsonObject data = doc.createNestedObject("data");
    data["ssid"] = WiFi.SSID();
    data["ip"] = WiFi.localIP().toString();
    data["mDNSHostname"] = String(globeMDNS::getMDNS_ESP_HOSTNAME()) + ".local";
    data["mDNS"] = globeMDNS::isRunning();
    data["hostname"] = WiFi.hostname();
    data["macAddress"] = WiFi.macAddress();
    data["chipId"] = "0x" + String(ESP.getChipId(), HEX);
    data["msStarted"] = millis();

    String res;
    serializeJson(doc, res);

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", res);
}

void restartHandle() {
    const size_t capacity = JSON_OBJECT_SIZE(2) + 40;
    DynamicJsonDocument doc(capacity);

    globeStepperMotor::goToOrigin();  // Voltar globo para origem

    doc["status"] = "restarting";
    doc["errorCode"] = 0;

    String res;
    serializeJson(doc, res);

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", res);
    delay(2000);
    ESP.restart();
}

void notFoundHandle() {
    // Procura uma arquivo no Filesystem e envia se existir
    // se não encontrar tenta enviar a pagina 404.html
    // se não estiver disponivel a página envia uma mensagem 404 (Not Found)
    if (handleFileRead(server.uri()) == false) {
        server.send(404, "text/plain", "404: Not Found - " + server.uri());
    }
}

void start() {
    server.addHandler(new GlobeMiddleware("")); // Custom ESP8266WebServer RequestHandler
    server.on("/coord", coordHandle); // handle `/coord`
    server.on("/center-globe", HTTP_POST, centerGlobeHandle); // handle `center-globe`
    server.on("/health", healthHandle); // handle `/health`
    server.on("/restart", restartHandle); // handle `/restart`
    server.onNotFound(notFoundHandle); // Lida com uri's nao listados acima
    // server.enableCORS(true);
    server.begin();
}

void loop() {
    server.handleClient();
}

}
