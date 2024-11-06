#include <Arduino.h>
#include <math.h>
#include <ESP32-TWAI-CAN.hpp>


// Default for ESP32
#define CAN_TX 5
#define CAN_RX 4


void sendObdFrame(uint8_t obdId) {
    CanFrame obdFrame = { 0 };
    obdFrame.identifier = 0x7DF; // Default OBD2 address;
    obdFrame.extd = 0;
    obdFrame.data_length_code = 8;
    obdFrame.data[0] = 2;
    obdFrame.data[1] = 1;
    obdFrame.data[2] = obdId;
    obdFrame.data[3] = 0xAA;    // Best to use 0xAA (0b10101010) instead of 0
    obdFrame.data[4] = 0xAA;    // CAN works better this way as it needs
    obdFrame.data[5] = 0xAA;    // to avoid bit-stuffing
    obdFrame.data[6] = 0xAA;
    obdFrame.data[7] = 0xAA;
    // Accepts both pointers and references 
    ESP32Can.writeFrame(obdFrame);  // timeout defaults to 1 ms
}

void sendFloatFrame(double num, int id) {
  uint8_t *array;
  array = (uint8_t*)(&num);
  CanFrame Frame = {0};
  int N = sizeof(double);
  for(int i = 0; i < N; i++) {
    Frame.data[i] = array[i];
  };
  Frame.identifier = id;
  Frame.extd = 0;
  Frame.data_length_code = N;
  ESP32Can.writeFrame(Frame);
}

void setup() {
  Serial.begin(115200);
  ESP32Can.setPins(CAN_TX, CAN_RX);
  Serial.println("CANBUS - dummy message sender!");

  ESP32Can.setRxQueueSize(5);
  ESP32Can.setTxQueueSize(5);
  ESP32Can.setSpeed(ESP32Can.convertSpeed(500));
  
  if(ESP32Can.begin()) {
      Serial.println("CAN bus started!");
  } else {
      Serial.println("CAN bus failed!");
  }
  Serial.println("Start sending messages!");
}

void loop() {
  // send packet: id is 11 bits, packet can contain up to 8 bytes of data
  Serial.print("Sending packet ... ");
  sendObdFrame(5); 
  Serial.println("done");
  delay(100);
  auto time_ms = millis();
  sendFloatFrame(sin(0.0001*time_ms), 0x75);
}
