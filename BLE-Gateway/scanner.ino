void initBLEScanner () {
  BLEDevice::deinit(false);
  BLEDevice::init("");
  BLEDevice::setPower (ESP_PWR_LVL_P9);
  
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), true);
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(100);  // less or equal setInterval value
}

void startBLEScanning () {
  Serial.println("Scanning...");
  pBLEScan->start(SCAN_TIME, false);
}

void stopBLEScanning () {
  Serial.println("Stop Scanning");
  pBLEScan->stop();
  pBLEScan->clearResults();
}

void serialPrintBLEDevice (BLEAdvertisedDevice myDevice) {
  Serial.print("Address: ");
  Serial.print(myDevice.getAddress().toString().c_str());
  Serial.print("  Name: ");
  Serial.print(myDevice.getName().c_str());
  char *pHex = BLEUtils::buildHexData(nullptr, (uint8_t*)myDevice.getManufacturerData().data(), myDevice.getManufacturerData().length());
  String manufacturerData = pHex;
  free(pHex);
  Serial.print("  ManufacturerData: ");
  Serial.println(manufacturerData);
}
