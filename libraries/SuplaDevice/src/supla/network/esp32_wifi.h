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
#include <WiFi.h>

#include "../supla_lib_config.h"
#include "network.h"

#define MAX_SSID_SIZE          32
#define MAX_WIFI_PASSWORD_SIZE 64

// TODO: change logs to supla_log

namespace Supla {
class ESP32Wifi : public Supla::Network {
 public:
  ESP32Wifi(const char *wifiSsid,
            const char *wifiPassword,
            IPAddress *ip = NULL)
      : Network(ip) {
    strcpy(ssid, wifiSsid);
    strcpy(password, wifiPassword);
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

  int connect(const char *server, int port = -1) {
    int connectionPort = (port == -1 ? 2015 : port);
    supla_log(
        LOG_DEBUG, "Establishing connection with: %s (port: %d)", server, connectionPort);
    return client.connect(server, connectionPort);
  }

  bool connected() {
    return client.connected();
  }

  bool isReady() {
    return WiFi.status() == WL_CONNECTED;
  }

  void disconnect() {
    client.stop();
  }

  // TODO: add handling of custom local ip
  void setup() {
    WiFiEventId_t event_gotIP = WiFi.onEvent(
        [](WiFiEvent_t event, WiFiEventInfo_t info) {
          Serial.print("local IP: ");
          Serial.println(WiFi.localIP());
          Serial.print("subnetMask: ");
          Serial.println(WiFi.subnetMask());
          Serial.print("gatewayIP: ");
          Serial.println(WiFi.gatewayIP());
          long rssi = WiFi.RSSI();
          Serial.print("Signal Strength (RSSI): ");
          Serial.print(rssi);
          Serial.println(" dBm");
        },
        WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);

    WiFiEventId_t event_disconnected = WiFi.onEvent(
        [](WiFiEvent_t event, WiFiEventInfo_t info) {
          Serial.println("wifi Station disconnected");
        },
        WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

    Serial.print("WIFI: establishing connection with SSID: \"");
    Serial.print(ssid);
    Serial.println("\"");
    WiFi.begin(ssid, password);
    yield();
  }

 protected:
  WiFiClient client;

  char ssid[MAX_SSID_SIZE];
  char password[MAX_WIFI_PASSWORD_SIZE];
};

};  // namespace Supla

#endif
