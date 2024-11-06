#include <Arduino.h>

#include <ESP32-TWAI-CAN.hpp>

#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <WiFi.h>
#include "index.h"

// Replace with your network credentials
const char* ssid = "";
const char* password = "";

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);  // WebSocket server on port 81

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] Received text: %s\n", num, payload);
      // Send a response back to the client
      webSocket.sendTXT(num, "Received:  " + String((char*)payload));
      break;
  }
}


// Default for ESP32
#define CAN_TX 5
#define CAN_RX 4

CanFrame rxFrame;

void setup() {
  Serial.begin(115200);
  delay(1000);
  /////////////////////////////////////////////////////////////////////////////
  //                               CANBUS-SETUP                              //
  /////////////////////////////////////////////////////////////////////////////

  ESP32Can.setPins(CAN_TX, CAN_RX);
  Serial.println("CANBUS - listener");

  ESP32Can.setRxQueueSize(5);
  ESP32Can.setTxQueueSize(5);
  ESP32Can.setSpeed(ESP32Can.convertSpeed(500));
  if (ESP32Can.begin()) {
    Serial.println("CAN bus started!");
  } else {
    Serial.println("CAN bus failed!");
  }
  Serial.println("Start sending messages!");

  /////////////////////////////////////////////////////////////////////////////
  //                                WIFI-SETUP                               //
  /////////////////////////////////////////////////////////////////////////////
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  /////////////////////////////////////////////////////////////////////////////
  //                              WEBAPP-SETUP                               //
  /////////////////////////////////////////////////////////////////////////////
  // Initialize WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // Serve a basic HTML page with JavaScript to create the WebSocket connection
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("Web Server: received a web page request");
    String html = HTML_CONTENT;  // Use the HTML content from the index.h file
    request->send(200, "text/html", html);
  });

  server.begin();
  Serial.print("ESP32 Web Server's IP address: ");
  Serial.println(WiFi.localIP());
  
}

void loop() {
  webSocket.loop(); // non blocking
  
  if (ESP32Can.readFrame(rxFrame, 1000)) {
    // Comment out if too many requests
    Serial.printf("Received frame: %03X \r\n", rxFrame.identifier);
    if (rxFrame.identifier == 0x7DF) {  // Standard OBD2 frame responce ID
      Serial.printf("Collant temp: %3d°C \r\n",
                    rxFrame.data[3] - 40);  // Convert to °C
    }
  }
}
