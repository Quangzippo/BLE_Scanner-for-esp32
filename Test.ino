#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <vector>
BLEScan* pBLEScan;
BLEClient*  pClient;
const int scanTime = 5; // Thời gian quét (giây)

// UUID của dịch vụ và đặc tính trên BLE Server
static BLEUUID serviceUUID_A("7cddf5af-453f-40fa-808e-37ae6ad8facd");
static BLEUUID charUUID_A("3383a686-a53c-42e4-a88c-e0e1de4c4bda");
static BLEUUID serviceUUID_B("69e2c127-7db7-469c-8852-b0a542e15623");
static BLEUUID charUUID_B("2916187f-e651-4c42-9144-d91262892b22");
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic *pRemoteCharacteristic_A;
static BLERemoteCharacteristic *pRemoteCharacteristic_B;
static BLEAdvertisedDevice *myDevice;
//vector <BLEAdvertisedDevice> myDevice;



// Parameters for distance calculation
const int A = -95; // RSSI at 1 meter (example value, adjust based on your environment)
const float n = 2.4; // Path-loss exponent (adjust based on your environment)

float calculateDistanceB(int rssi_B) {
    return pow(10, (A - rssi_B) / (10 * n));
}
float calculateDistanceA(int rssi_A) {
    return pow(10, (A - rssi_A) / (10 * n));
}

// Tạo lớp kế thừa từ BLEClientCallbacks
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pClient) {}

  void onDisconnect(BLEClient* pClient) {
        connected = false;
  }
};
bool connectToServerB() {
        pClient = BLEDevice::createClient();
        pClient->setClientCallbacks(new MyClientCallback());
        pClient->connect(myDevice); // Kết nối đến thiết bị đã quét được
        pClient->setMTU(517);  //set client to request maximum MTU from server (default is 23 otherwise)


        // Tìm dịch vụ trên thiết bị
        BLERemoteService* pRemoteService_B = pClient->getService(serviceUUID_B);
        if (pRemoteService_B == nullptr) {
          Serial.println("Failed to find service.");
          Serial.println(serviceUUID_B.toString().c_str());
          pClient->disconnect();
          return false;
        }
        

        // Tìm đặc tính nhiệt độ và độ ẩm
        BLERemoteCharacteristic* pRemoteCharacteristic_B = pRemoteService_B->getCharacteristic(charUUID_B);
        if (pRemoteCharacteristic_B == nullptr) {
          Serial.println("Failed to find characteristic.");
          Serial.println(charUUID_B.toString().c_str());
          pClient->disconnect();
          return false;
        }
        

        // Đọc giá trị từ đặc tính
       if (pRemoteCharacteristic_B->canRead()) {
        String value_B = pRemoteCharacteristic_B->readValue();
        Serial.print("Characteristic value: ");
        Serial.println(value_B.c_str());
      } else {
        Serial.println("Characteristic value (Temperature, Humidity) B : ");
      }
        connected = true;
        return false;
}

bool connectToServerA() {
        pClient = BLEDevice::createClient();
        pClient->setClientCallbacks(new MyClientCallback());
        pClient->connect(myDevice); // Kết nối đến thiết bị đã quét được
        pClient->setMTU(517);  //set client to request maximum MTU from server (default is 23 otherwise)


        // Tìm dịch vụ trên thiết bị
        BLERemoteService* pRemoteService_A = pClient->getService(serviceUUID_A);
        if (pRemoteService_A == nullptr) {
          Serial.println("Failed to find service.");
          Serial.println(serviceUUID_A.toString().c_str());
          pClient->disconnect();
          return false;
        }

        // Tìm đặc tính nhiệt độ và độ ẩm
        BLERemoteCharacteristic* pRemoteCharacteristic_A = pRemoteService_A->getCharacteristic(charUUID_A);
        if (pRemoteCharacteristic_A == nullptr) {
          Serial.println("Failed to find characteristic.");
          Serial.println(charUUID_A.toString().c_str());
          pClient->disconnect();
          return false;
        }
        //Serial.println(" - Found our characteristic");

        // Đọc giá trị từ đặc tính
       if (pRemoteCharacteristic_A->canRead()) {
        String value_A = pRemoteCharacteristic_A->readValue();
        Serial.print("Characteristic value: ");
        Serial.println(value_A.c_str());
      } else {
        Serial.println("Characteristic value (Temperature, Humidity) A:  ");
      }
        connected = true;
        return false;
}
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      // Kiểm tra nếu thiết bị có dịch vụ với UUID mong muốn
      if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID_A)) {
        Serial.println("Found device with matching service UUID A!");
            int rssi_A = advertisedDevice.getRSSI();
            Serial.printf("RSSI: %d\n", rssi_A);
            Serial.printf("Tx Power: %d dBm\n", advertisedDevice.getTXPower());
            float distance_A = calculateDistanceA(rssi_A);
            Serial.printf("Estimated Distance: %.2f meters\n", distance_A);
            pBLEScan->stop(); // Dừng quét sau khi tìm thấy thiết bị
            myDevice = new BLEAdvertisedDevice(advertisedDevice);     
            doConnect = true;
            doScan = true;
      }
      if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID_B)) {
        Serial.println("Found device with matching service UUID B!");
            int rssi_B = advertisedDevice.getRSSI();
            Serial.printf("RSSI: %d\n", rssi_B);
            Serial.printf("Tx Power: %d dBm\n", advertisedDevice.getTXPower());
            float distance_B = calculateDistanceB(rssi_B);
            Serial.printf("Estimated Distance: %.2f meters\n", distance_B);
            pBLEScan->stop(); // Dừng quét sau khi tìm thấy thiết bị
            myDevice = new BLEAdvertisedDevice(advertisedDevice);     
            doConnect = true;
            doScan = true;
      }
    }
};

void setup() {
  Serial.begin(115200);
  
  // Khởi tạo BLE
  BLEDevice::init("");
  
  // Khởi tạo BLE Scanner
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop() {
  Serial.println("Scanning...");
  pBLEScan->start(scanTime, false); // Quét trong thời gian `scanTime`
  if (doConnect == true) {
    if (connectToServerA()) {
      Serial.println("Connected to the BLE Server A.");
    }
    doConnect = false;
    }
  if (doConnect == true) {
    if (connectToServerB()) {
      Serial.println("Connected to the BLE Server B.");
    }
    doConnect = false;
    }
  pBLEScan->clearResults(); // Xóa kết quả sau mỗi lần quét
  delay(2000); // Chờ 2 giây rồi quét lại
}
