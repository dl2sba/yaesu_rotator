#include <arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "MotorController.h"
#include "Yaesu_GS232A.h"

extern MotorController_Class motorController;

void YAESU_GS232A_Class::init(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("YAESU_GS232A started");
}


void YAESU_GS232A_Class::processCommands(WiFiClient & client) {
#define TCP_TIMEOUT 5000
  uint8_t state = 0;
  char    buffer[10];
  uint8_t bufferIndex;
  uint32_t lastMillis = millis();

  while ( client.available() == 0 ) {
    if ( millis() - lastMillis > TCP_TIMEOUT ) {
      Serial.println("*** Timeout occured");
      client.stop();
      Serial.println("*** client stopped");
      return;
    }
  }

  do {
    int inthex = client.read();
    if ( inthex != -1 ) {
      char hex = inthex & 0xff;

      switch ( state ) {
        case 0:
          if ( hex == GS232A_CMD_IAC ) {
            ++numCmdIAC;
            state = 1;
          } else if ( hex == 'C' ) {
            ++numCmdC;
            state = 10;
          } else if ( hex == 'W' ) {
            ++numCmdW;
            state = 20;
          } else if ( hex == 'R' ) {
            ++numCmdR;
            state = 30;
          } else if ( hex == 'L' ) {
            ++numCmdL;
            state = 40;
          } else if ( hex == 'S' ) {
            ++numCmdS;
            state = 50;
          } else if ( hex == 'A' ) {
            ++numCmdA;
            state = 50;
          } else if ( hex == 'E' ) {
            ++numCmdE;
            state = 50;
          } else if ( hex == 'M' ) {
            ++numCmdM;
            state = 60;
          } else if ( hex == 'U' ) {
            ++numCmdU;
            state = 70;
          } else if ( hex == 'D' ) {
            ++numCmdD;
            state = 80;
          } else if ( hex == 'X' ) {
            ++numCmdX;
            state = 90;
          } else {
            Serial.print(state);
            Serial.print(": [");
            Serial.print(hex);
            Serial.print("] ");
            Serial.println(hex, HEX);
            state = 99;
          }
          break;

        case 1:
          if ( hex == GS232A_CMD_NOP ) {
            state = 0;
          } else if ( hex == GS232A_CMD_IAC ) {
            state = 0;
          } else if ( hex == GS232A_CMD_AYT ) {
            state = 0;
          } else if ( hex == GS232A_CMD_SB ) {
            state = 2;
          } else {
            Serial.print(state);
            Serial.print(": *** unknown ");
            Serial.println(hex, HEX);
          }
          break;

        case 2:
          if ( hex == GS232A_CMD_IAC ) {
            state = 3;
          }
          break;

        case 3:
          if ( hex != GS232A_CMD_SE ) {
            Serial.print(state);
            Serial.print(": *** unknown");
            Serial.println(hex, HEX);
          }
          state = 99;
          break;

        case 10:
          if ( hex == '2' ) {
            state = 11;
          } else if ( hex == 0x0d ) {
            size_t bufLen = sprintf(buffer, "+0%03d\r\n", motorController.getCurrentPos());
            client.write((uint8_t *) buffer, bufLen);
            Serial.print("C ");
            Serial.print(buffer);
            state = 99;
          } else {
            state = 99;
          }
          break;

        case 11:
          if ( hex == 0x0d ) {
            size_t bufLen = sprintf(buffer, "+0%03d+0%03d\r\n", motorController.getCurrentPos(), 0);
            client.write((uint8_t *) buffer, bufLen);
            Serial.print("C2 ");
            Serial.print(buffer);
            state = 99;
          }
          break;

        case 20:
          bufferIndex = 0;
          if ( hex >= '0' && hex <= '9') {
            buffer[bufferIndex++] = hex;
            state = 21;
          } else {
            state = 99;
          }
          break;

        case 21:
          if ( hex >= '0' && hex <= '9') {
            buffer[bufferIndex++] = hex;
          }
          if ( bufferIndex == 3 ) {
            buffer[bufferIndex] = 0;
            motorController.setNewPos(atoi(buffer));
            state = 22;
          }
          break;

        case 22:
          if ( hex == ' ' ) {
            state = 23;
          } break;

        case 23:
          bufferIndex = 0;
          if ( hex >= '0' && hex <= '9') {
            buffer[bufferIndex++] = hex;
            state = 24;
          } else {
            state = 99;
          }
          break;

        case 24:
          if ( hex >= '0' && hex <= '9') {
            buffer[bufferIndex++] = hex;
          }
          if ( bufferIndex == 3 ) {
            buffer[bufferIndex] = 0;
            state = 25;
          }
          break;

        case 25:
          if ( hex == 0x0d ) {
            motorController.setNewPos(atoi(buffer));
            client.write('\n');
            client.write('\r');
          }
          state = 99;
          break;

        case 30:
          if ( hex == 0x0d ) {
            Serial.println("R command");
            client.write('\n');
            client.write('\r');
          }
          state = 99;
          break;

        case 40:
          if ( hex == 0x0d ) {
            Serial.println("L command");
            client.write('\n');
            client.write('\r');
          }
          state = 99;
          break;

        case 50:
          if ( hex == 0x0d ) {
            Serial.println("S command detected");
            client.write('\n');
            client.write('\r');
          }
          state = 99;
          break;

        case 60:
          bufferIndex = 0;
          if ( hex >= '0' && hex <= '9') {
            buffer[bufferIndex++] = hex;
            state = 61;
          } else {
            state = 99;
          }
          break;

        case 61:
          if ( hex >= '0' && hex <= '9') {
            buffer[bufferIndex++] = hex;
            if ( bufferIndex == 3 ) {
              buffer[bufferIndex] = 0;
              state = 62;
            }
          } else {
            state = 99;
          }
          break;

        case 62:
          if ( hex == 0x0d ) {
            motorController.setNewPos(atoi(buffer));
            Serial.print("M az=");
            Serial.println(buffer);
            client.write('\n');
            client.write('\r');
          }
          state = 99;
          break;

        case 70:
          if ( hex == 0x0d ) {
            Serial.println("U command ignored");
            client.write('\n');
            client.write('\r');
          }
          state = 99;
          break;

        case 80:
          if ( hex == 0x0d ) {
            Serial.println("D command ignored");
            client.write('\n');
            client.write('\r');
          }
          state = 99;
          break;

        case 90:
          bufferIndex = 0;
          if ( hex >= '0' && hex <= '9') {
            buffer[bufferIndex++] = hex;
            state = 91;
          } else {
            state = 99;
          }
          break;

        case 91:
          if ( hex == 0x0d ) {
            Serial.println("X command ignored");
            client.write('\n');
            client.write('\r');
          }
          state = 99;
          break;

        default:
          Serial.print(state);
          Serial.print(": *** unknown");
          Serial.println(hex, HEX);
          state = 99;
          break;
      }
    }
  } while ( state != 99);
}

/***

void YAESU_GS232A_Class::doLeft(void) {
  azimuth_target -= G323A_ROTATE_STEP;
  if (azimuth_target < 0 ) azimuth_target = 0;
  nothalt = LOW;
}

void YAESU_GS232A_Class::doRight(void) {
  azimuth_target += G323A_ROTATE_STEP;
  if (azimuth_target > G323A_MAX_ANGLE) azimuth_target = G323A_MAX_ANGLE;
  nothalt = LOW;
}

void YAESU_GS232A_Class::doNothalt(void) {
  Serial.println("*** NOTHALT triggered ***");
  azimuth_target = azimuth;
  digitalWrite(GS232A_RELAIS_LEFT, LOW);
  digitalWrite(GS232A_RELAIS_RIGHT, LOW);
  nothalt = HIGH;
  turning = LOW;
}

***/


