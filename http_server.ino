// C:\Users\dietmar\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.1.0\libraries
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "Yaesu_GS232A.h"
#include "defines.h"

extern ESP8266WebServer server_http;
extern YAESU_GS232A_Class rotator;
extern MotorController_Class motorController;


/** IP to String? */
String toStringIP(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

String createHTML_HeadAndBody(void) {
  String msg = "<html><head>"
               "<meta http-equiv='Content-Type' content='text/html;charset=utf-8'>"
               "<meta name='viewport' content='user-scalable=no, width=device-width, height=device-height, initial-scale=1, maximum-scale=1'>"
               "<script type='text/javascript'>"
               " window.onload = setupOnLoad(); "
               " function setupOnLoad() { setTimeout('refreshPage();', 10000); }; "
               " function refreshPage() { window.location.href = location.href; };"
               "</script></head><body><title>" BUILD_VERSION "</title></body>"
               "<p><a href='/'>Home</a> <a href='/status'>Status</a></p><hr/>";
  return msg;
}

String createHTML_End(void) {
  String msg = "<hr/><p>Aktualisiert: <b><span id='ts_div'>noch nie</span></b></p>"
               "<script type='text/javascript'>document.getElementById('ts_div').innerHTML = new Date().toLocaleString();</script>"
               "</body></html>";
  return msg;
}


void handleRoot() {
  if (!server_http.authenticate(HTTP_USER, HTTP_PASSWORD)) {
    return server_http.requestAuthentication();
  }
  String msg = createHTML_HeadAndBody();
  msg += "<h1>Rotator</h1><table>";
  msg += "<tr><td>current azimuth</td><td>" + String(motorController.getCurrentPos()) + "&deg; </td></tr>";
  msg += "<tr><td>new azimuth</td><td>" + String(motorController.getNewPos()) + "&deg;</td></tr><tr><td>Motor state</td><td>";
  if (motorController.isEmergencyStopped()) {
    msg += "<a href='/nothaltReset'>*** EMERGENCY ***</a>";
  } else if (motorController.isTurning()) {
    msg += "turning";
  } else {
    msg += "stopped";
  }
  msg += "</td></tr><tr><td>New Angle</td><td><form action='/rotate' method='get'>"
         "<input type='text' name='A' value=''>"
         "<input type='submit' value='Rotate'></form></td></table>";
  msg += createHTML_End();
  server_http.send(200, "text/html", msg);
}

void handleStatus(void) {
  String msg = createHTML_HeadAndBody();
  msg += "<h1>Status</h1>";
  msg += "<table>";
  msg += "<tr><td colspan=2><b>" BUILD_VERSION "</b></td></tr>";
  msg += "<tr><td>compiled</td><td>" BUILD_DATE  "</td></tr>";
  msg += "<tr><td>local IP</td><td>" + toStringIP(WiFi.localIP()) + "</td></tr>";
  msg += "<tr><td>RSS</td><td>" + String(WiFi.RSSI()) + " dBm</td></tr>";
  msg += "<tr><td>free heap</td><td>" + String(ESP.getFreeHeap()) + "</td></tr>";
  msg += "<tr><td>uptime</td><td>" + String(millis() / 1000) + " s</td></tr>";
  msg += "<tr><td>analog read delay</td><td>" + String(motorController.analogReadTimeInMs) + "ms</td></tr>";
  msg += "<tr><td>analog read hysteresis</td><td>" + String(motorController.hysteresis) + "&deg;</td></tr>";
  msg += "<tr><td>min. ADC == 0</td><td>" + String(motorController.minADC) + "</td></tr>";
  msg += "<tr><td>max. ADC == 360</td><td>" + String(motorController.maxADC) + "</td></tr>";
  msg += "<tr><td>maximum motor on time</td><td>" + String(motorController.maxMotorOnTimeInMs / 1000) + "s</td></tr>";
  msg += "<tr><td>#emergency stops</td><td>" + String(motorController.numEmergencyStops) + "</td></tr>";
  msg += "<tr><td>#IAC</td><td>" + String(rotator.numCmdIAC) + "</td></tr>";
  msg += "<tr><td>#A</td><td>" + String(rotator.numCmdA) + "</td></tr>";
  msg += "<tr><td>#C</td><td>" + String(rotator.numCmdC) + "</td></tr>";
  msg += "<tr><td>#D</td><td>" + String(rotator.numCmdD) + "</td></tr>";
  msg += "<tr><td>#E</td><td>" + String(rotator.numCmdE) + "</td></tr>";
  msg += "<tr><td>#L</td><td>" + String(rotator.numCmdL) + "</td></tr>";
  msg += "<tr><td>#M</td><td>" + String(rotator.numCmdM) + "</td></tr>";
  msg += "<tr><td>#R</td><td>" + String(rotator.numCmdR) + "</td></tr>";
  msg += "<tr><td>#S</td><td>" + String(rotator.numCmdS) + "</td></tr>";
  msg += "<tr><td>#U</td><td>" + String(rotator.numCmdU) + "</td></tr>";
  msg += "<tr><td>#W</td><td>" + String(rotator.numCmdW) + "</td></tr>";
  msg += "<tr><td>#X</td><td>" + String(rotator.numCmdX) + "</td></tr>";
  msg += "</table>";
  msg += createHTML_End();
  server_http.send(200, "text/html", msg);
}

void handleNothaltReset() {
  if (!server_http.authenticate(HTTP_USER, HTTP_PASSWORD)) {
    return server_http.requestAuthentication();
  }
  motorController.resetEmergencyStop();
  server_http.send(200, "text/html", "<html><head><meta http-equiv='refresh' content='0; url=/' /></head></html>");
}

void handleRotate() {
  if (!server_http.authenticate(HTTP_USER, HTTP_PASSWORD)) {
    return server_http.requestAuthentication();
  }
  if (server_http.args() == 1) {
    if (server_http.argName(0) == "A") {
      int val = server_http.arg(0).toInt();
      motorController.setNewPos(val);
    }
  }
  server_http.send(200, "text/html", "<html><head><meta http-equiv='refresh' content='0; url=/' /></head></html>");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server_http.uri();
  message += "\nMethod: ";
  message += (server_http.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server_http.args();
  message += "\n";
  for (uint8_t i = 0; i < server_http.args(); i++) {
    message += " " + server_http.argName(i) + ": " + server_http.arg(i) + "\n";
  }
  server_http.send(404, "text/plain", message);
}


void setupHTTPServer(void) {
  //  methods secured by basic authentication
  server_http.on("/", handleRoot);
  server_http.on("/rotate", handleRotate);
  server_http.on("/nothaltReset", handleNothaltReset);

  // unsecured methods
  server_http.on("/status", handleStatus);
  server_http.onNotFound(handleNotFound);

  //  start the server
  server_http.begin();
  Serial.print("HTTP server started on ");
  Serial.println(toStringIP(WiFi.localIP()));
  Serial.println(" HTTP user     [" HTTP_USER "]");
  Serial.println(" HTTP password [" HTTP_PASSWORD "]");
}

void doHTTPServer(void) {
  server_http.handleClient();
}
