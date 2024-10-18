#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <DHT.h>

// #define DHTPIN 21     // Chân kết nối với cảm biến DHT
// #define DHTTYPE DHT11  // Chọn loại cảm biến: DHT11 hoặc DHT22

// DHT dht(DHTPIN, DHTTYPE);

// Tên và UUID cho BLE Service và Characteristics
#define SERVICE_UUID        "407fc20d-d5f5-4176-bb4d-cc9c1968f9f6"
#define CHARACTERISTIC_UUID "116b8358-35b9-40a5-8e5b-c71031046c2f"

// BLEServer* pServer = NULL;
// BLECharacteristic* pCharacteristic = NULL;
// bool deviceConnected = false;
// float temperature = 0.0;
// float humidity = 0.0;

// class MyServerCallbacks: public BLEServerCallbacks {
//   void onConnect(BLEServer* pServer) {
//     deviceConnected = true;
//   };

//   void onDisconnect(BLEServer* pServer) {
//     deviceConnected = false;
//   }
// };

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  // dht.begin();

  // Khởi tạo BLE
  BLEDevice::init("ESP32 Server C");
  BLEServer *pServer = BLEDevice::createServer();
  // pServer->setCallbacks(new MyServerCallbacks());

  // Tạo Service BLE
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Tạo characteristic cho Service
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristic->setValue("Hello!");
  // Bắt đầu dịch vụ
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  // Bắt đầu quảng bá (advertising)
  // pServer->getAdvertising()->start();
  Serial.println("Server C is Advertising");
}

void loop() {
  delay(2000);
}
