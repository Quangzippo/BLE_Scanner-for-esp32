#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <DHT.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_Sensor.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "69e2c127-7db7-469c-8852-b0a542e15623"
#define CHARACTERISTIC_UUID "2916187f-e651-4c42-9144-d91262892b22"

// Khởi tạo DHT11 với chân dữ liệu
#define DHTPIN 21     // Chân DHT11
#define DHTTYPE DHT11   // Cảm biến DHT11
DHT dht(DHTPIN, DHTTYPE);

BLECharacteristic *pCharacteristic = NULL;
BLEServer *pServer = NULL;

// // Hàm callback khi có client kết nối
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("Client kết nối!");
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.println("Client ngắt kết nối!");
  }
};

// Thiết lập giá trị nhiệt độ và độ ẩm để client có thể đọc
void updateSensorValues() {
  // Đọc nhiệt độ và độ ẩm từ DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Lỗi khi đọc từ cảm biến DHT11!");
    return;
  }  
  //Chuyển đổi giá trị thành dạng int16_t
  int16_t tempInt = temperature * 100;  // Nhiệt độ được nhân với 100 để chuyển thành int16_t
  int16_t humInt = humidity * 100;      // Độ ẩm cũng được nhân với 100

  String data = String(temperature) + "," + String(humidity);

  // Cập nhật giá trị đặc tính để client có thể đọc
  pCharacteristic->setValue(data.c_str());  // Đặt 4 byte dữ liệu vào đặc tính
  pCharacteristic->notify();

  // Hiển thị giá trị nhiệt độ và độ ẩm lên serial monitor
  Serial.print("Nhiệt độ: ");
  Serial.print(temperature);
  Serial.println(" °C");
  
  Serial.print("Độ ẩm: ");
  Serial.print(humidity);
  Serial.println(" %");
  delay(2000);  // Update values every 2 seconds
}


void setup() {
  Serial.begin(115200);
  // Khởi tạo cảm biến DHT11
  dht.begin();

  Serial.println("Starting BLE work!");
  BLEDevice::init("ESP32 Server B");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                                         BLEUUID(CHARACTERISTIC_UUID),
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );

  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Server B is Advertising");
}
void loop() {
  // put your main code here, to run repeatedly:
  updateSensorValues();
  delay(2000);
}