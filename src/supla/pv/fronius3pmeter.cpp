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

#include "fronius3pmeter.h"

using namespace Supla;
using namespace PV;

enum ParametersToRead { NONE,
                        Current_AC_Phase_1,
                        Current_AC_Phase_2,
                        Current_AC_Phase_3,
                        EnergyReactive_VArAC_Sum_Consumed,
                        EnergyReactive_VArAC_Sum_Produced,
                        EnergyReal_WAC_Sum_Consumed,
                        EnergyReal_WAC_Sum_Produced,
                        Frequency_Phase_Average,
                        PowerApparent_S_Phase_1,
                        PowerApparent_S_Phase_2,
                        PowerApparent_S_Phase_3,
                        PowerFactor_Phase_1,
                        PowerFactor_Phase_2,
                        PowerFactor_Phase_3,
                        PowerReactive_Q_Phase_1,
                        PowerReactive_Q_Phase_2,
                        PowerReactive_Q_Phase_3,
                        PowerReal_P_Phase_1,
                        PowerReal_P_Phase_2,
                        PowerReal_P_Phase_3,
                        Voltage_AC_Phase_1,
                        Voltage_AC_Phase_2,
                        Voltage_AC_Phase_3 };

Fronius3pmeter::Fronius3pmeter(IPAddress ip, int port, int deviceId)
    : ip(ip),
      port(port),
      buf(),
      FwdReactEnergy(0),
      RvrReactEnergy(0),
      FwdActEnergy(0),
      RvrActEnergy(0),
      currentPower(0),
      currentActivePower{},
      currentApparentPower{},
      currentReactivePower{},
      currentPowerFactor{},
      currentCurrent{},
      currentFreq(0),
      currentVoltage{},
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

void Fronius3pmeter::iterateAlways() {
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
          // remove last char if it is a comma
          if (buf[bytesCounter - 1] == ','){
            buf[bytesCounter - 1] = '\0';
          } else {
            buf[bytesCounter] = '\0';
          }
          char varName[80];
          char varValue[80];
          sscanf(buf, " %s  : %s", varName, varValue);
          if (strncmp(varName, "Current_AC_Phase_1", strlen("Current_AC_Phase_1")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current current 1: "));
            Serial.println(valueToFetch);
            currentCurrent[0] = valueToFetch * 1000;
          } else if (strncmp(varName, "Current_AC_Phase_2", strlen("Current_AC_Phase_2")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current current 2: "));
            Serial.println(valueToFetch);
            currentCurrent[1] = valueToFetch * 1000;
          } else if (strncmp(varName, "Current_AC_Phase_3", strlen("Current_AC_Phase_3")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current current 3: "));
            Serial.println(valueToFetch);
            currentCurrent[2] = valueToFetch * 1000;
          } else if (strncmp(varName, "EnergyReactive_VArAC_Sum_Consumed", strlen("EnergyReactive_VArAC_Sum_Consumed")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current reactive energy consumed: "));
            Serial.println(valueToFetch);
            FwdReactEnergy = valueToFetch * 100;
          } else if (strncmp(varName, "EnergyReactive_VArAC_Sum_Produced", strlen("EnergyReactive_VArAC_Sum_Produced")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current reactive energy produced: "));
            Serial.println(valueToFetch);
            RvrReactEnergy = valueToFetch * 100;
          } else if (strncmp(varName, "EnergyReal_WAC_Sum_Consumed", strlen("EnergyReal_WAC_Sum_Consumed")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current active energy consumed: "));
            Serial.println(valueToFetch);
            FwdActEnergy = valueToFetch * 100;
          } else if (strncmp(varName, "EnergyReal_WAC_Sum_Produced", strlen("EnergyReal_WAC_Sum_Produced")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current active energy produced: "));
            Serial.println(valueToFetch);
            RvrActEnergy = valueToFetch * 100;
          } else if (strncmp(varName, "Frequency_Phase_Average", strlen("Frequency_Phase_Average")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current frequency: "));
            Serial.println(valueToFetch);
            currentFreq = valueToFetch * 100;
          } else if (strncmp(varName, "PowerApparent_S_Phase_1", strlen("PowerApparent_S_Phase_1")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current power apparent 1: "));
            Serial.println(valueToFetch);
            currentApparentPower[0] = valueToFetch * 100000;
          } else if (strncmp(varName, "PowerApparent_S_Phase_2", strlen("PowerApparent_S_Phase_2")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current power apparent 2: "));
            Serial.println(valueToFetch);
            currentApparentPower[1] = valueToFetch * 100000;
          } else if (strncmp(varName, "PowerApparent_S_Phase_3", strlen("PowerApparent_S_Phase_3")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current power apparent 3: "));
            Serial.println(valueToFetch);
            currentApparentPower[2] = valueToFetch * 100000;
          } else if (strncmp(varName, "PowerFactor_Phase_1", strlen("PowerFactor_Phase_1")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current power factor 1: "));
            Serial.println(valueToFetch);
            currentPowerFactor[0] = valueToFetch * 1000;
          } else if (strncmp(varName, "PowerFactor_Phase_2", strlen("PowerFactor_Phase_2")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current power factor 2: "));
            Serial.println(valueToFetch);
            currentPowerFactor[1] = valueToFetch * 1000;
          } else if (strncmp(varName, "PowerFactor_Phase_3", strlen("PowerFactor_Phase_3")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current power factor 3: "));
            Serial.println(valueToFetch);
            currentPowerFactor[2] = valueToFetch * 1000;
          } else if (strncmp(varName, "PowerReactive_Q_Phase_1", strlen("PowerReactive_Q_Phase_1")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current power reactive 1: "));
            Serial.println(valueToFetch);
            currentReactivePower[0] = valueToFetch * 100000;
          } else if (strncmp(varName, "PowerReactive_Q_Phase_2", strlen("PowerReactive_Q_Phase_2")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current power reactive 2: "));
            Serial.println(valueToFetch);
            currentReactivePower[1] = valueToFetch * 100000;
          } else if (strncmp(varName, "PowerReactive_Q_Phase_3", strlen("PowerReactive_Q_Phase_3")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current power reactive 3: "));
            Serial.println(valueToFetch);
            currentReactivePower[2] = valueToFetch * 100000;
          } else if (strncmp(varName, "PowerReal_P_Phase_1", strlen("PowerReal_P_Phase_1")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current power active 1: "));
            Serial.println(valueToFetch);
            currentActivePower[0] = valueToFetch * 100000;
          } else if (strncmp(varName, "PowerReal_P_Phase_2", strlen("PowerReal_P_Phase_2")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current power active 2: "));
            Serial.println(valueToFetch);
            currentActivePower[1] = valueToFetch * 100000;
          } else if (strncmp(varName, "PowerReal_P_Phase_3", strlen("PowerReal_P_Phase_3")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current power active 3: "));
            Serial.println(valueToFetch);
            currentActivePower[2] = valueToFetch * 100000;
          } else if (strncmp(varName, "Voltage_AC_Phase_1", strlen("Voltage_AC_Phase_1")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current voltage 1: "));
            Serial.println(valueToFetch);
            currentVoltage[0] = valueToFetch * 100;
          } else if (strncmp(varName, "Voltage_AC_Phase_2", strlen("Voltage_AC_Phase_2")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current voltage 2: "));
            Serial.println(valueToFetch);
            currentVoltage[1] = valueToFetch * 100;
          } else if (strncmp(varName, "Voltage_AC_Phase_3", strlen("Voltage_AC_Phase_3")) == 0) {
            valueToFetch = atof(varValue);
            Serial.print(F("Current voltage 3: "));
            Serial.println(valueToFetch);
            currentVoltage[2] = valueToFetch * 100;
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
    setFwdActEnergy(0, FwdActEnergy);
    setRvrActEnergy(0, RvrActEnergy);
    setFwdReactEnergy(0, FwdReactEnergy);
    setRvrReactEnergy(0, RvrReactEnergy);
    setVoltage(0, currentVoltage[0]);
    setVoltage(1, currentVoltage[1]);
    setVoltage(2, currentVoltage[2]);
    setCurrent(0, currentCurrent[0]);
    setCurrent(1, currentCurrent[1]);
    setCurrent(2, currentCurrent[2]);
    setFreq(currentFreq);
    setPowerActive(0, currentActivePower[0]);
    setPowerActive(1, currentActivePower[1]);
    setPowerActive(2, currentActivePower[2]);
    setPowerReactive(0, currentReactivePower[0]);
    setPowerReactive(1, currentReactivePower[1]);
    setPowerReactive(2, currentReactivePower[2]);
    setPowerApparent(0, currentApparentPower[0]);
    setPowerApparent(1, currentApparentPower[1]);
    setPowerApparent(2, currentApparentPower[2]);
    setPowerFactor(0, currentPowerFactor[0]);
    setPowerFactor(1, currentPowerFactor[1]);
    setPowerFactor(2, currentPowerFactor[2]);
    updateChannelValues();
  }
}

bool Fronius3pmeter::iterateConnected(void *srpc) {
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
        strcpy(buf, "GET /solar_api/v1/GetMeterRealtimeData.cgi?Scope=Device&DeviceId=");
        char idBuf[20];
        sprintf(idBuf, "%d", deviceId);
        strcat(buf, idBuf);
        strcat(buf, " HTTP/1.1");
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
          FwdActEnergy = 0;
          RvrActEnergy = 0;
          FwdReactEnergy = 0;
          RvrReactEnergy = 0;
          currentVoltage[0] = 0;
          currentVoltage[1] = 0;
          currentVoltage[2] = 0;
          currentCurrent[0] = 0;
          currentCurrent[1] = 0;
          currentCurrent[2] = 0;
          currentFreq = 0;
          currentActivePower[0] = 0;
          currentActivePower[1] = 0;
          currentActivePower[2] = 0;
          currentReactivePower[0] = 0;
          currentReactivePower[1] = 0;
          currentReactivePower[2] = 0;
          currentApparentPower[0] = 0;
          currentApparentPower[1] = 0;
          currentApparentPower[2] = 0;
          currentPowerFactor[0] = 0;
          currentPowerFactor[1] = 0;
          currentPowerFactor[2] = 0;
          dataIsReady = true;
        }
      }
    }
  }
  return Element::iterateConnected(srpc);
}

void Fronius3pmeter::readValuesFromDevice() {
}

