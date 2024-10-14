#include <Arduino.h>
#include <ESP32-TWAI-CAN.hpp>

// Default for ESP32
#define CAN_TX 5
#define CAN_RX 4

void setup() {
  Serial.begin(9600);
  ESP32Can.setPins(CAN_TX, CAN_RX);
}

void loop() {
  Serial.println("hello world!");
  delay(1000);

}
