String jsonToString (DynamicJsonDocument jsonDoc) {
  String myReturnString = "";
  serializeJson (jsonDoc, myReturnString);
  return myReturnString;
}

DynamicJsonDocument makeJSON_Status () {
  DynamicJsonDocument jsonDoc(500);

  jsonDoc["uptime"] = (unsigned long int)(millis()/1000);
  jsonDoc["devices"] = myFoundDevicesCount;
  jsonDoc["wifiRSSI"] = WiFi.RSSI();
  
  return jsonDoc;
}

DynamicJsonDocument makeJSON_DeviceList (BLEAdvertisedDevice* foundDevices) {
  DynamicJsonDocument jsonDoc(5000);

  jsonDoc["devices"]["count"] = myFoundDevicesCount;

  for (int deviceIndex = 0; deviceIndex < myFoundDevicesCount; deviceIndex ++) {
    String myAddress = foundDevices[deviceIndex].getAddress().toString().c_str();
    jsonDoc["devices"][myAddress] = makeJSON_Device (&foundDevices[deviceIndex]);
  }

  return jsonDoc;
}

DynamicJsonDocument makeJSON_Device (BLEAdvertisedDevice* myFoundDevice) {
  StaticJsonDocument<500> jsonDoc;
    
  String myAddress = myFoundDevice->getAddress().toString().c_str();
  String myName = myFoundDevice->getName().c_str();
  String myAppearance = "";
  if (myFoundDevice->haveAppearance()) myAppearance = myFoundDevice->getAppearance();
  String myUUID = "";
  if (myFoundDevice->haveServiceUUID()) myUUID = myFoundDevice->getServiceUUID().toString().c_str();
  int myRSSI = 0;
  if (myFoundDevice->haveRSSI()) myRSSI = myFoundDevice->getRSSI();
  int myTxPower = myFoundDevice->getTXPower();
  if (myFoundDevice->haveTXPower()) myTxPower = myFoundDevice->getTXPower();

  jsonDoc["name"] = myName;
  jsonDoc["address"] = myAddress;
  jsonDoc["appearance"] = myAppearance;
  jsonDoc["UUID"] = myUUID;
  jsonDoc["RSSI"] = myRSSI;
  jsonDoc["txPower"] = myTxPower;

  //addJSON_Test (&jsonDoc);

  if (myFoundDevice->haveManufacturerData()) {
    String myManufacturerData = BLEUtils::buildHexData(nullptr, (uint8_t*)myFoundDevice->getManufacturerData().data(), myFoundDevice->getManufacturerData().length());
    jsonDoc["manufacturerData"] = "0x" + myManufacturerData;

    if (myName == "ClimaSens") jsonDoc["climasens"] = (makeJSON_ClimaSens (&myManufacturerData));
    if (myManufacturerData.substring(0,4) == "dd03") jsonDoc["STIHLconnector"] = (makeJSON_STIHLConnector (&myManufacturerData));
  }
  
  return jsonDoc;
}

void addJSON_Test (DynamicJsonDocument* test) {
  //test["test"] = 0;
}

DynamicJsonDocument makeJSON_ClimaSens (String* myManufacturerData) {
  StaticJsonDocument<200> jsonDoc;
  
  float myBattery = (float)strtoul(myManufacturerData->substring(4,8).c_str(), NULL, 16) / 1000;
  float myInternalTemperature = (float)strtoul(myManufacturerData->substring(8,12).c_str(), NULL, 16) / 100;
  unsigned int myLight = strtoul(myManufacturerData->substring(12,16).c_str(), NULL, 16);
  float myTemperature = (float)strtoul(myManufacturerData->substring(16,20).c_str(), NULL, 16) / 100;
  float myHumidity = (float)strtoul(myManufacturerData->substring(20,24).c_str(), NULL, 16) / 100;
  float myPressure = (float)strtoul(myManufacturerData->substring(24,28).c_str(), NULL, 16) / 10;
  boolean myContact = strtoul(myManufacturerData->substring(28,30).c_str(), NULL, 16) && 0x01;
  boolean myButton = strtoul(myManufacturerData->substring(28,30).c_str(), NULL, 16) && 0x02;

  jsonDoc["battery"] = myBattery;
  jsonDoc["light"] = myLight;
  jsonDoc["internal_temperature"] = myInternalTemperature;
  jsonDoc["temperature"] = myTemperature;
  jsonDoc["humidity"] = myHumidity;
  jsonDoc["pressure"] = myPressure;
  jsonDoc["contact"] = myContact;
  jsonDoc["button"] = myButton;
  
  return jsonDoc;
}

DynamicJsonDocument makeJSON_STIHLConnector (String* myManufacturerData) {
  StaticJsonDocument<200> jsonDoc;
  
  float myBattery = (float)strtoul(myManufacturerData->substring(40,42).c_str(), NULL, 16) * 0.05;
  int myTemperature = strtol(myManufacturerData->substring(42,44).c_str(), NULL, 16);
  long unsigned int myWorkingSeconds = __builtin_bswap32(strtoul(myManufacturerData->substring(26,18).c_str(), NULL, 16));
  unsigned int myStatusID = strtol(myManufacturerData->substring(38,40).c_str(), NULL, 16);
  
  jsonDoc["battery"] = myBattery;
  jsonDoc["temperature"] = myTemperature;
  jsonDoc["workingSeconds"] = myWorkingSeconds;
  jsonDoc["statusID"] = myStatusID;
  
  return jsonDoc;
}
