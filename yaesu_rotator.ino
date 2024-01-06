#include <dummy.h>
#include <stdio.h>
#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include "Yaesu_GS232A.h"
#include "MotorController.h"
#include "defines.h"

//  ***************************************************************
//  Use ESP8266 Board NodeMCU 1.0 (ESP-12E Mdoule)
//

ESP8266WebServer server_http(80);
WiFiServer server_socket(8844);
MotorController_Class motorController;
YAESU_GS232A_Class  rotator;

uint32_t lastSend = 0;
#define TIME_SEND 60000

void setup(void) {
  Serial.begin(SERIAL_SPEED);
  Serial.println("\n\r\n\r" BUILD_VERSION " - " BUILD_DATE "\n\r\n\r");
  rotator.init();
  motorController.init();
}

void checkAndStartWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n\rWiFi NOT started");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("WiFi starting ");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\n\rWiFi started!");
    Serial.println("Connected to WLAN [" WIFI_SSID "]");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin(WIFI_DEVICE_NAME)) {
      Serial.println("MDNS responder started. You can find the device at [http://" WIFI_DEVICE_NAME ".local]");
    } else {
      Serial.println("*** MDNS responder cannot be started ***");

    }

    setupHTTPServer();
    setupSocketServer();
  }
}

void loop(void) {
  checkAndStartWiFi();
  doHTTPServer();
  doSocketServer();
  motorController.process();
  MDNS.update();
}
