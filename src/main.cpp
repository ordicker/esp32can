#include <Arduino.h>

#include <ESP32-TWAI-CAN.hpp>

#include <ESPAsyncWebServer.h>
#include <WiFi.h>


// Replace with your network credentials
const char* ssid = "";
const char* password = "";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

AsyncWebSocketClient* wsClient;
 
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    wsClient = client;
  } else if(type == WS_EVT_DISCONNECT){
    wsClient = nullptr;
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

  /////////////////////////////////////////////////////////////////////////////
  //                                WIFI-SETUP                               //
  /////////////////////////////////////////////////////////////////////////////
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  /////////////////////////////////////////////////////////////////////////////
  //                              WEBAPP-SETUP                               //
  /////////////////////////////////////////////////////////////////////////////
  // Start webserver
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();
  
}

void loop() {
  char text[100];
  if(wsClient != nullptr && wsClient->canSend()) {
    if (ESP32Can.readFrame(rxFrame, 1000)) {
      wsClient->text("DATA: \n");
      if (rxFrame.identifier == 0x75) {
        double val = *(double*)rxFrame.data;
        sprintf(text, "Received frame: %03X \r\n DATA: %f", rxFrame.identifier, val);
      }      
      wsClient->text(text);
    }
  }
  // Wait 10 ms
  delay(10);

  
  if (ESP32Can.readFrame(rxFrame, 1000)) {
    // Comment out if too many requests
    Serial.printf("Received frame: %03X \r\n", rxFrame.identifier);
    if (rxFrame.identifier == 0x7DF) {  // Standard OBD2 frame responce ID
      Serial.printf("Collant temp: %3d°C \r\n",
                    rxFrame.data[3] - 40);  // Convert to °C
    }
  }
}
