void initWifi () {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setHostname(hostname);
  ArduinoOTA.setHostname(hostname);

  // Wait 10sec for valid connection
  for (int connectingTimeout = 100; connectingTimeout > 0; connectingTimeout --) {
    if (connectingTimeout == 0 && WiFi.status() != WL_CONNECTED) ESP.restart();
    if (WiFi.status() == WL_CONNECTED) break;
    delay (100);
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request){
    request->redirect("/");
    ESP.restart();
  });

  server.on("/otaUpdate", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "Waiting for OTA-Update ...");
    startUpdate();
    request->redirect("/");
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String myHTML = "<center><FONT SIZE=\"4\">";
    myHTML += "<a href=/allJSON>allJSON</a>";
    myHTML += "&nbsp&nbsp<a href=/reboot>Reboot</a>";
    myHTML += "&nbsp&nbsp<a href=/update>Web-Update</a>";
    myHTML += "&nbsp&nbsp<a href=/otaUpdate>OTA-Update</a>";
    myHTML += "<br>Uptime: " + String(millis()/1000);
    myHTML += "<br><br> Devices: " + String(myFoundDevicesCount) + "<br>";
    for (int deviceIndex = 0; deviceIndex < myFoundDevicesCount; deviceIndex ++) {
      String myAdress = (myFoundDevices[deviceIndex].getAddress().toString().c_str());
      myHTML += "<a href=/" + myAdress + ">" + myAdress + "</a>" + "<br>";
    }
    myHTML += "</FONT></center>";
    request->send(200, "text/html", myHTML);
  });

  server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", jsonToString(makeJSON_Status()));
  });

  server.on("/allJSON", HTTP_GET, [](AsyncWebServerRequest *request){
    if (updating) return;
    request->send(200, "application/json", jsonToString(makeJSON_DeviceList(myFoundDevices)));
  });

  server.onNotFound([](AsyncWebServerRequest *request){
    if (updating) return;
    String requestString = request->url();
    requestString.toLowerCase(), requestString.replace("-",":");
    
    for (int deviceIndex = 0; deviceIndex < myFoundDevicesCount; deviceIndex ++) {
      String myAdress = (myFoundDevices[deviceIndex].getAddress().toString().c_str());
      if (requestString.indexOf(myAdress) > 0) {
        request->send(200, "application/json", jsonToString(makeJSON_Device(&myFoundDevices[deviceIndex])));
        return;
      }
    }
    request->send(404, "text/plain", "Not found");
  });

  // Simple Firmware Update Form
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>");
  });
  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
    boolean shouldReboot = !Update.hasError();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot?"OK":"FAIL");
    response->addHeader("Connection", "close");
  },[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index){
      startUpdate ();
      Serial.printf("Update Start: %s\n", filename.c_str());
      //Update.runAsync(true);
      if(!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)){
        Update.printError(Serial);
      }
    }
    if(!Update.hasError()){
      if(Update.write(data, len) != len){
        Update.printError(Serial);
      }
    }
    if(final){
      if(Update.end(true)){
        Serial.printf("Update Success: %uB\n", index+len);
        request->redirect("/");
        ESP.restart();
      } else {
        Update.printError(Serial);
      }
    }
  });
  
  ArduinoOTA.begin();
  server.begin();
}
