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

#include "solaredge.h"

#define TEMPERATURE_NOT_AVAILABLE -275

using namespace Supla;
using namespace PV;

// This header should be received from server when data is returned
// It will be verified with actual header if they match. Otherwise, data structure is assumed to be changed
// and it will require this file to be adjusted to support new format
const char headerVerification[] = "date,inverterMode,temperature,totalActivePower,dcVoltage,groundFaultResistance,powerLimit,totalEnergy,vL1To2,vL2To3,vL3To1,L1-acCurrent,L1-acVoltage,L1-acFrequency,L1-apparentPower,L1-activePower,L1-reactivePower,L1-qRef,L1-cosPhi,L2-acCurrent,L2-acVoltage,L2-acFrequency,L2-apparentPower,L2-activePower,L2-reactivePower,L2-qRef,L2-cosPhi,L3-acCurrent,L3-acVoltage,L3-acFrequency,L3-apparentPower,L3-activePower,L3-reactivePower,L3-qRef,L3-cosPhi";

SolarEdge::SolarEdge(const char *apiKeyValue, const char *siteIdValue, const char *inverterSerialNumberValue, Supla::Clock *clock) : 
      buf{},
      totalGeneratedEnergy(0),
      currentPower(0),
      currentCurrent(0),
      currentFreq(0),
      currentVoltage(0),
      bytesCounter(0),
      retryCounter(0),
      startCharFound(false),
      dataIsReady(false),
      dataFetchInProgress(false),
      headerFound(false),
      connectionTimeoutMs(0),
      clock(clock)
{
  // SolarEdge api allows 300 requests daily, so it is one request per almost 5 min
  refreshRateSec = 6*60; // refresh every 6 min

  int len = strlen(apiKeyValue);
  if (len > APIKEY_MAX_LENGTH) {
    len = APIKEY_MAX_LENGTH;
  }
  strncpy(apiKey, apiKeyValue, len);

  len = strlen(siteIdValue);
  if (len > PARAMETER_MAX_LENGTH) {
    len = PARAMETER_MAX_LENGTH;
  }
  strncpy(siteId, siteIdValue, len);

  len = strlen(inverterSerialNumberValue);
  if (len > PARAMETER_MAX_LENGTH) {
    len = PARAMETER_MAX_LENGTH;
  }
  strncpy(inverterSerialNumber, inverterSerialNumberValue, len);

  temperatureChannel.setType(SUPLA_CHANNELTYPE_THERMOMETER);
  temperatureChannel.setDefault(SUPLA_CHANNELFNC_THERMOMETER);
  temperatureChannel.setNewValue(TEMPERATURE_NOT_AVAILABLE);
}

void SolarEdge::iterateAlways() {
  if (dataFetchInProgress) {
    if (millis() - connectionTimeoutMs > 30000) {
      Serial.println(F("SolarEdge: connection timeout. Remote host is not responding"));
      pvClient.stop();
      dataFetchInProgress = false;
      dataIsReady = false;
      return;
    }
    if (!pvClient.connected()) {
      Serial.println(F("SolarEdge fetch completed"));
      dataFetchInProgress = false;
      dataIsReady = true;
    }
    if (pvClient.available()) {
      Serial.print(F("Reading data from SolarEdge: "));
      Serial.println(pvClient.available());
    }
    while (pvClient.available()) {
      char c;
      c = pvClient.read();
      Serial.print(c);
      if (c == '\n') {
        if (bytesCounter > 0) {
          // new line is found with bytesCounter > 0 means that we have full received line of data in buf
          buf[bytesCounter] = '\0'; // add null character at the end of received string

          if (!headerFound) {
            if (0 == strncmp(headerVerification, buf, sizeof(headerVerification)-1)) {
              headerFound = true;
            }
          } else {
            int commaCount = 0;
            for (int i = 0; i < strlen(buf); i++) {
              if (buf[i] == ',') commaCount++;
            }
            // proper line of data should contain at least 34 commas
            if (commaCount >= 34) {
              strtok(buf, ",");
              for (int i = 1; i < 34; i++) {
                char *value = strtok(nullptr, ",");
                Serial.println(value);
                /*
0 date,
1 inverterMode,
2 temperature,
3 totalActivePower,
4 dcVoltage,
5 groundFaultResistance,
6 powerLimit,
7 totalEnergy,
8 vL1To2,
9 vL2To3,
10 vL3To1,
11 L1-acCurrent,
12 L1-acVoltage,
13 L1-acFrequency,
14 L1-apparentPower,
15 L1-activePower,
16 L1-reactivePower,
17 L1-qRef,
18 L1-cosPhi,
19 L2-acCurrent,
20 L2-acVoltage,
21 L2-acFrequency,
22 L2-apparentPower,
23 L2-activePower,
24 L2-reactivePower,
25 L2-qRef,
26 L2-cosPhi,
27 L3-acCurrent,
28 L3-acVoltage,
29 L3-acFrequency,
30 L3-apparentPower,
31 L3-activePower,
32 L3-reactivePower,
33 L3-qRef,
34 L3-cosPhi
*/
                switch (i) {
                  case 2: { // temperature
                    double temperature = atof(value);
                    temperatureChannel.setNewValue(temperature);
                    break;
                  }
                  case 7: // totalEnergy
                   
                    break;

                }
              }
            }

          }

        }
        bytesCounter = 0;
      } else if (bytesCounter < 1023) {   
        buf[bytesCounter] = c;
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

bool SolarEdge::iterateConnected(void *srpc) {
  if (clock && clock->isReady()) {
    if (!dataFetchInProgress) {
      bytesCounter = 0;
      if (lastReadTime == 0 || millis() - lastReadTime > (retryCounter > 0 ? 5000 : refreshRateSec*1000)) {
        lastReadTime = millis();
        Serial.println(F("SolarEdge connecting"));
        pvClient.setBufferSizes(2048, 2048); //
        pvClient.setInsecure();
        int returnCode = 0;
        if (returnCode = pvClient.connect("monitoringapi.solaredge.com", 443)) {
          retryCounter = 0;
          dataFetchInProgress = true;
          connectionTimeoutMs = lastReadTime;
          Serial.println(F("Succesful connect"));

          char buf[200];
          strcpy(buf, "GET /equipment/");

          strcat(buf, siteId);
          strcat(buf, "/");
          strcat(buf, inverterSerialNumber);
          strcat(buf, "/data.csv?startTime=");

          time_t timestamp = time(0); // get current time
          timestamp -= 10*60; // go back in time 10 minutes

          char startTime[30];
          char endTime[30];

          struct tm timeinfo;
          gmtime_r(&timestamp, &timeinfo);

          sprintf(startTime, "%d-%d-%d%%20%d:%d:%d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
          sprintf(endTime, "%d-%d-%d%%2023:59:59", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);

          strcat(buf, startTime);
          strcat(buf, "&endTime=");
          strcat(buf, endTime);
          strcat(buf, "&api_key=");
          strcat(buf, apiKey);
          strcat(buf, " HTTP/1.1");

          Serial.print(F("Query: "));
          Serial.println(buf);
          pvClient.println(buf);
          pvClient.println(F("Host: localhost"));
          pvClient.println(F("Connection: close"));
          pvClient.println();

        } else {  // if connection wasn't successful, try few times. If it fails,
          // then assume that inverter is off during the night
          Serial.print(F("Failed to connect to SolarEdge api, return code: "));
          Serial.println(returnCode);
          retryCounter++;
        }
      }
    }
  }
  return Element::iterateConnected(srpc);
}

void SolarEdge::readValuesFromDevice() {
}

Supla::Channel *SolarEdge::getSecondaryChannel() {
  return &temperatureChannel;
}
