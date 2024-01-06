#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include "Yaesu_GS232A.h"

extern WiFiServer server_socket;
extern YAESU_GS232A_Class rotator;

void setupSocketServer(void) {
  server_socket.begin();
  Serial.println("Socket server started");
}


void doSocketServer(void) {
  WiFiClient client = server_socket.available();
  if (client) {
    digitalWrite(LED_BUILTIN, LOW);
    rotator.processCommands(client);
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

