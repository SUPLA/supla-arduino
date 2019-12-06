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

#ifndef esp_wifi_h__
#define esp_wifi_h__

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "../../supla_lib_config.h"
#include "network.h"

#define MAX_SSID_SIZE          32
#define MAX_WIFI_PASSWORD_SIZE 64

// TODO: change logs to supla_log

namespace Supla {
class ESPWifi : public Supla::Network {
 public:
  ESPWifi(const char *wifiSsid, const char *wifiPassword, IPAddress *ip = NULL) : Network(ip) {
    if (netIntf != NULL) {
      Serial.println(
          "ESPWifi: Error - network interface already defined! Overwriting");
    }
    strcpy(ssid, wifiSsid);
    strcpy(password, wifiPassword);
    if (strlen(ssid) == 0) {
      Serial.println("ESPWifi: Error - Empty SSID");
    }
    if (strlen(password) == 0) {
      Serial.println("ESPWifi: Error - Empty WiFi password");
    }
  }

  int read(void *buf, int count) {
    _supla_int_t size = client.available();

    if (size > 0) {
      if (size > count) size = count;
      long readSize = client.read((uint8_t *)buf, size);
#ifdef SUPLA_COMM_DEBUG
      Serial.print("Received: [");
      for (int i = 0; i < readSize; i++) {
        Serial.print(static_cast<unsigned char *>(buf)[i], HEX);
        Serial.print(" ");
      }
      Serial.println("]");
#endif

      return readSize;
    }
    return -1;
  }

  int write(void *buf, int count) {
#ifdef SUPLA_COMM_DEBUG
    Serial.print("Sending: [");
    for (int i = 0; i < count; i++) {
      Serial.print(static_cast<unsigned char *>(buf)[i], HEX);
      Serial.print(" ");
    }
    Serial.println("]");
#endif
    long sendSize = client.write((const uint8_t *)buf, count);
    return sendSize;
  }

  bool connect(const char *server, int port) {
    return client.connect(server, port);
  }

  bool connected() {
    return client.connected();
  }

  void disconnect() {
    client.stop();
  }

  // TODO: add handling of custom local ip
  void setup() {
    Serial.print("Connecting to network: \"");
    Serial.print(ssid);
    Serial.println("\"");
    WiFi.begin(ssid, password);

    // TODO: Change to not blocking
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }

    Serial.print("localIP: ");
    Serial.println(WiFi.localIP());
    Serial.print("subnetMask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("gatewayIP: ");
    Serial.println(WiFi.gatewayIP());
  }

 protected:
  WiFiClient client;

  char ssid[MAX_SSID_SIZE];
  char password[MAX_WIFI_PASSWORD_SIZE];
};

};  // namespace Supla

#endif
