/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
   And has a characteristic of: beb5483e-36e1-4688-b7f5-ea07361b26a8

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   A connect hander associated with the server starts a background task that performs notification
   every couple of seconds.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t value = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};



void setup() {
  Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init("MyESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

//ESP32 シリアル割り込み処理はきかない  https://github.com/espressif/arduino-esp32/issues/906

void loop() {
  std::string rxValue;
  if (Serial.available() > 0) {
    Serial.print("setVal:");
    for (int i = 0; Serial.available() > 0 && i < 20; i++) {
      rxValue += Serial.read();
      Serial.print(rxValue[i]);
    }
    Serial.println();
    // notify changed value
    if (deviceConnected) {
      pCharacteristic->setValue(rxValue);
      pCharacteristic->notify();
      delay(10); // bluetooth stack will go into congestion, if too many packets are sent
    }
  }
  // GATT Indicate
  //Notification と同様にデバイスからの通知に用います。
  //基本的な動作はNotificationと同じですがIndicateでは通知したデータが受け取られたかどうかをBLEデバイス側で応答確認できます。
  //なお Indicateと Notificationは内部的に同じ機構を用いており、通知時にどちらが使用されたか判定ができません。
  //そのため Indicateと Notification同時に動作させないよう気をつけてください。
  // http://bril-tech.blogspot.com/2015/05/bluetoothsmartmbed-6.html

  // リモート特性表示を受信して​​いないBLEクライアント
  // https://github.com/nkolban/esp32-snippets/issues/397

  //BLEの表示/通知の問題
  //https://github.com/espressif/esp-idf/issues/568
  
  //esp_ble_gatts_send_indicate BLEで一度に2つの特性
  //https://www.esp32.com/viewtopic.php?t=5266


  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}
