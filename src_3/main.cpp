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
// #include <Stepper.h>
// #include <StepperMotorGlobe.h>
#include <AccelStepper.h>
#include <Hash.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"

#define STEPS 2048
#define IN1   D5   // IN1 is connected to NodeMCU pin D1 (GPIO5)
#define IN2   D6   // IN2 is connected to NodeMCU pin D2 (GPIO4)
#define IN3   D7   // IN3 is connected to NodeMCU pin D3 (GPIO0)
#define IN4   D8   // IN4 is connected to NodeMCU pin D4 (GPIO2)

///

// #define A        IN1                     // the pin connected to the wire A of the coil A (or to the H-bridge pin controlling the same wire)
// #define A_bar    IN2                     // the pin connected to the wire A- of the coil A (or to the H-bridge pin controlling the same wire)
// #define B        IN3                     // the pin connected to the wire B of the coil A (or to the H-bridge pin controlling the same wire)
// #define B_bar    IN4                     // the pin connected to the wire B- of the coil A (or to the H-bridge pin controlling the same wire)
// // #define x        3000                  // smaller values may make the motor produce more speed and less torque
// #define stepsPerRevolution 200         // you can the number of steps required to make a complete revolution in the data sheet of your motor
int x = 5000;

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper library with 28BYJ-48 stepper motor:
AccelStepper stepper = AccelStepper(8, IN1, IN3, IN2, IN4);

const char htmlPage[] PROGMEM = R"=====(
    <html>
    <head>
    <meta http-equiv="content-type" content="text/html;charset=UTF-8">
    <title>Hello ESP8266</title>
    <style>
    p { text-indent: 30px; margin: 0px; }
    </style>
    </head>
    <body>
        <h1>HELLO</h1>
        <p>Hello from ESP8266.</p>
    </body>
    </html>
)=====";

ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// initialize stepper library
// Stepper stepper(STEPS, IN4, IN2, IN3, IN1);

#define USE_SERIAL Serial

// void initStepper()
// {
//     stepper.setSpeed(10);  // set stepper motor speed to 10 rpm
//     stepper.step(1024);

//     digitalWrite(IN1, LOW);
//     digitalWrite(IN2, LOW);
//     digitalWrite(IN3, LOW);
//     digitalWrite(IN4, LOW);
// }

void initArduinoOTA()
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

void setup()
{
    // USE_SERIAL.begin(921600);
    USE_SERIAL.begin(9600);

    //Serial.setDebugOutput(true);
    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for (uint8_t t = 4; t > 0; t--)
    {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP(SECRET_SSID, SECRET_PASS);

    while (WiFiMulti.run() != WL_CONNECTED)
    {
        delay(100);
    }

    initArduinoOTA();

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    if(MDNS.begin("esp8266")) {
        USE_SERIAL.println("MDNS responder started");
    }

    // handle index
    server.on("/", []() {
        // send index.html
        // server.send(200, "text/html", "<html><head><script>var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);connection.onopen = function () {  connection.send('Connect ' + new Date()); }; connection.onerror = function (error) {    console.log('WebSocket Error ', error);};connection.onmessage = function (e) {  console.log('Server: ', e.data);};function sendRGB() {  var r = parseInt(document.getElementById('r').value).toString(16);  var g = parseInt(document.getElementById('g').value).toString(16);  var b = parseInt(document.getElementById('b').value).toString(16);  if(r.length < 2) { r = '0' + r; }   if(g.length < 2) { g = '0' + g; }   if(b.length < 2) { b = '0' + b; }   var rgb = '#'+r+g+b;    console.log('RGB: ' + rgb); connection.send(rgb); }</script></head><body>LED Control:<br/><br/>R: <input id=\"r\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>G: <input id=\"g\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>B: <input id=\"b\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/></body></html>");
        // RAM:   [====      ]  37.6% (used 30836 bytes from 81920 bytes)
        // Flash: [===       ]  33.1% (used 345232 bytes from 1044464 bytes)
        // server.send(200, "text/html", "<!DOCTYPE html><html><body><h1>Meu primeiro Hello World!</h1></body></html>");
        // RAM:   [====      ]  38.1% (used 31172 bytes from 81920 bytes)
        // Flash: [===       ]  33.1% (used 345584 bytes from 1044464 bytes)
        // String message = R"=====(


        // server.send (200, "text/html", message);
        // RAM:   [====      ]  37.5% (used 30760 bytes from 81920 bytes)
        // Flash: [===       ]  33.1% (used 345572 bytes from 1044464 bytes)
        server.send (200, "text/html", htmlPage);
    });
    server.on("/status", []() {
        // send index.html
        // server.send(200, "text/html", "<html><head><script>var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);connection.onopen = function () {  connection.send('Connect ' + new Date()); }; connection.onerror = function (error) {    console.log('WebSocket Error ', error);};connection.onmessage = function (e) {  console.log('Server: ', e.data);};function sendRGB() {  var r = parseInt(document.getElementById('r').value).toString(16);  var g = parseInt(document.getElementById('g').value).toString(16);  var b = parseInt(document.getElementById('b').value).toString(16);  if(r.length < 2) { r = '0' + r; }   if(g.length < 2) { g = '0' + g; }   if(b.length < 2) { b = '0' + b; }   var rgb = '#'+r+g+b;    console.log('RGB: ' + rgb); connection.send(rgb); }</script></head><body>LED Control:<br/><br/>R: <input id=\"r\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>G: <input id=\"g\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>B: <input id=\"b\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/></body></html>");
        server.send(200, "text/plain", MDNS.isRunning() ? "true" : "false");
    });
    server.on("/globe", []() {
        server.sendHeader("Location", String("https://guilhermerodrigues680.github.io/website-terrestrial-globe-with-esp8266/"), true);
        server.send (302, "text/plain", "");
    });
    server.on("/config", []() {
        String message = "Number of args received:";
        message += server.args();
        for (int i = 0; i < server.args(); i++) {
            message += "Arg nº" + (String)i + " –> ";
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
            x = server.arg("vel").toInt();
        }


        server.send (200, "text/plain", message);
    });

    server.begin();

    // Add service to MDNS
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);

    USE_SERIAL.println("Ready");
    USE_SERIAL.print("IP address: ");
    USE_SERIAL.println(WiFi.localIP());

    // initStepper();

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);


    //////

    // pinMode(IN1, OUTPUT);
    // pinMode(IN2, OUTPUT);
    // pinMode(IN3, OUTPUT);
    // pinMode(IN4, OUTPUT);

    // digitalWrite(IN1, LOW);
    // digitalWrite(IN2, LOW);
    // digitalWrite(IN3, LOW);
    // digitalWrite(IN4, HIGH);

    /////

    // Set the maximum steps per second:
    stepper.setMaxSpeed(1000);
}

void loop()
{
    ArduinoOTA.handle();
    webSocket.loop();
    server.handleClient();
    MDNS.update();


    ////
    // digitalWrite(IN1, HIGH);
    // digitalWrite(IN2, LOW);
    // digitalWrite(IN3, LOW);
    // digitalWrite(IN4, LOW);
    // delayMicroseconds(5000);

    // digitalWrite(IN1, LOW);
    // digitalWrite(IN2, LOW);
    // digitalWrite(IN3, HIGH);
    // digitalWrite(IN4, LOW);
    // delayMicroseconds(5000);

    // digitalWrite(IN1, LOW);
    // digitalWrite(IN2, HIGH);
    // digitalWrite(IN3, LOW);
    // digitalWrite(IN4, LOW);
    // delayMicroseconds(5000);

    // digitalWrite(IN1, LOW);
    // digitalWrite(IN2, LOW);
    // digitalWrite(IN3, LOW);
    // digitalWrite(IN4, HIGH);
    // delayMicroseconds(5000);

    ////

    // digitalWrite(IN1, HIGH);
    // digitalWrite(IN2, LOW);
    // digitalWrite(IN3, LOW);
    // digitalWrite(IN4, HIGH);
    // delayMicroseconds(5000);

    // digitalWrite(IN1, LOW);
    // digitalWrite(IN2, HIGH);
    // digitalWrite(IN3, HIGH);
    // digitalWrite(IN4, LOW);
    // delayMicroseconds(5000);

    // digitalWrite(IN1, HIGH);
    // digitalWrite(IN2, HIGH);
    // digitalWrite(IN3, LOW);
    // digitalWrite(IN4, LOW);
    // delayMicroseconds(5000);

    // digitalWrite(IN1, LOW);
    // digitalWrite(IN2, LOW);
    // digitalWrite(IN3, HIGH);
    // digitalWrite(IN4, HIGH);
    // delayMicroseconds(5000);

    ////

    // digitalWrite(A, HIGH);
    // digitalWrite(A_bar, LOW);
    // digitalWrite(B, LOW);
    // digitalWrite(B_bar, LOW);
    // delayMicroseconds (x);

    // digitalWrite(A, HIGH);
    // digitalWrite(A_bar, LOW);
    // digitalWrite(B, HIGH);
    // digitalWrite(B_bar, LOW);
    // delayMicroseconds (x);

    // digitalWrite(A, LOW);
    // digitalWrite(A_bar, LOW);
    // digitalWrite(B, HIGH);
    // digitalWrite(B_bar, LOW);
    // delayMicroseconds (x);

    // digitalWrite(A, LOW);
    // digitalWrite(A_bar, HIGH);
    // digitalWrite(B, HIGH);
    // digitalWrite(B_bar, LOW);
    // delayMicroseconds (x);

    // digitalWrite(A, LOW);
    // digitalWrite(A_bar, HIGH);
    // digitalWrite(B, LOW);
    // digitalWrite(B_bar, LOW);
    // delayMicroseconds (x);

    // digitalWrite(A, LOW);
    // digitalWrite(A_bar, HIGH);
    // digitalWrite(B, LOW);
    // digitalWrite(B_bar, HIGH);
    // delayMicroseconds (x);

    // digitalWrite(A, LOW);
    // digitalWrite(A_bar, LOW);
    // digitalWrite(B, LOW);
    // digitalWrite(B_bar, HIGH);
    // delayMicroseconds (x);

    // digitalWrite(A, HIGH);
    // digitalWrite(A_bar, LOW);
    // digitalWrite(B, LOW);
    // digitalWrite(B_bar, HIGH);
    // delayMicroseconds (x);

    // Set the speed of the motor in steps per second:
    stepper.setSpeed(1000);
    // Step the motor with constant speed as set by setSpeed():
    stepper.runSpeed();

}
