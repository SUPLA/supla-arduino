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

#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <WiFiClientSecure.h>

#include "../supla_lib_config.h"
#include "network.h"

#define MAX_SSID_SIZE          32
#define MAX_WIFI_PASSWORD_SIZE 64

#ifdef ARDUINO_ARCH_ESP8266
WiFiEventHandler gotIpEventHandler, disconnectedEventHandler;
#endif

// TODO: change logs to supla_log

namespace Supla {
class ESPWifi : public Supla::Network {
 public:
  ESPWifi(const char *wifiSsid = nullptr,
          const char *wifiPassword = nullptr,
          unsigned char *ip = nullptr)
      : Network(ip), client(nullptr), isSecured(true), wifiConfigured(false) {
    ssid[0] = '\0';
    password[0] = '\0';
    setSsid(wifiSsid);
    setPassword(wifiPassword);
#ifdef ARDUINO_ARCH_ESP32
    enableSSL(
        false);  // current ESP32 WiFiClientSecure does not suport "setInsecure"
#endif
  }

  int read(void *buf, int count) {
    _supla_int_t size = client->available();

    if (size > 0) {
      if (size > count) size = count;
      long readSize = client->read((uint8_t *)buf, size);
#ifdef SUPLA_COMM_DEBUG
      Serial.print(F("Received: ["));
      for (int i = 0; i < readSize; i++) {
        Serial.print(static_cast<unsigned char *>(buf)[i], HEX);
        Serial.print(F(" "));
        delay(0);
      }
      Serial.println(F("]"));
#endif

      return readSize;
    }
    return -1;
  }

  int write(void *buf, int count) {
#ifdef SUPLA_COMM_DEBUG
    Serial.print(F("Sending: ["));
    for (int i = 0; i < count; i++) {
      Serial.print(static_cast<unsigned char *>(buf)[i], HEX);
      Serial.print(F(" "));
      delay(0);
    }
    Serial.println(F("]"));
#endif
    long sendSize = client->write((const uint8_t *)buf, count);
    return sendSize;
  }

  int connect(const char *server, int port = -1) {
    String message;
    if (client == NULL) {
      if (isSecured) {
        message = "Secured connection";
        client = new WiFiClientSecure();
        if (fingerprint.length() > 0) {
          message += " with certificate matching";
#ifdef ARDUINO_ARCH_ESP8266
          ((WiFiClientSecure *)client)->setFingerprint(fingerprint.c_str());
#else
          message += " - NOT SUPPORTED ON ESP32 implmentation";
#endif
        } else {
          message += " without certificate matching";
#ifdef ARDUINO_ARCH_ESP8266
          ((WiFiClientSecure *)client)->setInsecure();
#else
          message += " - NOT SUPPORTED ON ESP32 implmentation";
#endif
        }
      } else {
        message = "unsecured connection";
        client = new WiFiClient();
      }
    }

    int connectionPort = (isSecured ? 2016 : 2015);
    if (port != -1) {
      connectionPort = port;
    }

    supla_log(LOG_DEBUG,
              "Establishing %s with: %s (port: %d)",
              message.c_str(),
              server,
              connectionPort);

//       static_cast<WiFiClientSecure*>(client)->setBufferSizes(512, 512); //
    //    EXPERIMENTAL

    bool result = client->connect(server, connectionPort);

    if (result && isSecured) {
      if (!((WiFiClientSecure *)client)->verify(fingerprint.c_str(), server)) {
        supla_log(LOG_DEBUG, "Provided certificates doesn't match!");
        client->stop();
        return false;
      }
    };

    return result;
  }

  bool connected() {
    return (client != NULL) && client->connected();
  }

  bool isReady() {
    return WiFi.status() == WL_CONNECTED;
  }

  void disconnect() {
    if (client != nullptr) {
      client->stop();
    }
  }

  // TODO: add handling of custom local ip
  void setup() {
    if (!wifiConfigured) {
      wifiConfigured = true;
#ifdef ARDUINO_ARCH_ESP8266
      gotIpEventHandler =
          WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &event) {
            (void)(event);
            Serial.print(F("local IP: "));
            Serial.println(WiFi.localIP());
            Serial.print(F("subnetMask: "));
            Serial.println(WiFi.subnetMask());
            Serial.print(F("gatewayIP: "));
            Serial.println(WiFi.gatewayIP());
            long rssi = WiFi.RSSI();
            Serial.print(F("Signal strength (RSSI): "));
            Serial.print(rssi);
            Serial.println(F(" dBm"));
          });
      disconnectedEventHandler = WiFi.onStationModeDisconnected(
          [](const WiFiEventStationModeDisconnected &event) {
            (void)(event);
            Serial.println(F("WiFi station disconnected"));
          });
#else
      WiFiEventId_t event_gotIP = WiFi.onEvent(
          [](WiFiEvent_t event, WiFiEventInfo_t info) {
            Serial.print(F("local IP: "));
            Serial.println(WiFi.localIP());
            Serial.print(F("subnetMask: "));
            Serial.println(WiFi.subnetMask());
            Serial.print(F("gatewayIP: "));
            Serial.println(WiFi.gatewayIP());
            long rssi = WiFi.RSSI();
            Serial.print(F("Signal Strength (RSSI): "));
            Serial.print(rssi);
            Serial.println(F(" dBm"));
          },
          WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);

      WiFiEventId_t event_disconnected = WiFi.onEvent(
          [](WiFiEvent_t event, WiFiEventInfo_t info) {
            Serial.println(F("wifi Station disconnected"));
          },
          WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
#endif
      Serial.print(F("WiFi: establishing connection with SSID: \""));
      Serial.print(ssid);
      Serial.println(F("\""));
      WiFi.begin(ssid, password);
    } else {
      Serial.println(F("WiFi: resetting WiFi connection"));
      if (client) {
        delete client;
        client = nullptr;
      }
      WiFi.reconnect();
    }

    yield();
  }

  void enableSSL(bool value) {
    isSecured = value;
  }

  void setServersCertFingerprint(String value) {
    fingerprint = value;
  }

  void setSsid(const char *wifiSsid) {
    if (wifiSsid) {
      strncpy(ssid, wifiSsid, MAX_SSID_SIZE);
    }
  }

  void setPassword(const char *wifiPassword) {
    if (wifiPassword) {
      strncpy(password, wifiPassword, MAX_WIFI_PASSWORD_SIZE);
    }
  }

  void setTimeout(int timeoutMs) {
    if (client) {
      client->setTimeout(timeoutMs);
    }
  }

  void fillStateData(TDSC_ChannelState &channelState) {
    channelState.Fields |= SUPLA_CHANNELSTATE_FIELD_IPV4 |
                           SUPLA_CHANNELSTATE_FIELD_MAC |
                           SUPLA_CHANNELSTATE_FIELD_WIFIRSSI |
                           SUPLA_CHANNELSTATE_FIELD_WIFISIGNALSTRENGTH;
    channelState.IPv4 = WiFi.localIP();
    WiFi.macAddress(channelState.MAC);
    int rssi = WiFi.RSSI();
    channelState.WiFiRSSI = rssi;
    if (rssi > -50) {
      channelState.WiFiSignalStrength = 100;
    } else if (rssi <= -100) {
      channelState.WiFiSignalStrength = 0;
    } else {
      channelState.WiFiSignalStrength = 2 * (rssi + 100);
    }
  }

 protected:
  WiFiClient *client = NULL;
  bool isSecured;
  bool wifiConfigured;
  String fingerprint;
  char ssid[MAX_SSID_SIZE];
  char password[MAX_WIFI_PASSWORD_SIZE];
};

};  // namespace Supla

#endif
