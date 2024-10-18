#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <DHT.h>
#include <BLE2902.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID        "7cddf5af-453f-40fa-808e-37ae6ad8facd"
#define CHARACTERISTIC_UUID "3383a686-a53c-42e4-a88c-e0e1de4c4bda"


// Khởi tạo DHT11 với chân dữ liệu
#define DHTPIN 21     // Chân DHT11
#define DHTTYPE DHT11   // Cảm biến DHT11
DHT dht(DHTPIN, DHTTYPE);

// Cấu hình OLED SSD1306
// #define SCREEN_WIDTH 128 // OLED display width, in pixels
// #define SCREEN_HEIGHT 32 // OLED display height, in pixels
// #define OLED_RESET    -1 // Chân reset OLED, nếu không có thì đặt là -1
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
  // // Khởi tạo màn hình OLED
  //   if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Địa chỉ I2C của OLED 0x3C
  //   Serial.println(F("OLED không khởi tạo được"));
  //   for (;;); // Vòng lặp vô tận nếu không khởi tạo được
  // }
  // delay(2000);
  // display.clearDisplay();
  // display.setTextColor(WHITE);


  Serial.println("Starting BLE work!");
  BLEDevice::init("ESP32 Server A");
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
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Server A is Advertising");
}
void loop() {
  // put your main code here, to run repeatedly:
  updateSensorValues();
//   // Đọc dữ liệu từ DHT11
//   float h = dht.readHumidity();
//   float t = dht.readTemperature();
//   // Kiểm tra nếu đọc thành công
//   if (isnan(h) || isnan(t)) {
//     Serial.println("Failed to read from DHT sensor!");
//     return;
//   }
//   //Hiển thị lên màn hình OLED
//   display.clearDisplay();
//   // display temperature
//   display.setTextSize(1);
//   display.setCursor(0,0);
//   display.print("Temperature: ");
//   display.setTextSize(2);
//   display.setCursor(0,10);
//   display.print(t);
//   display.print(" ");
//   display.setTextSize(1);
//   display.cp437(true);
//   display.write(167);
//   display.setTextSize(2);
//   display.print("C");

//  // display humidity
//   display.setTextSize(1);
//   display.setCursor(0, 35);
//   display.print("Humidity: ");
//   display.setTextSize(2);
//   display.setCursor(0, 45);
//   display.print(h);
//   display.print(" %"); 
  
//   display.display(); 
  delay(2000);
}