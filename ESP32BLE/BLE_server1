/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

uint8_t txValue = 0;
BLECharacteristic *pCharacteristic;

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  BLEDevice::init("MyESP32");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  Serial.println("Characteristic defined! Now you can read it in your phone!");

}

void loop() {
  // put your main code here, to run repeatedly:
  //pCharacteristic->setValue(i++);
  digitalWrite(2, HIGH);
  delay(1500);
  digitalWrite(2, LOW);
  delay(500);
  if (txValue > 255) {
    txValue = 0;
  }
  pCharacteristic->setValue(&txValue, 1);
  Serial.printf("*** Sent Value: %d ***\n", txValue);
  txValue++;
}
