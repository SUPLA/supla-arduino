/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "fronius.h"

using namespace Supla;
using namespace PV;

enum ParametersToRead { NONE, TOTAL_ENERGY, FAC, IAC, PAC, UAC };

Fronius::Fronius(IPAddress ip, int port, int deviceId)
    : ip(ip),
      port(port),
      buf(),
      totalGeneratedEnergy(0),
      currentPower(0),
      currentCurrent(0),
      currentFreq(0),
      currentVoltage(0),
      bytesCounter(0),
      retryCounter(0),
      valueToFetch(NONE),
      deviceId(deviceId),
      startCharFound(false),
      dataIsReady(false),
      dataFetchInProgress(false),
      connectionTimeoutMs(0) {
  refreshRateSec = 15;
}

void Fronius::iterateAlways() {
  if (dataFetchInProgress) {
    if (millis() - connectionTimeoutMs > 30000) {
      Serial.println(F("Fronius: connection timeout. Remote host is not responding"));
      pvClient.stop();
      dataFetchInProgress = false;
      dataIsReady = false;
      return;
    }
    if (!pvClient.connected()) {
      Serial.println(F("Fronius fetch completed"));
      dataFetchInProgress = false;
      dataIsReady = true;
    }
    if (pvClient.available()) {
      Serial.print(F("Reading data from Fronius: "));
      Serial.println(pvClient.available());
    }
    while (pvClient.available()) {
      char c;
      c = pvClient.read();
      if (c == '\n') {
        if (startCharFound) {
          if (bytesCounter > 79) bytesCounter = 79;
          buf[bytesCounter] = '\0';
          char varName[80];
          char varValue[80];
          sscanf(buf, " %s  : %s", varName, varValue);
          if (valueToFetch != NONE && strncmp(varName, "Value", strlen("Value")) == 0) {
            switch (valueToFetch) {
              case TOTAL_ENERGY: {
                float totalProd = atof(varValue);
                Serial.print(F("Total production: "));
                Serial.print(totalProd);
                Serial.println(F(" Wh"));
                totalGeneratedEnergy = totalProd * 100;

                break;
              }
              case PAC: {
                float curPower = atof(varValue);
                Serial.print(F("Current power: "));
                Serial.println(curPower);
                currentPower = curPower * 100000;

                break;
              }
              case IAC: {
                float curCurrent = atof(varValue);
                Serial.print(F("Current: "));
                Serial.println(curCurrent);
                currentCurrent = curCurrent * 1000;

                break;
              }
              case FAC: {
                float curFreq = atof(varValue);
                Serial.print(F("Frequency: "));
                Serial.println(curFreq);
                currentFreq = curFreq * 100;

                break;
              }
              case UAC: {
                float curVoltage = atof(varValue);
                Serial.print(F("Voltage: "));
                Serial.println(curVoltage);
                currentVoltage = curVoltage * 100;

                break;
              }
            }
            valueToFetch = NONE;
          } else if (strncmp(varName, "TOTAL_ENERGY\"", strlen("TOTAL_ENERGY")) == 0) {
            valueToFetch = TOTAL_ENERGY;
          } else if (strncmp(varName, "FAC", strlen("FAC")) == 0) {
            valueToFetch = FAC;
          } else if (strncmp(varName, "UAC", strlen("UAC")) == 0) {
            valueToFetch = UAC;
          } else if (strncmp(varName, "IAC", strlen("IAC")) == 0) {
            valueToFetch = IAC;
          } else if (strncmp(varName, "PAC", strlen("PAC")) == 0) {
            valueToFetch = PAC;
          }
        }
        bytesCounter = 0;
        startCharFound = false;
      } else if (c == '"' || startCharFound) {
        startCharFound = true;
        if (c == '"') {
          c = ' ';
        }
        if (bytesCounter < 80) {
          buf[bytesCounter] = c;
        }
        bytesCounter++;
      }
    }
    if (!pvClient.connected()) {
      pvClient.stop();
    }
  }
  if (dataIsReady) {
    dataIsReady = false;
    setFwdActEnergy(0, totalGeneratedEnergy);
    setPowerActive(0, currentPower);
    setCurrent(0, currentCurrent);
    setVoltage(0, currentVoltage);
    setFreq(currentFreq);
    updateChannelValues();
  }
}

bool Fronius::iterateConnected(void *srpc) {
  if (!dataFetchInProgress) {
    if (lastReadTime == 0 || millis() - lastReadTime > refreshRateSec*1000) {
      lastReadTime = millis();
      Serial.print(F("Fronius connecting "));
      Serial.println(deviceId);
      if (pvClient.connect(ip, port)) {
        retryCounter = 0;
        dataFetchInProgress = true;
        connectionTimeoutMs = lastReadTime;
        Serial.println(F("Succesful connect"));

        char buf[100];
        strcpy(buf, "GET /solar_api/v1/GetInverterRealtimeData.cgi?Scope=Device&DeviceID=");
        char idBuf[20];
        sprintf(idBuf, "%d", deviceId);
        strcat(buf, idBuf);
        strcat(buf, "&DataCollection=CommonInverterData HTTP/1.1");
        pvClient.println(buf);
        pvClient.println("Host: localhost");
        pvClient.println("Connection: close");
        pvClient.println();

      } else {  // if connection wasn't successful, try few times. If it fails,
                // then assume that inverter is off during the night
        Serial.print(F("Failed to connect to Fronius at: "));
        Serial.print(ip);
        Serial.print(F(":"));
        Serial.println(port);
        retryCounter++;
        if (retryCounter > 3) {
          currentPower = 0;
          currentFreq = 0;
          currentCurrent = 0;
          currentVoltage = 0;
          dataIsReady = true;
        }
      }
    }
  }
  return Element::iterateConnected(srpc);
}

void Fronius::readValuesFromDevice() {
}

