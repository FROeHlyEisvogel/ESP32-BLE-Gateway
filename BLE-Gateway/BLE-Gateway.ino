// ESP32 Dev
// 4BM Flash-Size
// Minimal SPIFFS

//Bluetooth
#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

//Wifi
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

//OTA
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

AsyncWebServer server(80);

const char* ssid = "***";
const char* password = "***";
const char* hostname = "BLE-Gateway";
boolean updating = false;

//Scanner
BLEScan* pBLEScan;
BLEAdvertisedDevice myFoundDevices[50];
unsigned int myFoundDevicesCount = 0;
#define SCAN_TIME 0    // seconds (0 = infinity)

void serialPrintBLEDevice (BLEAdvertisedDevice myDevice);

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    pBLEScan->clearResults();
    
    for (int deviceIndex = 0; deviceIndex < myFoundDevicesCount; deviceIndex ++) {
      if (myFoundDevices[deviceIndex].getAddress().toString() == advertisedDevice.getAddress().toString()) {
        myFoundDevices[deviceIndex] = advertisedDevice;
        return;
      }
    }
    myFoundDevices[myFoundDevicesCount] = advertisedDevice;
    myFoundDevicesCount += 1;
    
    //serialPrintBLEDevice (advertisedDevice);
  }
};

void startUpdate () {
  updating = true;
  stopBLEScanning ();
}

void setup() {
  Serial.begin(115200);
  
  initWifi();
  initBLEScanner ();
}

void loop() {
  if (updating == false) startBLEScanning ();
  stopBLEScanning ();
  
  ArduinoOTA.handle();
  yield();
}
